package com.ncnn.demo;
import android.content.res.AssetManager;
import android.graphics.Bitmap;

import java.util.ArrayList;
import java.util.List;

public class textocr {
    static {
        System.loadLibrary("mytext");

    }

    public native boolean Init(AssetManager am, byte[] pse_param, byte[] pse_bin, byte[] crnn_param, byte[] crnn_bin, List<String> resultLabel);
    //public native float[] Detect(Bitmap bitmap);
    public native int[] Detect(Bitmap bitmap,int lengthsize);

}
