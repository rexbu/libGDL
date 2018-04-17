package com.rex.utils;

import android.hardware.Camera;
import android.util.Log;

/**
 * @date 16/11/26 2:12 PM
 * @author VisionIn
 */
public class CameraUtil {
    public static Camera mCamera = null;

    public static Camera.Size openCamera(boolean isFront) {
        if (mCamera != null) {
            throw new RuntimeException("camera already initialized");
        }
        int mPosition;
        if(isFront){
            mPosition= Camera.CameraInfo.CAMERA_FACING_FRONT;
        }else {
            mPosition=Camera.CameraInfo.CAMERA_FACING_BACK;
        }
        mCamera=Camera.open(mPosition);

        if (mCamera == null) {
            Log.e("Visionin", "No front-facing camera found; opening default");
            mCamera = Camera.open();    // opens first back-facing camera
        }

        Camera.Parameters params = mCamera.getParameters();
        return params.getPreviewSize();
    }

    public static Camera.Size openCamera(int desiredWidth, int desiredHeight, boolean isFront) {
        if (mCamera != null) {
            throw new RuntimeException("camera already initialized");
        }
        int mPosition;
        if(isFront){
            mPosition= Camera.CameraInfo.CAMERA_FACING_FRONT;
            mCamera = Camera.open(mPosition);
        }else {
            mPosition=Camera.CameraInfo.CAMERA_FACING_BACK;
            mCamera=Camera.open(mPosition);
        }

        if (mCamera == null) {
            Log.e("Visionin", "No front-facing camera found; opening default");
            mCamera = Camera.open();    // opens first back-facing camera
        }

        Camera.Parameters params = mCamera.getParameters();
        Camera.Size ppsfv = params.getPreferredPreviewSizeForVideo();
        if (ppsfv != null) {
            Log.e("Visionin", "Camera preferred preview size for video is " +
                    ppsfv.width + "x" + ppsfv.height);
            params.setPreviewSize(ppsfv.width, ppsfv.height);
        }

        for (Camera.Size size : params.getSupportedPreviewSizes()) {
            if (size.width == desiredWidth && size.height == desiredHeight) {
                params.setPreviewSize(desiredWidth, desiredHeight);
                break;
            }
        }

        params.setRecordingHint(true);
        mCamera.setParameters(params);

        int[] fpsRange = new int[2];
        params.getPreviewFpsRange(fpsRange);
        return params.getPreviewSize();
    }

    public static void releaseCamera() {
        if (mCamera != null) {
            mCamera.stopPreview();
            mCamera.release();
            mCamera = null;
            Log.d("Visionin", "releaseCamera -- done");
        }
    }
}
