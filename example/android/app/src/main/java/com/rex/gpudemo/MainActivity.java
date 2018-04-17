package com.rex.gpudemo;

import android.app.Activity;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.util.Log;
import android.widget.ImageView;

import com.rex.libgpu.GPUPipeline;

import java.io.IOException;
import java.io.InputStream;

public class MainActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        ImageView view = (ImageView)findViewById(R.id.imageview);

        AssetManager am = getResources().getAssets();

        try {
            InputStream is = am.open("demo.jpg");
            byte[] bytes = new byte[is.available()];
            is.read(bytes);

            Bitmap bitmap = BitmapFactory.decodeByteArray(bytes, 0, bytes.length);
            int width = bitmap.getWidth();
            int height = bitmap.getHeight();

            GPUPipeline pipeline = new GPUPipeline(new String[]{"GPUPicture", "GPUMosaicFilter", "GPURawOutput"});
            pipeline.loadImageBytes(bytes);
            pipeline.processImage();

            byte[] out = pipeline.getBytes();
            view.setImageBitmap(rgbaToBitmap(out, width, height));
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    Bitmap rgbaToBitmap(byte[] rgba, int width, int height){
        int[] bitout = new int[rgba.length/4];
        for (int i=0; i<bitout.length; i++){
            int r = rgba[i*4];
            int g = rgba[i*4+1];
            int b = rgba[i*4+2];
            bitout[i] = 0xff000000 + (r << 16) + (g << 8) + b;
        }
        Bitmap bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
        bitmap.setPixels(bitout, 0, width, 0, 0, width, height);
        return bitmap;
    }
}
