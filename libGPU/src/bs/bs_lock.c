/**
 * file :	bs_lock.c
 * author :	bushaofeng
 * create :	2013-11-19 23:06
 * func : 
 */

#include "bs_lock.h"
#include "bs_timer.h"

// 如果申请不到锁的等待时间
static struct timeval g_wait_interval = {0, 5000};

void bs_lock_init(bs_lock_t* lock){
    assert(lock!=NULL);
    *lock = 0;
}

void bs_rdlock(bs_lock_t* lock){
    assert(lock!=NULL);
    while(*lock < 0){
        bs_timer_sleep(&g_wait_interval);
    }
    (*lock)++;
}

state_t bs_rdlock_try(bs_lock_t* lock){
    if (lock==NULL || *lock<0) {
        return BS_INVALID;
    }
    (*lock)++;
    return BS_SUCCESS;
}

state_t bs_rdlock_unlock(bs_lock_t* lock){
    assert(lock!=NULL);
    return --(*lock)>=0;
}

void bs_wrlock(bs_lock_t* lock){
    assert(lock!=NULL);

    while(*lock!=0){
        bs_timer_sleep(&g_wait_interval);
    }
    //fprintf(stdout, "lock: %s", s);
    *lock = -1;
}

state_t bs_wrlock_try(bs_lock_t* lock){
    if (lock==NULL || *lock!=0) {
        return BS_INVALID;
    }
    *lock = -1;
    return BS_SUCCESS;
}

void bs_wrlock_unlock(bs_lock_t* lock){
    assert(lock!=NULL);
    //fprintf(stdout, "unlock: %s", s);
    *lock = 0;
}
