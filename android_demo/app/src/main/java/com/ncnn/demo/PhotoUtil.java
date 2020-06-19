package com.ncnn.demo;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.provider.MediaStore;

import java.nio.FloatBuffer;


public class PhotoUtil {

    // start camera
//    public static String start_camera(Activity activity, int requestCode) {
//        Uri imageUri;
//        // save image in cache path
//        File outputImage = new File(Environment.getExternalStorageDirectory().getAbsolutePath()
//                + "/ncnn_mobile/", System.currentTimeMillis() + ".jpg");
//        Log.d("outputImage", outputImage.getAbsolutePath());
//        try {
//            if (outputImage.exists()) {
//                outputImage.delete();
//            }
//            File out_path = new File(Environment.getExternalStorageDirectory().getAbsolutePath()
//                    + "/ncnn_mobile/");
//            if (!out_path.exists()) {
//                out_path.mkdirs();
//            }
//            outputImage.createNewFile();
//        } catch (IOException e) {
//            e.printStackTrace();
//        }
//        if (Build.VERSION.SDK_INT >= 24) {
//            // compatible with Android 7.0 or over
//            imageUri = FileProvider.getUriForFile(activity,
//                    "com.example.ncnn1", outputImage);
//        } else {
//            imageUri = Uri.fromFile(outputImage);
//        }
//        // set system camera Action
//        Intent intent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
//        intent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
//        // set save photo path
//        intent.putExtra(MediaStore.EXTRA_OUTPUT, imageUri);
//        // set photo quality, min is 0, max is 1
//        intent.putExtra(MediaStore.EXTRA_VIDEO_QUALITY, 0);
//        activity.startActivityForResult(intent, requestCode);
//        // return image absolute path
//        return outputImage.getAbsolutePath();
//    }

    // get picture in photo
    public static void use_photo(Activity activity, int requestCode) {
        Intent intent = new Intent(Intent.ACTION_PICK);
        intent.setType("image/*");
        activity.startActivityForResult(intent, requestCode);
    }

    // get photo from Uri
    public static String get_path_from_URI(Context context, Uri uri) {
        String result;
        Cursor cursor = context.getContentResolver().query(uri, null, null, null, null);
        if (cursor == null) {
            result = uri.getPath();
        } else {
            cursor.moveToFirst();
            int idx = cursor.getColumnIndex(MediaStore.Images.ImageColumns.DATA);
            result = cursor.getString(idx);
            cursor.close();
        }
        return result;
    }

    // Caffe model，get predict data
    public static float[] getScaledMatrix(Bitmap bitmap, int desWidth,
                                          int desHeight) {
        float[] mean = {103.94f, 116.78f, 123.68f};
        float scale = 0.017f;
        // create data buffer
        float[] floatValues = new float[desWidth * desHeight * 3];
        FloatBuffer floatBuffer = FloatBuffer.wrap(floatValues, 0, desWidth * desHeight * 3);
        floatBuffer.rewind();
        // get image pixel
        int[] pixels = new int[desWidth * desHeight];
        Bitmap bm = Bitmap.createScaledBitmap(bitmap, desWidth, desHeight, false);
        bm.getPixels(pixels, 0, bm  .getWidth(), 0, 0, desWidth, desHeight);
        // pixel to data
        for (int clr : pixels) {
            floatBuffer.put(((clr & 0xFF) - mean[0]) * scale);
            floatBuffer.put((((clr >> 8) & 0xFF) - mean[1]) * scale);
            floatBuffer.put((((clr >> 16) & 0xFF) - mean[2]) * scale);
        }
        if (bm.isRecycled()) {
            bm.recycle();
        }
        return floatBuffer.array();
    }

    // TensorFlow model，get predict data
    public static float[] getScaledMatrix1(Bitmap bitmap, int desWidth,
                                           int desHeight) {
        // create data buffer
        float[] floatValues = new float[desWidth * desHeight * 3];
        FloatBuffer floatBuffer = FloatBuffer.wrap(floatValues, 0, desWidth * desHeight * 3);
        floatBuffer.rewind();
        // get image pixel
        int[] pixels = new int[desWidth * desHeight];
        Bitmap bm = Bitmap.createScaledBitmap(bitmap, desWidth, desHeight, false);
        bm.getPixels(pixels, 0, bm  .getWidth(), 0, 0, desWidth, desHeight);
        // pixel to data
        for (int clr : pixels) {
            floatBuffer.put((((clr >> 16) & 0xFF) - 128f) / 128f);
            floatBuffer.put((((clr >> 8) & 0xFF) - 128f) / 128f);
            floatBuffer.put(((clr & 0xFF) - 128f) / 128f);
        }
        if (bm.isRecycled()) {
            bm.recycle();
        }
        return floatBuffer.array();
    }


    // compress picture
    public static Bitmap getScaleBitmap(String filePath) {
        BitmapFactory.Options opt = new BitmapFactory.Options();
        opt.inJustDecodeBounds = true;
        BitmapFactory.decodeFile(filePath, opt);

        int bmpWidth = opt.outWidth;
        int bmpHeight = opt.outHeight;

        int maxSize = 500;

        // compress picture with inSampleSize
        opt.inSampleSize = 1;
        while (true) {
            if (bmpWidth / opt.inSampleSize < maxSize || bmpHeight / opt.inSampleSize < maxSize) {
                break;
            }
            opt.inSampleSize *= 2;
        }
        opt.inJustDecodeBounds = false;
        return BitmapFactory.decodeFile(filePath, opt);
    }

}