#include "Thread.h"
#ifdef __ANDROID__
#include <jni.h>
#endif

void* thread_func(void* para);

pthread_t Thread::start(){
    pthread_create(&m_thread, m_attr, thread_func, this);
    return m_thread;
}

void* thread_func(void* para){
    Thread*     thread = (Thread*)para;

#ifdef __ANDROID__
    extern JavaVM*  g_jvm;
    JNIEnv*     	env;

    g_jvm->AttachCurrentThread(&env, NULL);
#endif
    
    thread->run();
    
#ifdef __ANDROID__
    g_jvm->DetachCurrentThread();
#endif
    return NULL;
}
