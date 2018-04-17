/**
 * file :	style_android.cpp
 * author :	Rex
 * create :	2018-02-11 17:22
 * func : 
 * history:
 */

#include <jni.h>
#include <android/native_window_jni.h>
#include "GPU.h"
#include "../GDLStyle.h"

#ifdef __cplusplus
extern "C" {
#endif

void processStyleImage(JNIEnv *env, jobject thiz, jstring jmodel_path, jstring jpic_path, jstring jstyle_path, jobject jsurface);
void processMnistImage(JNIEnv *env, jobject thiz, jstring jmodel_path, jobject jsurface);
#ifdef __cplusplus
}
#endif

static GPUView* g_view = NULL;

void processMnistImage(JNIEnv *env, jobject thiz, jstring jmodel_path, jobject jsurface){
    const char* model_path = env->GetStringUTFChars(jmodel_path, NULL);
    if (g_view==NULL){
        ANativeWindow* window = ANativeWindow_fromSurface(env, jsurface);
        g_view = new GPUView(window);
    }

    static GPUPicture* pic = NULL;
    if (pic==NULL){
        GDLSession* session = new GDLSession(model_path);
        uint8_t number[] = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                            255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                            255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                            255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                            255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 244, 240, 255, 250, 222, 217, 231, 253, 255, 255, 255,
                            255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 252, 245, 220, 216, 215, 141, 95, 84, 185, 113, 0, 0, 111, 248, 255, 255, 255,
                            255, 255, 255, 255, 255, 255, 255, 247, 210, 191, 158, 112, 91, 38, 16, 19, 8, 0, 32, 94, 39, 1, 42, 163, 254, 255, 255, 255,
                            255, 255, 255, 255, 255, 255, 255, 217, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 131, 180, 164, 187, 215, 250, 255, 255, 255, 255,
                            255, 255, 255, 255, 255, 255, 255, 238, 66, 0, 0, 0, 0, 0, 62, 100, 23, 16, 213, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                            255, 255, 255, 255, 255, 255, 255, 255, 186, 111, 127, 42, 0, 50, 208, 255, 196, 128, 225, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                            255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 116, 0, 116, 255, 255, 255, 255, 252, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                            255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 157, 0, 48, 232, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                            255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 243, 84, 0, 133, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                            255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 230, 47, 0, 83, 146, 239, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                            255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 187, 29, 0, 0, 115, 235, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                            255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 205, 74, 0, 0, 89, 221, 255, 255, 255, 255, 255, 255, 255, 255,
                            255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 247, 167, 17, 0, 66, 218, 255, 255, 255, 255, 255, 255, 255,
                            255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 58, 0, 0, 157, 255, 255, 255, 255, 255, 255, 255,
                            255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 217, 150, 101, 15, 0, 15, 197, 255, 255, 255, 255, 255, 255, 255,
                            255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 232, 147, 57, 7, 0, 0, 0, 68, 230, 255, 255, 255, 255, 255, 255, 255,
                            255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 245, 168, 65, 0, 0, 0, 0, 40, 143, 231, 255, 255, 255, 255, 255, 255, 255, 255,
                            255, 255, 255, 255, 255, 255, 255, 255, 236, 200, 91, 14, 0, 0, 0, 47, 139, 221, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                            255, 255, 255, 255, 255, 255, 250, 137, 55, 18, 0, 0, 0, 35, 140, 223, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                            255, 255, 255, 255, 211, 108, 40, 0, 0, 0, 0, 4, 86, 203, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                            255, 255, 255, 255, 160, 24, 0, 1, 38, 93, 112, 177, 240, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                            255, 255, 255, 255, 232, 211, 214, 219, 229, 245, 253, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                            255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                            255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};
        //pic = new GPUPicture([[NSBundle mainBundle] pathForResource:@"mnist" ofType:@"png"].UTF8String);
        gpu_frame_option_t option = GPUFrameBuffer::defaultFrameOption();
        option.color_format = GL_LUMINANCE;
        option.format = GL_LUMINANCE;
        pic = new GPUPicture((uint8_t*)number, 28, 28, &option);

        pic->addTarget(session);
        GPUConvShape* shape = new GPUConvShape(28, 1, 2);
        session->addLayerBase(shape);
        GPUConvLayer* conv = new GPUConvLayer(5, 32);
        session->addConv(conv, "conv1");
        GPUPoolLayer* pool = new GPUPoolLayer(32, 2, CONV_POOL_MAX, 2);
        session->addLayerBase(pool);
        conv = new GPUConvLayer(5, 64);
        session->addConv(conv, "conv2");
        pool = new GPUPoolLayer(64);
        session->addLayerBase(pool);

        CPUFullConnLayer* full = new CPUFullConnLayer();
        session->addFullConv(full, "fc1");
        full = new CPUFullConnLayer(CONV_ACTIVE_NULL);
        session->addFullConv(full, "fc2");

        session->addTarget(g_view);

        session->layer(1)->setTensor("/data/data/com.aivisn.gdldemo/conv1.val");
        session->layer(2)->setTensor("/data/data/com.aivisn.gdldemo/pool1.val");
        session->layer(3)->setTensor("/data/data/com.aivisn.gdldemo/conv2.val");
        session->layer(4)->setTensor("/data/data/com.aivisn.gdldemo/pool2.val");
        session->layer(5)->setTensor("/data/data/com.aivisn.gdldemo/fc1.val");
        session->layer(6)->setTensor("/data/data/com.aivisn.gdldemo/fc2.val");
    }

    struct timeval t0,t1;
    gettimeofday(&t0, NULL);
    pic->processImage();
    gettimeofday(&t1, NULL);
    err_log("frame time:%lu ms\n", (t1.tv_sec-t0.tv_sec)*1000+(t1.tv_usec-t0.tv_usec)/1000);

    env->ReleaseStringUTFChars(jmodel_path, model_path);
}

static GDLStyle* g_style = NULL;
void processStyleImage(JNIEnv *env, jobject thiz, jstring jmodel_path, jstring jpic_path, jstring jstyle_path, jobject jsurface){
    const char* model_path = env->GetStringUTFChars(jmodel_path, NULL);
    const char* pic_path = env->GetStringUTFChars(jpic_path, NULL);
    const char* style_path = env->GetStringUTFChars(jstyle_path, NULL);
    ANativeWindow* window = ANativeWindow_fromSurface(env, jsurface);

    GPUPicture pic(pic_path);
    if (g_style==NULL){
        g_style = new GDLStyle(model_path);
        g_style->style(style_path);
    }
    if (g_view==NULL){
        g_view = new GPUView(window);
    }
    pic.addTarget(g_style);
    g_style->addTarget(g_view);

    pic.processImage();
    env->ReleaseStringUTFChars(jmodel_path, model_path);
    env->ReleaseStringUTFChars(jpic_path, pic_path);
    env->ReleaseStringUTFChars(jstyle_path, style_path);
}
