/**
 * file :	ThreadPool.h
 * author :	Rex
 * create :	2017-02-22 23:40
 * func :   线程池，用于异步
 * history:
 */

#ifndef	__THREADPOOL_H_
#define	__THREADPOOL_H_

#include <iostream>
#include <queue>
#include <vector>
#include "bs.h"
#include "Thread.h"

using namespace std;
typedef struct thread_task_t{
    void*   (*run)(void*);
    void*   arg;
}thread_task_t;

class ThreadWork:public LoopThread{
public:
    ThreadWork(pthread_mutex_t* lock=NULL, pthread_cond_t* ready=NULL, std::queue<thread_task_t>* queue=NULL, pthread_attr_t* attr = NULL);
    ~ThreadWork();
    virtual void loop();
    
    queue<thread_task_t>*       m_queue;
    pthread_mutex_t*            m_lock;
    pthread_cond_t*             m_ready;
    
    std::queue<thread_task_t>   m_queue_tasks;
    pthread_mutex_t             m_queue_lock;
    pthread_cond_t              m_queue_ready;
};

class ThreadPool{
public:
    static ThreadPool* shareInstance();
    static void destroyInstance();
    /**
     * @thread_num: 创建线程数
     * @policy: 调度方式
     */
    ThreadPool(int thread_num = 2, int policy=SCHED_OTHER);
    ~ThreadPool();
    
    void add(void*   (*run)(void*),void* arg);
    void destroy();
    inline pthread_t thread_id(int i){
        return m_threads[i]->m_thread;
    }
    inline int thread_num(){
        return m_threads.size();
    }
    
protected:
    static ThreadPool*          m_instance;
    
    std::vector<ThreadWork*>    m_threads;
    std::queue<thread_task_t>   m_queue;
    pthread_mutex_t             m_lock;
    pthread_cond_t              m_ready;
    pthread_attr_t              m_attr;
};

void async_run(void*   (*run)(void*), void* arg);
#endif
