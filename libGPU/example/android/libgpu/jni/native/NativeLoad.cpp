/**
 * file :	NativeLoad.cpp
 * author :	bushaofeng
 * create :	2016-08-12 13:25
 * func : 
 * history:
 */
#include <iostream>
#include <string>
#include <map>

#include <jni.h>
#include <android/log.h>
#include <dlfcn.h>  
#include <fcntl.h>

using namespace std;

JavaVM*	g_jvm = NULL;
map<string, void *> g_so_map;
char g_package_name[256] = {0};

jlong loadSo(JNIEnv *env, jclass jc, jstring so_name);
jint registJNIMethod(JNIEnv *env, jclass jc, jlong so, jstring jclassname, jstring jfunc, jstring jsignature);
static int registerNativeMethods(JNIEnv *env, const char *className, JNINativeMethod *gMethods, int numMethods);

jint JNI_OnLoad(JavaVM *vm, void *reserved) {  
    void *env = NULL;  
    //LOGI("JNI_OnLoad");  
    if (vm->GetEnv(&env, JNI_VERSION_1_6) != JNI_OK) {  
        __android_log_print(ANDROID_LOG_ERROR, "NativeLoad", "ERROR: GetEnv failed");
        return -1;
    }

    FILE* fp = fopen("/proc/self/cmdline", "r");
    fread(g_package_name, sizeof(g_package_name), 1, fp);
    /*
    if (strcmp(g_package_name, "com.visionin.demo")!=0)
    {
        return -1;
    }
    */
    fclose(fp);

    g_jvm = vm;
    __android_log_print(ANDROID_LOG_ERROR, "NativeLoad", "g_jvm: %llu", (unsigned long long)g_jvm);

    JNINativeMethod method[2] = {
    	{"loadSo", "(Ljava/lang/String;)J", (void*)loadSo},
    	{"registJNIMethod", "(JLjava/lang/String;Ljava/lang/String;Ljava/lang/String;)I", (void*)registJNIMethod}
    	};
    registerNativeMethods((JNIEnv *)env, "com/rex/load/NativeLoad", method, 2);

    return JNI_VERSION_1_6;
}

jlong loadSo(JNIEnv *env, jclass jc, jstring so_name){
	void *filehandle = NULL;  
    
    const char* so = env->GetStringUTFChars(so_name, NULL);
    if(so == NULL) {  
        __android_log_print(ANDROID_LOG_ERROR, "NativeLoad", "Error: so name is null!");
       return 0;
    }

    map<string, void*>::iterator iter = g_so_map.find(string(so));
    if (iter == g_so_map.end())
    {
        filehandle = dlopen(so, RTLD_LAZY);
        if (filehandle==NULL)
        {
            char libpath[1024];
            snprintf(libpath, sizeof(libpath), "/data/data/%s/lib/%s", g_package_name, so);
            __android_log_print(ANDROID_LOG_ERROR, "NativeLoad", "%s load error[%s]. change to load path: %s", so, dlerror(), libpath);
            filehandle = dlopen(libpath, RTLD_LAZY);
            if (filehandle==NULL)
            {
                __android_log_print(ANDROID_LOG_ERROR, "NativeLoad", "Libary %s Load Error[%s]!", so, dlerror());
                return 0;
            }
        }

        g_so_map[string(so)] = filehandle;
    }
    else{
        filehandle = iter->second;
    }

    env->ReleaseStringUTFChars(so_name, so);
    return (jlong)filehandle;
}

jint registJNIMethod(JNIEnv *env, jclass jc, jlong so, jstring jclassname, jstring jfunc, jstring jsignature){
	const char* classname = env->GetStringUTFChars(jclassname, NULL);
	const char* java_func = env->GetStringUTFChars(jfunc, NULL);
	const char* signature = env->GetStringUTFChars(jsignature, NULL);
	//const char* native_func = env->GetStringUTFChars(jnative_func, NULL);
    if(classname==NULL || java_func == NULL || signature==NULL) {  
       return 0;
    }

    void* func = dlsym((void*)so, java_func);
    if (func==NULL)
    {
    	__android_log_print(ANDROID_LOG_ERROR, "NativeLoad", "Libary[%lld] Load Method [%s] Error!", (long long)so, java_func);
    	return 0;
    }

    JNINativeMethod method = {java_func, signature, func};
    int st = registerNativeMethods(env, classname, &method, 1);

    env->ReleaseStringUTFChars(jclassname, classname);
    env->ReleaseStringUTFChars(jfunc, java_func);
    env->ReleaseStringUTFChars(jsignature, signature);
    //env->ReleaseStringUTFChars(jnative_func, native_func);

    return st;
}

static int registerNativeMethods(JNIEnv *env, const char *className, JNINativeMethod *methods, int numMethods){  
    jclass clazz;  
    clazz = env->FindClass(className);  
    if (clazz == NULL) {  
        __android_log_print(ANDROID_LOG_ERROR, "NativeLoad", "Native registration unable to find class '%s'", className);  
        return JNI_FALSE;  
    }  
    if (env->RegisterNatives(clazz, methods, numMethods) < 0) {  
        __android_log_print(ANDROID_LOG_ERROR, "NativeLoad", "RegisterNatives failed for '%s'", className);  
        return JNI_FALSE;  
    }  
    return JNI_TRUE;  
}  
