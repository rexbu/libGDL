/**
 * file :	android_gpu.cpp
 * author :	Rex
 * create :	2017-07-21 14:45
 * func : 
 * history:
 */

#include <jni.h>
#include "GPUPipeline.h"

#ifdef __cplusplus
extern "C" {
#endif

jlong createGPUPipeline(JNIEnv* env, jobject jo, jobjectArray jnames);
void destroyGPUPipeline(JNIEnv* env, jobject jo);
jboolean loadImageBytes(JNIEnv* env, jobject jo, jbyteArray jbytes);
jboolean loadImagePath(JNIEnv* env, jobject jo, jstring jpath);
void processImage(JNIEnv* env, jobject jo);
void setMosaicBlockSize(JNIEnv* env, jobject jo, jfloat size);
void setMosaicCircle(JNIEnv* env, jobject jo, jfloat x, jfloat y, jfloat radius);
jbyteArray getBytes(JNIEnv * env, jobject jobj);

#ifdef __cplusplus
}
#endif
static GPUPipeline* getPGPUPipeline(JNIEnv * env, jobject jo);
static jclass 	g_gp_class = NULL;
static jfieldID g_gp_field = NULL;

jlong createGPUPipeline(JNIEnv* env, jobject jo, jobjectArray jnames){
	jsize len = env->GetArrayLength(jnames);
	char** names = (char**)malloc(sizeof(char*)*len);
	if (names == NULL)
	{
		return 0;
	}

	for (int i = 0; i < len; ++i)
	{
		jstring jstr = (jstring)env->GetObjectArrayElement(jnames, i);
		const char* name = env->GetStringUTFChars(jstr, NULL);
		names[i] = (char*)malloc(strlen(name)+1);
		strcpy(names[i], name);
		env->ReleaseStringUTFChars(jstr, name);
	}
	
	GPUPipeline* p = new GPUPipeline((const char**)names, (int)len);
	
	for (int i = 0; i < len; ++i)
	{
		free(names[i]);
	}
	free(names);

	return (jlong)p;
}

void destroyGPUPipeline(JNIEnv* env, jobject jo){
	GPUPipeline* p = getPGPUPipeline(env, jo);
	delete p;
}

jboolean loadImageBytes(JNIEnv* env, jobject jo, jbyteArray jbytes){
	GPUPipeline* p = getPGPUPipeline(env, jo);
	//jbyte* bytes = (jbyte*)env->GetPrimitiveArrayCritical(jbytes, NULL);
	jbyte* bytes = env->GetByteArrayElements(jbytes, NULL);
	p->loadImage((uint8_t*)bytes, (uint32_t)env->GetArrayLength(jbytes));
	//env->ReleasePrimitiveArrayCritical(jbytes, bytes, 0);
	env->ReleaseByteArrayElements(jbytes, bytes, 0);
}

jboolean loadImagePath(JNIEnv* env, jobject jo, jstring jpath){
	GPUPipeline* p = getPGPUPipeline(env, jo);
	const char* path = env->GetStringUTFChars(jpath, NULL);
	p->loadImage(path);
	env->ReleaseStringUTFChars(jpath, path);
}
void processImage(JNIEnv* env, jobject jo){
	GPUPipeline* p = getPGPUPipeline(env, jo);
	p->processImage();
}
void setMosaicBlockSize(JNIEnv* env, jobject jo, jfloat size){
	GPUPipeline* p = getPGPUPipeline(env, jo);
	p->setMosaicBlockSize(size);
}

void setMosaicCircle(JNIEnv* env, jobject jo, jfloat x, jfloat y, jfloat radius){
	GPUPipeline* p = getPGPUPipeline(env, jo);
	gpu_point_t c = {x, y};
	p->setMosaicCircle(c, radius);
}

jbyteArray getBytes(JNIEnv * env, jobject jo){
	GPUPipeline* p = getPGPUPipeline(env, jo);

	uint32_t size = p->rawOutSize();
	jbyteArray jbytes =env->NewByteArray(size);
	env->SetByteArrayRegion(jbytes, 0, size, (const jbyte*)p->getBytes());
    return jbytes;
}

GPUPipeline* getPGPUPipeline(JNIEnv * env, jobject jo){
	if (g_gp_class==NULL)
	{
		g_gp_class = env->FindClass("com/rex/libgpu/GPUPipeline");
	}
	if (g_gp_field==NULL)
	{
		g_gp_field = env->GetFieldID(g_gp_class, "mPtr", "J");
	}

	return (GPUPipeline*)env->GetLongField(jo, g_gp_field);
}