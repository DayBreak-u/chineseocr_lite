package com.ncnn.demo;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.ContextCompat;
import androidx.core.app.ActivityCompat;
//import android.support.v4.app.ActivityCompat;
//import android.support.v4.content.ContextCompat;
import android.Manifest;
import android.app.Activity;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.net.Uri;
import android.os.Bundle;
import android.widget.TextView;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import android.text.method.ScrollingMovementMethod;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.List;


import com.bumptech.glide.Glide;
import com.bumptech.glide.load.engine.DiskCacheStrategy;
import com.bumptech.glide.request.RequestOptions;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
//    static {
//        System.loadLibrary("native-lib");
//    }
    private static final String TAG = MainActivity.class.getName();
    private textocr ocr = new textocr();
    private static final int USE_PHOTO = 1001;
    private Bitmap yourSelectedImage = null;
    private ImageView show_image;
    private TextView result_text;
    private boolean load_result = false;
    private int[] ddims = {1, 3, 352, 352}; //这里的维度的值要和train model的input 一一对应
    private List<String> resultLabel = new ArrayList<>();

    private void init_ocr() throws IOException {
        byte[] psenet_lite_mbv2_param = null;
        byte[] psenet_lite_mbv2_bin = null;
        byte[] crnn_lite_lstm_v2_param = null;
        byte[] crnn_lite_lstm_v2_bin = null;

        //psenet_lite_mbv2
        {
            //用io流读取二进制文件，最后存入到byte[]数组中
            InputStream assetsInputStream = getAssets().open("psenet_lite_mbv2.param.bin");// param：  网络结构文件
            int available = assetsInputStream.available();
            psenet_lite_mbv2_param = new byte[available];
            int byteCode = assetsInputStream.read(psenet_lite_mbv2_param);
            assetsInputStream.close();
        }
        {
            //用io流读取二进制文件，最后存入到byte上，转换为int型
            InputStream assetsInputStream = getAssets().open("psenet_lite_mbv2.bin");//bin：   model文件
            int available = assetsInputStream.available();
            psenet_lite_mbv2_bin = new byte[available];
            int byteCode = assetsInputStream.read(psenet_lite_mbv2_bin);
            assetsInputStream.close();
        }
        //crnn_lite_lstm_v2
        {
            //用io流读取二进制文件，最后存入到byte[]数组中
            InputStream assetsInputStream = getAssets().open("crnn_lite_lstm_v2.param.bin");// param：  网络结构文件
            int available = assetsInputStream.available();
            crnn_lite_lstm_v2_param = new byte[available];
            int byteCode = assetsInputStream.read(crnn_lite_lstm_v2_param);
            assetsInputStream.close();
        }
        {
            //用io流读取二进制文件，最后存入到byte上，转换为int型
            InputStream assetsInputStream = getAssets().open("crnn_lite_lstm_v2.bin");//bin：   model文件
            int available = assetsInputStream.available();
            crnn_lite_lstm_v2_bin = new byte[available];
            int byteCode = assetsInputStream.read(crnn_lite_lstm_v2_bin);
            assetsInputStream.close();
        }

        AssetManager am = getAssets();
        readCacheLabelFromLocalFile();
        load_result = ocr.Init(am,psenet_lite_mbv2_param, psenet_lite_mbv2_bin, crnn_lite_lstm_v2_param, crnn_lite_lstm_v2_bin,resultLabel);
        Log.d("load model", "ocr_load_model_result:" + load_result);

    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        try
        {
            init_ocr();//初始化模型
            Log.e("MainActivity", "initocr ok");
        } catch (IOException e) {
            Log.e("MainActivity", "initocr error");
        }

        init_view();

        // Example of a call to a native method
        //TextView tv = findViewById(R.id.sample_text);
        //tv.setText(stringFromJNI());
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    //public native String stringFromJNI();

    private void init_view() {
        request_permissions();
        show_image = (ImageView) findViewById(R.id.show_image);
        result_text = (TextView) findViewById(R.id.result_text);
        result_text.setMovementMethod(ScrollingMovementMethod.getInstance());
        yourSelectedImage = BitmapFactory.decodeResource(getResources(), R.drawable.test2);
        show_image.setImageBitmap(yourSelectedImage);
        Button use_photo = (Button) findViewById(R.id.use_photo);
        use_photo.setOnClickListener(new View.OnClickListener() {
            //使用dog
             @Override
            public void onClick(View arg0) {
//                yourSelectedImage = BitmapFactory.decodeResource(getResources(), R.drawable.tex);
//                show_image.setImageBitmap(yourSelectedImage);
                 if (!load_result) {
                     Toast.makeText(MainActivity.this, "never load model", Toast.LENGTH_SHORT).show();
                     return;
                 }
                 PhotoUtil.use_photo(MainActivity.this, USE_PHOTO);
            }
            //调用本地图库
//            @Override
//            public void onClick(View view) {
//                if (!load_result) {
//                    Toast.makeText(MainActivity.this, "never load model", Toast.LENGTH_SHORT).show();
//                    return;
//                }
//                PhotoUtil.use_photo(MainActivity.this, USE_PHOTO);
//            }

        });



        Button detect_photo = (Button) findViewById(R.id.detect_photo);
        detect_photo.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View arg0) {

                if (yourSelectedImage == null)
                    return;

                predict_image(yourSelectedImage);
            }

        });
    }




    void predict_image(Bitmap bmp) {
        Bitmap rgba = bmp.copy(Bitmap.Config.ARGB_8888, true);
        try {

            long start = System.currentTimeMillis();
            // get predict result
            //输入最长边尺寸 ，手机端需要根据手机的输入图片大小来设置，
            //目前的手机图片尺寸都很大，小尺寸精度不行
            int[] ocrres = ocr.Detect(rgba,1500);
            // time end
            long end = System.currentTimeMillis();
            long time = end - start;
            Log.d("result length", "length of result: " + String.valueOf(ocrres.length));
            result_text.setText("检测一次");
            int num = 0;
            String show_text="";
            while(num < ocrres.length){
                if(ocrres[num] == -1){
                    show_text += "\n";
                    num++;
                }
                else{
                    show_text += resultLabel.get(ocrres[num]);
                    num++;
                }

            }
            show_text += "\n";
            show_text+="time：" + time + "ms";
            result_text.setText(show_text);


        }catch (Exception e) {
            e.printStackTrace();
        }
    }
    // load label's name
    private void readCacheLabelFromLocalFile() {
        try {
            AssetManager assetManager = getApplicationContext().getAssets();
            BufferedReader reader = new BufferedReader(new InputStreamReader(assetManager.open("keys.txt")));//这里是label的文件
            String readLine = null;
            while ((readLine = reader.readLine()) != null) {
                resultLabel.add(readLine);

            }
            Log.e("labelCache", "error " + resultLabel.size());
            reader.close();
        } catch (Exception e) {
            Log.e("labelCache", "error " + e);
        }
    }
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        String image_path;
        RequestOptions options = new RequestOptions().skipMemoryCache(true).diskCacheStrategy(DiskCacheStrategy.NONE);
        if (resultCode == Activity.RESULT_OK) {
            switch (requestCode) {
                case USE_PHOTO:
                    if (data == null) {
                        Log.w(TAG, "user photo data is null");
                        return;
                    }
                    Uri image_uri = data.getData();
                    Glide.with(MainActivity.this).load(image_uri).apply(options).into(show_image);
                    // get image path from uri
                    image_path = PhotoUtil.get_path_from_URI(MainActivity.this, image_uri);
                    // predict image

                    //predict_image(image_path);
                    yourSelectedImage = PhotoUtil.getScaleBitmap(image_path);
                    break;
//                case START_CAMERA:
//                    // show photo
//                    Glide.with(MainActivity.this).load(camera_image_path).apply(options).into(show_image);
//                    // predict image
//                    predict_image(camera_image_path);
//                    break;
            }
        }
    }

    private Bitmap decodeUri(Uri selectedImage) throws FileNotFoundException {
        // Decode image size
        BitmapFactory.Options o = new BitmapFactory.Options();
        o.inJustDecodeBounds = true;
        BitmapFactory.decodeStream(getContentResolver().openInputStream(selectedImage), null, o);

        // The new size we want to scale to
        final int REQUIRED_SIZE = 400;

        // Find the correct scale value. It should be the power of 2.
        int width_tmp = o.outWidth, height_tmp = o.outHeight;
        int scale = 1;
        while (true) {
            if (width_tmp / 2 < REQUIRED_SIZE
                    || height_tmp / 2 < REQUIRED_SIZE) {
                break;
            }
            width_tmp /= 2;
            height_tmp /= 2;
            scale *= 2;
        }

        // Decode with inSampleSize
        BitmapFactory.Options o2 = new BitmapFactory.Options();
        o2.inSampleSize = scale;
        return BitmapFactory.decodeStream(getContentResolver().openInputStream(selectedImage), null, o2);
    }
    // request permissions
    private void request_permissions() {

        List<String> permissionList = new ArrayList<>();
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED) {
            permissionList.add(Manifest.permission.CAMERA);
        }

        if (ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            permissionList.add(Manifest.permission.WRITE_EXTERNAL_STORAGE);
        }

        if (ContextCompat.checkSelfPermission(this, Manifest.permission.READ_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            permissionList.add(Manifest.permission.READ_EXTERNAL_STORAGE);
        }

        // if list is not empty will request permissions
        if (!permissionList.isEmpty()) {
            ActivityCompat.requestPermissions(this, permissionList.toArray(new String[permissionList.size()]), 1);
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        switch (requestCode) {
            case 1:
                if (grantResults.length > 0) {
                    for (int i = 0; i < grantResults.length; i++) {

                        int grantResult = grantResults[i];
                        if (grantResult == PackageManager.PERMISSION_DENIED) {
                            String s = permissions[i];
                            Toast.makeText(this, s + " permission was denied", Toast.LENGTH_SHORT).show();
                        }
                    }
                }
                break;
        }
    }

}
