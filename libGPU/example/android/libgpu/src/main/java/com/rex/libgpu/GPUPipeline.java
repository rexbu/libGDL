package com.rex.libgpu;

import android.util.Log;

import com.rex.load.NativeLoad;

/**
 * Created by Rex on 2017/9/26.
 */

public class GPUPipeline {
    static {
        long so = NativeLoad.loadSo("libGPU.so");
        NativeLoad.registJNIMethod(so, "com/rex/libgpu/GPUPipeline", "createGPUPipeline", "([Ljava/lang/String;)J");
        NativeLoad.registJNIMethod(so, "com/rex/libgpu/GPUPipeline", "destroyGPUPipeline", "()V");
        NativeLoad.registJNIMethod(so, "com/rex/libgpu/GPUPipeline", "loadImageBytes", "([B)Z");
        NativeLoad.registJNIMethod(so, "com/rex/libgpu/GPUPipeline", "loadImagePath", "(Ljava/lang/String;)Z");
        NativeLoad.registJNIMethod(so, "com/rex/libgpu/GPUPipeline", "processImage", "()V");
        NativeLoad.registJNIMethod(so, "com/rex/libgpu/GPUPipeline", "getBytes", "()[B");
        NativeLoad.registJNIMethod(so, "com/rex/libgpu/GPUPipeline", "setMosaicBlockSize", "(F)V");
        NativeLoad.registJNIMethod(so, "com/rex/libgpu/GPUPipeline", "setMosaicCircle", "(FFF)V");
    }

    protected long  mPtr;   // jni层指针
    public GPUPipeline(String[] names){
        mPtr = createGPUPipeline(names);
        if (mPtr==0){
            Log.e("GPU", "create pipeline error!");
        }
    }

    @Override
    protected void finalize(){
        destroyGPUPipeline();
    }

    public native long createGPUPipeline(String[] name);
    public native void destroyGPUPipeline();

    public native boolean loadImageBytes(byte[] bytes);
    public native boolean loadImagePath(String path);
    public native void processImage();
    public native byte[] getBytes();

    public native void setMosaicBlockSize(float size);
    public native void setMosaicCircle(float x, float y, float radius);
}
