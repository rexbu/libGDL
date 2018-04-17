/**
 * file :	bs_cqueue.c
 * author :	bushaofeng
 * create :	2014-01-22 18:24
 * func : 
 * history:
 */

#include "bs_cqueue.h"
#include "bs_common.h"

state_t bs_cqueue_init(_cqueue_t* c, uint32_t size, uint32_t esize, bool_t islock){
    assert(c!=NULL);
    memset(c, 0, sizeof(_cqueue_t));
    c->esize = esize;
    c->size = size;
    c->mem = malloc(c->size * c->esize);
    if(c->mem == NULL){
        return BS_NOMEM;
    }
    memset(c->mem, 0, c->size*c->esize);

    c->islock = islock;
    if(islock){
        bs_lock_init(&c->lock);
    }

    return BS_SUCCESS;
}

void* bs_cqueue_push(_cqueue_t* c, void* e, uint32_t size){
    void*           ptr;

    if(c == NULL){
        return NULL;
    }

    if(c->islock){
        bs_wrlock(&c->lock);
    }

    if(cqueue_full(c)){
        c->head = cqueue_next(c, c->head);
    }

    ptr = cqueue_addr(c, c->rear);
    if(e != NULL){
        bs_memcpy(ptr, c->esize, e, size);
    }
    else{
        memset(ptr, 0, c->esize);
    }
    c->rear = cqueue_next(c, c->rear);

    if(c->islock){
        bs_wrlock_unlock(&c->lock);
    }

    return ptr;
}

state_t bs_cqueue_pop(_cqueue_t* c, void* e, uint32_t size){
    state_t         st = BS_SUCCESS;

    if(c==NULL || e==NULL){
        return BS_PARANULL;
    }

    if(c->islock){
        bs_wrlock(&c->lock);
    }

    if(cqueue_empty(c)){
        st = BS_EMPTY;
    }
    else{
        bs_memcpy(e, size, cqueue_addr(c, c->head), c->esize);
        c->head = cqueue_next(c, c->head);
    }

    if(c->islock){
        bs_wrlock_unlock(&c->lock);
    }

    return st;
}

void* bs_cqueue_get(_cqueue_t* c, uint32_t i){
    uint32_t        pos = cqueue_index(c, i);
    
    if (!cqueue_contain(c, pos)) {
        return NULL;
    }
    
    return cqueue_addr(c, pos);
}

void* bs_cqueue_search(_cqueue_t* c, void* e, compare_f cmp){
    uint32_t        i;

    for(i=c->head; i!=c->rear; i=(i+1)%c->size){
        if(cmp(e, cqueue_addr(c, i))==0){
            return cqueue_addr(c, i);
        }
    }

    return NULL;
}
