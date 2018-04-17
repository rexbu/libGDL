package com.rex.libgpu;

import android.graphics.Bitmap;
import android.util.Log;

import com.rex.load.NativeLoad;

/**
 * Created by Rex on 2017/9/26.
 */

public class GPUPipeline {
  static {
    long so = NativeLoad.loadSo("libGPU.so");
    NativeLoad.registerJNIMethod(so, "com/rex/libgpu/GPUPipeline", "createGPUPipeline",
        "([Ljava/lang/String;)J");
    NativeLoad.registerJNIMethod(so, "com/rex/libgpu/GPUPipeline", "destroyGPUPipeline", "()V");
    NativeLoad.registerJNIMethod(so, "com/rex/libgpu/GPUPipeline", "loadImageBytes", "([B)Z");
    NativeLoad.registerJNIMethod(so, "com/rex/libgpu/GPUPipeline", "loadImagePath",
        "(Ljava/lang/String;)Z");
    NativeLoad.registerJNIMethod(so, "com/rex/libgpu/GPUPipeline", "processImage", "()V");
    NativeLoad.registerJNIMethod(so, "com/rex/libgpu/GPUPipeline", "getBytes", "()[B");
    NativeLoad.registerJNIMethod(so, "com/rex/libgpu/GPUPipeline", "setMosaicBlockSize", "(F)V");
    NativeLoad.registerJNIMethod(so, "com/rex/libgpu/GPUPipeline", "setMosaicCircle", "(FFF)V");
    NativeLoad.registerJNIMethod(so, "com/rex/libgpu/GPUPipeline", "setPictureSize", "(FF)V");
    NativeLoad.registerJNIMethod(so, "com/rex/libgpu/GPUPipeline", "loadBitmapImage", "(Landroid/graphics/Bitmap;)Z");
  }
  // jni层指针
  protected long mPtr;

  public GPUPipeline(String[] names) {
    mPtr = createGPUPipeline(names);
    if (mPtr == 0) {
      Log.e("GPU", "create pipeline error!");
    }
  }

  @Override
  protected void finalize() {
    destroyGPUPipeline();
  }

  public native long createGPUPipeline(String[] name);
  public native void destroyGPUPipeline();

  public native boolean loadImageBytes(byte[] bytes);
  public native boolean loadImagePath(String path);
  public native void processImage();
  public native byte[] getBytes();

  public native void setPictureSize(float w, float h);
  public native void setMosaicBlockSize(float size);
  public native void setMosaicCircle(float x, float y, float radius);
  public native boolean loadBitmapImage(Bitmap bitmap);
}
