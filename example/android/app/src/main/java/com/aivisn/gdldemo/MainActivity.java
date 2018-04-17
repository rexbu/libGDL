package com.aivisn.gdldemo;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.os.Bundle;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.TextView;

import com.aivisn.style.GDLStyle;
import com.rex.utils.DeviceUtil;
import com.rex.utils.FileUtil;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Timer;
import java.util.TimerTask;

public class MainActivity extends Activity implements SurfaceHolder.Callback {

    SurfaceView surface;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        surface = (SurfaceView)this.findViewById(R.id.surfaceView);
        SurfaceHolder holder = surface.getHolder();
        holder.addCallback(this);
        InputStream is = null;

        String[] res = new String[]{"chicago.jpg", "udnie.style", "style.model", "mnist.gdl", "conv1.val", "conv2.val", "pool1.val", "pool2.val", "fc1.val", "fc2.val"};
        try {
            for (int i=0; i<res.length; i++) {
                String name = res[i];
                is = this.getResources().getAssets().open(name);
                @SuppressLint("SdCardPath") OutputStream os = FileUtil.fileOutputStream("/data/data/" + DeviceUtil.getPackageName(this) + "/" + name);
                FileUtil.write(os, is);
                is.close();
                os.close();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        final Surface surface = holder.getSurface();
        final GDLStyle style = new GDLStyle();
        // style.processStyleImage("/data/data/com.aivisn.gdldemo/style.model", "/data/data/com.aivisn.gdldemo/chicago.jpg", "/data/data/com.aivisn.gdldemo/udnie.style", surface);

        Timer timer = new Timer();
        timer.schedule(new TimerTask() {

            @Override
            public void run() {
                style.processMnistImage("/data/data/com.aivisn.gdldemo/mnist.gdl", surface);
            }
        }, 0, 10);
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }
}
