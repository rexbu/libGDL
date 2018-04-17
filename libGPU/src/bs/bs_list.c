#include "bs_list.h"

state_t _list_init(_list_t* list, uint32_t size, uint32_t esize, bool_t lock){
    assert(list!=NULL && size>0);
    list->head.prev = NULL;
    list->head.next = &list->rear;
    list->rear.prev = &list->head;
    list->rear.next = NULL;
    list->size = 0;
    list->esize = esize;
    list->islock = lock;
    if(lock){
        if(pthread_rwlock_init(&list->lock, NULL)!=0){
            return BS_INVALID;
        }
    }
    return bs_pool_init(&list->pool, size, sizeof(_list_head_t)+esize, BS_FALSE);
}

void* _list_insert_head(_list_t* list, void* elem){
    _list_head_t*       pe;

    assert(list!=NULL && elem!=NULL);
    
    if(list->islock){
        pthread_rwlock_wrlock(&list->lock);
    }
    pe = (_list_head_t*)pool_malloc(&list->pool);
    if(pe == NULL){
        return NULL;
    }

    pe->prev = &list->head;
    pe->next = list->head.next;
    list->head.next->prev = pe;
    list->head.next = pe;
    memcpy(pe+1, elem, list_elen(list));
    list->size++;
    if(list->islock){
        pthread_rwlock_unlock(&list->lock);
    }
    
    return pe+1;
}

void* _list_insert_rear(_list_t* list, void* elem){
    _list_head_t*       pe;

    assert(list!=NULL);
    
    if(list->islock){
        pthread_rwlock_wrlock(&list->lock);
    }
    pe = (_list_head_t*)pool_malloc(&list->pool);
    if(pe == NULL){
        return NULL;
    }

    pe->next = &list->rear;
    pe->prev = list->rear.prev;
    list->rear.prev->next = pe;
    list->rear.prev = pe;
    if(elem==NULL){
        memset(pe+1, 0, list_elen(list));
    }
    else{
        memcpy(pe+1, elem, list_elen(list));
    }
    list->size++;
    if(list->islock){
        pthread_rwlock_unlock(&list->lock);
    }
    
    return pe+1;
}

void* _list_insert_next(_list_t* list, void* cur, void* elem){
    _list_head_t*       pe = list_addr(cur);
    _list_head_t*       pnew;

    assert(list!=NULL && cur!=NULL);

    if(list->islock){
        pthread_rwlock_wrlock(&list->lock);
    }
    pnew = (_list_head_t*)pool_malloc(&list->pool);
    if(pnew == NULL){
        return NULL;
    }

    pnew->next = pe->next;
    pnew->prev = pe;
    pe->next->prev = pnew;
    pe->next = pnew;
    if(elem==NULL){
        memset(pnew+1, 0, list_elen(list));
    }
    else{
        memcpy(pnew+1, elem, list_elen(list));
    }
    list->size++;
    if(list->islock){
        pthread_rwlock_unlock(&list->lock);
    }

    return pnew+1;
}

void* _list_insert_prev(_list_t* list, void* cur, void* elem){
    _list_head_t*       pe = list_addr(cur);
    _list_head_t*       pnew;

    assert(list!=NULL && cur!=NULL);

    if(list->islock){
        pthread_rwlock_wrlock(&list->lock);
    }
    pnew = (_list_head_t*)pool_malloc(&list->pool);
    if(pnew == NULL){
        return NULL;
    }

    pnew->prev = pe->prev;
    pnew->next = pe;
    pe->prev->next = pnew;
    pe->prev = pnew;
    if(elem==NULL){
        memset(pnew+1, 0, list_elen(list));
    }
    else{
        memcpy(pnew+1, elem, list_elen(list));
    }
    list->size++;
    if(list->islock){
        pthread_rwlock_unlock(&list->lock);
    }

    return pnew+1;
}

state_t _list_remove(_list_t* list, void* elem){
    _list_head_t*       pe = list_addr(elem);

    assert(list!=NULL);
    if(elem==NULL){
        return BS_SUCCESS;
    }
    if(list->size<=0){
        return BS_INVALID;
    }
    
    if(list->islock){
        pthread_rwlock_wrlock(&list->lock);
    }
    pe->next->prev = pe->prev;
    pe->prev->next = pe->next;

    pool_free(&list->pool, pe);
    list->size--;
    if(list->islock){
        pthread_rwlock_unlock(&list->lock);
    }

    return BS_SUCCESS;
}

state_t _list_clear(_list_t* list){
    void*           p;

    assert(list!=NULL);
    
    if(list->islock){
        pthread_rwlock_wrlock(&list->lock);
    }
    for(p=list_head(list); list_forward(p); p=list_next(p)){
        _list_remove(list, p);
    }
    if(list->islock){
        pthread_rwlock_unlock(&list->lock);
    }

    return BS_SUCCESS;
}

void* _list_search(_list_t* list, const void* elem, list_compare_f compare){
    void*       p;

    assert(list!=NULL);
    
    if(list->islock){
        pthread_rwlock_rdlock(&list->lock);
    }
    for(p=list_head(list); list_forward(p); p=list_next(p)){
        if(compare(elem, (const void*)p) == 0){
            if(list->islock){
                pthread_rwlock_unlock(&list->lock);
            }
            return p;
        }
    }
    if(list->islock){
        pthread_rwlock_unlock(&list->lock);
    }

    return NULL;
}

void* _list_index(_list_t* list, uint32_t id){
    void*       p = list_head(list);
    uint32_t    i;

    assert(list!=NULL);
    
    if(list->islock){
        pthread_rwlock_rdlock(&list->lock);
    }
    for(i=0; i<id && list_forward(p); p=list_next(p),i++){
    }
    
    if(list_forward(p) && i==id){
        if(list->islock){
            pthread_rwlock_unlock(&list->lock);
        }
        return p;
    }
    if(list->islock){
        pthread_rwlock_unlock(&list->lock);
    }

    return NULL;
}

void* _list_set_index(_list_t* list, uint32_t idx, void* elem){
    int         i;
    void*       p;

    p = _list_index(list, idx);
    // 如果idx超出当前节点数，则插入NULL补充
    if(p==NULL && idx>=list->size){
        for(i = list->size; i<idx; i++){
            list_insert(list, NULL);
        }
        return list_insert(list, elem);
    }
    else if(p==NULL){
        return NULL;
    }

    if(list->islock){
        pthread_rwlock_wrlock(&list->lock);
    }
    if(elem==NULL){
        memset(p, 0, list_elen(list));
    }
    else{
        memcpy(p, elem, list_elen(list));
    }
    if(list->islock){
        pthread_rwlock_unlock(&list->lock);
    }
    
    return p;
}

void* _list_set(_list_t* list, const void* key, void* data, list_compare_f compare){
    void*       p = list_search(list, key, compare);
    
    if(p==NULL){
        return _list_insert_rear(list, data);
    }
    else{
        memcpy(p, data, list_elen(list));
        return p;
    }
}
