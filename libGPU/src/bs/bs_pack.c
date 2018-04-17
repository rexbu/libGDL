/**
 * file :	bs_pack.c
 * author :	bushaofeng
 * create :	2013-11-21 17:54
 * func : 
 * history:	
 *          2013-11-28 pack buffer改为外部传入，key与value全部存储在buffer中，不再动态创建
 */

#include "bs_pack.h"

static void* _pack_add_buf(char* buf, uint32_t *size, const void* info, uint32_t info_len, char split);

state_t bs_pack_init(bs_pack_t* pack, char* buf, uint32_t size){
    int head_len = 2*sizeof(uint32_t)+1;

    assert(pack!=NULL && buf!=NULL && size>head_len);
    pack->buf = buf;
    pack->size = size;
    pack->len = head_len;
    pack->item_len = 0;
    pack->dyn = BS_FALSE;
    // pack头，第一个int为总长度，第二个int为item数量
    *(uint32_t*)buf = head_len;
    *(uint32_t*)(buf+sizeof(uint32_t)) = 0;
    buf[head_len-1] = 0;

    return BS_SUCCESS;
}

state_t bs_pack_init_dyn(bs_pack_t* pack, uint32_t size){
    char* buf = (char*)malloc(size);
    if(buf==NULL){
        return BS_NOMEM;
    }

    bs_pack_init(pack, buf, size);
    pack->dyn = BS_TRUE;
    return BS_SUCCESS;
}

state_t bs_pack_add(bs_pack_t* pack, const char* key, const void* value, uint32_t value_size){
    bs_pack_item_t* item;
    assert(pack!=NULL && key!=NULL && value!=NULL);
    if(pack->item_len>=BS_PACK_ITEMSIZE || strlen(key)+value_size + 2*sizeof(uint32_t)+2 + pack->len > pack->size){
        return BS_NOBUFFER;
    }
    
    item = &pack->item[pack->item_len];
    item->key = _pack_add_buf(pack->buf, &pack->len, key, strlen(key), 0);
    item->key_size = strlen(key);
    
    item->value = _pack_add_buf(pack->buf, &pack->len, value, value_size, 0);
    item->value_size = value_size;
    pack->item_len++;

    memcpy(pack->buf, &pack->len, sizeof(uint32_t));
    memcpy(pack->buf+sizeof(uint32_t), &pack->item_len, sizeof(uint32_t));
    return BS_SUCCESS;
}

int bs_pack_getint(bs_pack_t* pack, const char* key){
    int* p = (int*)bs_pack_get(pack, key);
    if(p==NULL){
        return -1;
    }

    return *p;
}

char* bs_pack_getstr(bs_pack_t* pack, const char* key){
    return (char*)bs_pack_get(pack, key);
}

void* bs_pack_get(bs_pack_t* pack, const char* key){
    uint32_t    size = 0;
    return bs_pack_get_size(pack, key, &size);
}

void* bs_pack_get_size(bs_pack_t* pack, const char* key, uint32_t* size){
    bs_pack_item_t*     item;
    int                 i;

    assert(pack!=NULL && key!=NULL);
    for(i=0; i<pack->item_len; i++){
        item = &pack->item[i];
        if(strcmp(key, item->key)==0){
            *size = item->value_size;
            return item->value;
        }
    }
    
    return NULL;
}

state_t bs_pack_set(bs_pack_t* pack, const char* key, const void* value, uint32_t size){
    void*           val;
    uint32_t        val_size;
    
    val = bs_pack_get_size(pack, key, &val_size);
    if(val==NULL){
        return bs_pack_add(pack, key, value, size);
    }
    else if(val_size != size){
        // 目前只支持长度相同时才能set，否则parse会出错
        return BS_PARAERR;
    }
    
    memcpy(val, value, size);
    return BS_SUCCESS;
}

state_t bs_pack_parse(bs_pack_t* pack, char* buf, uint32_t buf_size){
    uint32_t        item_size = 0;
    uint32_t        size = *((uint32_t*)buf);
    bs_pack_item_t* item;

    assert(pack!=NULL && buf!=NULL);
    if(size > buf_size){
        return BS_NOBUFFER;
    }
    
    pack->dyn = BS_FALSE;
    pack->buf = buf;
    pack->size = buf_size;
    pack->item_len = 0;
    item_size = *((uint32_t*)(buf+sizeof(uint32_t)));
    pack->len = sizeof(uint32_t)*2 + 1;
    while(pack->item_len<item_size && pack->item_len<BS_PACK_ITEMSIZE){
        item = &pack->item[pack->item_len];
        memcpy(&item->key_size, buf+pack->len, sizeof(item->key_size));
        pack->len += sizeof(item->key_size);
        item->key = buf+pack->len;
        pack->len += (item->key_size+1);

        memcpy(&item->value_size, buf+pack->len, sizeof(item->value_size));
        pack->len+= sizeof(item->value_size);
        item->value = buf+pack->len;
        pack->len += (item->value_size+1);
        pack->item_len++;
    }

    if(pack->len != size || pack->item_len!=item_size){
        return BS_PARAERR;
    }

    return BS_SUCCESS;
}

uint32_t bs_pack_getbufsize(const char* buf){
    assert(buf!=NULL);
    return *(uint32_t*)buf;
}

state_t bs_pack_destroy(bs_pack_t* pack){
    assert(pack!=NULL);
    if(pack->dyn){
        free(pack->buf);
    }

    return BS_SUCCESS;
}

static void* _pack_add_buf(char* buf, uint32_t* size, const void* info, uint32_t info_len, char split){
    void* ptr;

    assert(buf!=NULL && info!=NULL);
    memcpy(buf+*size, &info_len, sizeof(info_len));
    *size += sizeof(info_len);
    ptr = buf+*size;
    memcpy(ptr, info, info_len);
    *size += info_len;
    memcpy(buf+*size, &split, sizeof(split));
    *size += 1;

    return ptr;
}

