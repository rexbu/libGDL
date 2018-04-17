/**
 * file :	ThreadPool.cpp
 * author :	Rex
 * create :	2017-02-22 23:40
 * func : 
 * history:
 */

#include <sched.h>
#include "ThreadPool.h"

#pragma --mark "工作者线程"
ThreadWork::ThreadWork(pthread_mutex_t* lock, pthread_cond_t* ready, std::queue<thread_task_t>* queue, pthread_attr_t* attr):
LoopThread(attr){
    m_lock = lock;
    m_ready = ready;
    m_queue = queue;
    
    pthread_mutex_init(&m_queue_lock, NULL);
    pthread_cond_init(&m_queue_ready, NULL);
    if (m_lock == NULL) {
        m_lock = &m_queue_lock;
    }
    if (m_ready==NULL) {
        m_ready = &m_queue_ready;
    }
    if (m_queue==NULL) {
        m_queue = &m_queue_tasks;
    }
}

ThreadWork::~ThreadWork(){
    pthread_mutex_destroy(&m_queue_lock);
    pthread_cond_destroy(&m_queue_ready);
}

void ThreadWork::loop(){
    /* 如果线程池没有被销毁且没有任务要执行，则等待 */
    pthread_mutex_lock(m_lock);
    while(m_queue->size()==0 && m_running) {
        pthread_cond_wait(m_ready, m_lock);
    }
    
    // 如果停止，则退出线程
    if (!m_running) {
        pthread_mutex_unlock(m_lock);
        pthread_exit(NULL);
    }
    
    // 队头取出任务执行
    thread_task_t task = m_queue->front();
    m_queue->pop();
    pthread_mutex_unlock(m_lock);
    task.run(task.arg);
}

#pragma --mark "线程池"
ThreadPool* ThreadPool::m_instance = NULL;

ThreadPool* ThreadPool::shareInstance(){
    if (m_instance==NULL) {
        m_instance = new ThreadPool();
    }
    
    return m_instance;
}

void ThreadPool::destroyInstance(){
    if (m_instance!=NULL) {
        delete m_instance;
        m_instance = NULL;
    }
}

ThreadPool::ThreadPool(int thread_num, int policy){
    pthread_attr_init(&m_attr);
    pthread_attr_setschedpolicy(&m_attr, policy);
    int priority = 99;
    pthread_attr_setschedparam(&m_attr, (const struct sched_param *)&priority);
    
    pthread_mutex_init(&m_lock, NULL);
    pthread_cond_init(&m_ready, NULL);
    for (int i=0; i<thread_num; i++) {
        ThreadWork* work = new ThreadWork(&m_lock, &m_ready, &m_queue, &m_attr);
        work->start();
        m_threads.push_back(work);
    }
}

ThreadPool::~ThreadPool(){
    destroy();
}

void ThreadPool::add(void*   (*run)(void*),void* arg){
    pthread_mutex_lock(&m_lock);
    thread_task_t task = {run , arg};
    m_queue.push(task);
    pthread_cond_signal(&m_ready);
    pthread_mutex_unlock(&m_lock);
}

void ThreadPool::destroy(){
    if (m_threads.size()==0) {
        return;
    }
    
    // 停止线程
    for (int i=0; i<m_threads.size(); i++) {
        m_threads[i]->stop();
    }
    
    // 广播
    pthread_mutex_lock(&m_lock);
    pthread_cond_broadcast(&m_ready);
    pthread_mutex_unlock(&m_lock);
    // 等待所有线程结束后释放线程空间
    for (int i=0; i<m_threads.size(); i++) {
        pthread_join(m_threads[i]->m_thread, NULL);
    }
    for (int i=0; i<m_threads.size(); i++) {
        delete m_threads[i];
    }
    m_threads.clear();
    
    pthread_mutex_destroy(&m_lock);
    pthread_cond_destroy(&m_ready);
}

void async_run(void*   (*run)(void*), void* arg){
    ThreadPool::shareInstance()->add(run, arg);
}
