package com.benjaminwan.ocr;

import com.benjaminwan.ocrlibrary.OcrEngine;
import com.benjaminwan.ocrlibrary.OcrResult;

public class Main {
    public static void main(String[] args) {
        String jniLibDir = System.getProperty("java.library.path");
        System.out.println("java.library.path=" + jniLibDir);

        if (args.length < 2) {
            System.out.println("Usage: models/dir image/path");
            return;
        }

        //------- init models/dir image/path -------
        String modelsDir = args[0];
        String imagePath = args[1];
        System.out.println(String.format("modelsDir=%s, imagePath=%s", modelsDir, imagePath));

        //------- get jni version -------
        OcrEngine ocrEngine = new OcrEngine();
        String version = ocrEngine.getVersion();
        System.out.println("version=" + version);

        //------- init Logger -------
        ocrEngine.initLogger(
                true,
                true,
                true,
                true,
                true
        );
        ocrEngine.enableResultText(imagePath);

        //------- init Models -------
        boolean ret = ocrEngine.initModels(modelsDir);
        System.out.println("init Models " + ret);

        //------- set param -------
        ocrEngine.setBoxScoreThresh(0.6f);//文字框置信度门限，文字框没有正确框住所有文字时，减小此值
        ocrEngine.setBoxThresh(0.3f); //请自行试验
        ocrEngine.setMiniArea(3f); //请自行试验
        ocrEngine.setUnClipRatio(1.8f); //单个文字框大小倍率，越大时单个文字框越大
        ocrEngine.setDoAngle(true); //启用(1)/禁用(0) 文字方向检测，只有图片倒置的情况下(旋转90~270度的图片)，才需要启用文字方向检测
        ocrEngine.setMostAngle(true); //启用(1)/禁用(0) 角度投票(整张图片以最大可能文字方向来识别)，当禁用文字方向检测时，此项也不起作用
        //------- start detect -------
        OcrResult ocrResult = ocrEngine.detect(imagePath, 0); //按图像长边进行总体缩放，放大增加识别耗时但精度更高，缩小减小耗时但精度降低，reSize=0代表不缩放

        //------- print result -------
        System.out.println(ocrResult.toString());
        return;
    }
}
