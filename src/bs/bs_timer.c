/**
 * file :	bs_timer.c
 * author :	bushaofeng
 * create :	2013-11-29 22:42
 * func : 
 * history:
 */
#include "bs_common.h"
#include "bs_timer.h"
#include "bs_list.h"
#include "bs_log.h"

static list_t(bs_timer_t)       timer_list;
//static struct itimerval         interval;
static uint32_t                 interval;
static pthread_rwlock_t         lock;
bool_t                          is_init = BS_FALSE;

state_t bs_timer_init(uint32_t inter){
    int                     st;
    struct timeval          tm;
    struct itimerval        itimer;
    pthread_rwlockattr_t    attr;

    if(is_init){
        return BS_SUCCESS;
    }
    
    list_init_size(&timer_list, TIMER_DEF_SIZE, BS_TRUE);
    pthread_rwlockattr_init(&attr);
    st = pthread_rwlock_init(&lock, &attr);
    if(st!=0){
        return BS_INVALID;
    }
    interval = inter;
    BS_SET_TIMEVAL(&tm, inter);
    itimer.it_value = tm;
    itimer.it_interval = tm;
    signal(SIGALRM, bs_timer_proc);
    st = setitimer(ITIMER_REAL, &itimer, NULL);
    if(st!=0){
        list_destroy(&timer_list);
        return BS_INVALID;
    }
    
    is_init = BS_TRUE;
    return BS_SUCCESS;
}

bs_timer_t* bs_timer_set(_timer_f func, void* para, struct timeval tm){
    bs_timer_t*             node;

    node = (bs_timer_t*)list_insert(&timer_list, NULL);
    if(node == NULL){
        pthread_rwlock_unlock(&lock);
        return NULL;
    }

    node->func = func;
    node->para = para;
    node->run_cnt = 0;
    node->time_cnt = 0;
    node->time_size = (tm.tv_sec*1000000+tm.tv_usec)/interval;
    if((tm.tv_sec*1000000+tm.tv_usec)%interval>0){
        node->time_size++;
    }
    node->stop_cnt = 0;
    ////TODO: 依赖vector
    // node->id = pool_position(&timer_list.pool, timer_list.rear.prev);
    return node;
}

bs_timer_t* bs_timer_set_now(_timer_f func, void* para, struct timeval tm){
    bs_timer_t*             node = bs_timer_set(func, para, tm);

    node->run_cnt++;
    node->func(node);
    
    return node;
}

bs_timer_t* bs_timer_set_num(_timer_f func, void* para, struct timeval tm, uint32_t num){
    bs_timer_t*             node = bs_timer_set(func, para, tm);
    
    node->stop_cnt = num;
    return node;
}

state_t bs_timer_del(int id){
    state_t                 st;
    void*                   node;

    node = list_position(&timer_list, id);
    if(node==NULL){
        return BS_NOTFOUND;
    }
    st = list_remove(&timer_list, node);
    
    return st;
}

void bs_timer_proc(){
    void*                   node;
    bs_timer_t*             tn;

    // 时间间隔太短时候容易一起申请锁
    if(pthread_rwlock_trywrlock(&lock) != 0){
        return;
    }
    for(node=list_head(&timer_list); list_forward(node); node=list_next(node)){
        tn = (bs_timer_t*)(node);
        if(++tn->time_cnt==tn->time_size){
            tn->time_cnt = 0;
            tn->run_cnt++;
            pthread_rwlock_unlock(&lock);
            tn->func(tn);
            if(tn->run_cnt == tn->stop_cnt){
                list_remove(&timer_list, tn);
            }
            pthread_rwlock_wrlock(&lock);
        }
    }
    pthread_rwlock_unlock(&lock);
}

void bs_timer_signal(int sig){
    bs_timer_proc();
}

void bs_timer_procnull(){
    return;
}

void bs_timer_select(struct timeval* tm, _timers_f func, void* para){
	while(BS_TRUE){
		struct timeval start;
		struct timeval end;
		struct timeval sleep;

		gettimeofday(&start, NULL);
		func(para);
		gettimeofday(&end, NULL);

		long run_time = BS_USEC_INTERVAL(&end, &start);
		long sleep_time = (1000000*tm->tv_sec+tm->tv_usec) - run_time;
		BS_SET_TIMEVAL(&sleep, sleep_time);

		select(1, NULL, NULL, NULL, &sleep);
	}
}
