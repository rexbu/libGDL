/**
 * file :	bs_data.c
 * author :	Rex
 * create :	2016-11-15 17:53
 * func : 
 * history:
 */

#include "bs_data.h"

void* data_init(void* p){
    data_t* s = (data_t*)p;
    s->mem = (char*)malloc(STRING_DEF_SIZE);
    memset(s->mem, 0, STRING_DEF_SIZE);
    s->size = STRING_DEF_SIZE;
    s->len = 0;
    s->set = data_set;
    s->append = data_append;
    return s;
}

void data_destroy(void* p){
    data_t* s = (data_t*)p;
    if (s->mem != NULL)
    {
        free(s->mem);
    }
    s->mem = NULL;
    s->size = 0;
}

status_t data_set(struct data_t* str, const char* s, uint32_t size){
    if (str->size <= size) {
        size_t relen = size+1; // 如果len=1，需要额外加1个字节，所有后面+1
        // realloc: 如果有足够连续空间，直接在当前指针上扩展空间，如果不足则重新申请空间，同时会释放原空间
        char* pnew = (char*)realloc(str->mem, relen);
        if (pnew==NULL)
        {
            return BS_NOMEM;
        }
        
        str->mem = pnew;
        str->size = (uint32_t)relen;
    }
    memcpy(str->mem, s, size);
    str->len = size;
    str->mem[str->len] = '\0';
    
    return (int)size;
}
status_t data_append(data_t* str, const char* s, uint32_t size){
    if (str->size <= str->len+size)
    {
        size_t relen = size*3/2+1; // 如果len=1，需要额外加1个字节，所有后面+1
        // realloc: 如果有足够连续空间，直接在当前指针上扩展空间，如果不足则重新申请空间，同时会释放原空间
        char* pnew = (char*)realloc(str->mem, str->size + relen);
        if (pnew==NULL){
            return BS_NOMEM;
        }
        
        str->mem = pnew;
        str->size += relen;
    }
    memcpy(str->mem+str->len, s, size);
    str->len += size;
    str->mem[str->len] = '\0';
    
    return str->size;
}
