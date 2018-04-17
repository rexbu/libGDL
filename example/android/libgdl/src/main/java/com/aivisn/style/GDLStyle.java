package com.aivisn.style;

import android.view.Surface;

import com.rex.load.NativeLoad;

/**
 * Created by Rex on 2018/2/11.
 */

public class GDLStyle {
    static{
        long so = NativeLoad.loadSo("libstyle.so");

        NativeLoad.registerJNIMethod(so, "com/aivisn/style/GDLStyle", "processStyleImage", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Landroid/view/Surface;)V");
        NativeLoad.registerJNIMethod(so, "com/aivisn/style/GDLStyle", "processMnistImage", "(Ljava/lang/String;Landroid/view/Surface;)V");
    }

    public native void processStyleImage(String model, String pic, String style, Surface surface);
    public native void processMnistImage(String model, Surface surface);
}
