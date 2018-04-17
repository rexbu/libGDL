#include "bs_pool.h"
#include "bs_mmap.h"

state_t bs_pool_init(_pool_t* pool, uint32_t size, uint32_t esize, bool_t islock){
    state_t             st;
    /*
    int                 i;
    _pool_head_t*       curr;
    _pool_head_t*       next;
    */

    assert(pool!=NULL && size>0 && esize>0);
    st = vector_init(&pool->buf);
    if(st!=BS_SUCCESS){
        return st;
    }

    pool->used_cnt = 0;
    pool->unused = NULL;

    return BS_SUCCESS;
}

void* bs_pool_malloc(_pool_t* pool){
    void*       ptr = NULL;

    assert(pool != NULL);
    if(pool->unused != NULL){
//        ptr = pool->unused+1;
//        memset(ptr, 0, vector_esize(&pool->buf)-sizeof(_pool_head_t));
//        pool->unused = pool->unused->next;
//        pool->used_cnt++;
//        pool->unused_cnt--;
    }
    else{
//        ptr = vector_getback(&pool->buf);
//        if(ptr!=NULL){
//            ptr = (char*)ptr + sizeof(_pool_head_t);
//            pool->used_cnt++;
//        }
    }

    return ptr;
}

void bs_pool_free(_pool_t* pool, void* mem){
    assert(pool != NULL && mem != NULL);
    _pool_head_t* head = (_pool_head_t*)mem - 1;
    head->next = pool->unused;
    pool->unused = head;
    pool->used_cnt--;
}

