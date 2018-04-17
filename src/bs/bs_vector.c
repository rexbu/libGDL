/**
 * bushaofeng. 2011.8.26
 * bs_vector.c
 * history:
 */

#include "bs_vector.h"

uint32_t     g_vector_realloc_size = VECTOR_REALLOC_SIZE;

void* _vector_init(void* p){
    _vector_t* v = (_vector_t*)p;
     
    v->len = 0;
    v->size = 0;
    v->mem = NULL;
    return v;
}

void _vector_destroy(void* p){
    _vector_t* v = (_vector_t*)p;
    if (v->mem != NULL)
    {
        free(v->mem);
    }
    v->len = 0;
    v->size = 0;
}

int _vector_add(_vector_t* v, void* elem, uint32_t esize){
    void*   pnew = NULL;
    
    if (vector_count(v) >= vector_size(v)) {
        pnew = realloc(v->mem, (vector_size(v)*esize + g_vector_realloc_size)*esize);
        if (pnew==NULL) {
            return BS_INVALID;
        }
        
        v->mem = pnew;
        v->size += g_vector_realloc_size;
    }
    memcpy((char*)v->mem+vector_count(v)*esize, elem, esize);
    v->len++;
    return v->len-1;
}

state_t _vector_find(_vector_t* v, void* elem, uint32_t esize){
    int i = 0;
    for (; i<vector_count(v); i++) {
        if(memcmp((char*)v->mem+i*esize, elem, esize)==0){
            return i;
        }
    }
    
    return BS_NOTFOUND;
}

//int bs_vector_lock_init(_vector_t* vector, uint32_t size, uint32_t esize, bool_t islock){
//    assert(vector!=NULL);
//    memset(vector, 0, sizeof(_vector_t));
//    vector->islock = islock;
//    if(islock){
//        if(pthread_rwlock_init(&vector->lock, NULL)!=0){
//            return BS_INVALID;
//        }
//    }
//    vector->esize = esize;
//    vector->mem = malloc(vector->esize * size);
//    if(vector->mem == NULL){
//        return BS_NOMEM;
//    }
//    vector->size = size;
//    vector->vlen = 0;
//
//    return BS_SUCCESS;
//}
//
//int bs_vector_lock_add(_vector_t* vector, void* elem){
//    void*       pnew;
//    char*       pcopy;
//    uint32_t    resize;
//    
//    assert(vector!=NULL && elem!=NULL);
//    
//    if(vector->islock){
//        pthread_rwlock_wrlock(&vector->lock);
//    }
//    if(vector->vlen >= vector->size){
//        resize = vector->size>1? vector->size/2:1;
//        pnew = realloc(vector->mem, (vector->vlen+resize)*vector->esize);
//        if(pnew == NULL){
//            return BS_NOMEM;
//        }
//
//        vector->mem= pnew;
//        vector->size += resize;
//    }
//
//    pcopy = (char*)vector->mem + vector->vlen * vector->esize;
//    memcpy(pcopy, elem, vector->esize);
//    vector->vlen++;
//    if(vector->islock){
//        pthread_rwlock_unlock(&vector->lock);
//    }
//
//    return BS_SUCCESS;
//}
//
//void* bs_vector_lock_getback(_vector_t* v){
//    void*       pnew;
//    char*       pcopy;
//    uint32_t    resize;
//    
//    assert(v!=NULL);
//    
//    if(v->islock){
//        pthread_rwlock_wrlock(&v->lock);
//    }
//    
//    if(v->vlen >= v->size){
//        resize = v->size>1 ? v->size/2:1;
//        pnew = realloc(v->mem, (v->vlen+resize) * v->esize);
//        if(pnew == NULL){
//            return NULL;
//        }
//
//        v->mem= pnew;
//        v->size += resize;
//    }
//
//    pcopy = (char*)v->mem + v->vlen * v->esize;
//    v->vlen++;
//
//    if(v->islock){
//        pthread_rwlock_unlock(&v->lock);
//    }
//
//    return pcopy;
//}
