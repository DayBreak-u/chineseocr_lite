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
        System.out.printf("modelsDir=%s, imagePath=%s", modelsDir, imagePath);

        //------- numThread -------
        int numThread = 4;
        if (args.length > 2 && args[2] != null && args[2].length() > 0) {
            try {
                numThread = Integer.parseInt(args[2].trim());
            } catch (NumberFormatException e) {
                e.printStackTrace();
            }
        }

        //------- padding -------
        int padding = 50;
        if (args.length > 3 && args[3] != null && args[3].length() > 0) {
            try {
                padding = Integer.parseInt(args[3].trim());
            } catch (NumberFormatException e) {
                e.printStackTrace();
            }
        }

        //------- imgResize -------
        int imgResize = 0;
        if (args.length > 4 && args[4] != null && args[4].length() > 0) {
            try {
                imgResize = Integer.parseInt(args[4].trim());
            } catch (NumberFormatException e) {
                e.printStackTrace();
            }
        }

        //------- boxScoreThresh -------
        float boxScoreThresh = 0.6f;
        if (args.length > 5 && args[5] != null && args[5].length() > 0) {
            try {
                boxScoreThresh = Float.parseFloat(args[5].trim());
            } catch (NumberFormatException e) {
                e.printStackTrace();
            }
        }

        //------- boxThresh -------
        float boxThresh = 0.3f;
        if (args.length > 6 && args[6] != null && args[6].length() > 0) {
            try {
                boxThresh = Float.parseFloat(args[6].trim());
            } catch (NumberFormatException e) {
                e.printStackTrace();
            }
        }

        //------- minArea -------
        float minArea = 3f;
        if (args.length > 7 && args[7] != null && args[7].length() > 0) {
            try {
                minArea = Float.parseFloat(args[7].trim());
            } catch (NumberFormatException e) {
                e.printStackTrace();
            }
        }

        //------- unClipRatio -------
        float unClipRatio = 2f;
        if (args.length > 8 && args[8] != null && args[8].length() > 0) {
            try {
                unClipRatio = Float.parseFloat(args[8].trim());
            } catch (NumberFormatException e) {
                e.printStackTrace();
            }
        }

        //------- doAngle -------
        int doAngleFlag = 1;
        if (args.length > 9 && args[9] != null && args[9].length() > 0) {
            try {
                doAngleFlag = Integer.parseInt(args[9].trim());
            } catch (NumberFormatException e) {
                e.printStackTrace();
            }
        }
        boolean doAngle = (doAngleFlag == 1);

        //------- mostAngle -------
        int mostAngleFlag = 1;
        if (args.length > 10 && args[10] != null && args[10].length() > 0) {
            try {
                mostAngleFlag = Integer.parseInt(args[10].trim());
            } catch (NumberFormatException e) {
                e.printStackTrace();
            }
        }
        boolean mostAngle = (mostAngleFlag == 1);

        //------- get jni version -------
        OcrEngine ocrEngine = new OcrEngine();
        String version = ocrEngine.getVersion();
        System.out.println("version=" + version);

        //------- setNumThread -------
        ocrEngine.setNumThread(numThread);

        //------- init Logger -------
        ocrEngine.initLogger(
                true,
                true,
                true
        );
        ocrEngine.enableResultText(imagePath);

        //------- init Models -------
        boolean ret = ocrEngine.initModels(modelsDir);
        System.out.println("init Models " + ret);

        //------- set param -------
        System.out.printf("padding(%d) boxScoreThresh(%f) boxThresh(%f) minArea(%f) unClipRatio(%f) doAngle(%b) mostAngle(%b)", padding, boxScoreThresh, boxThresh, minArea, unClipRatio, doAngle, mostAngle);
        ocrEngine.setPadding(padding); //图像外接白框，用于提升识别率，文字框没有正确框住所有文字时，增加此值。
        ocrEngine.setBoxScoreThresh(boxScoreThresh); //文字框置信度门限，文字框没有正确框住所有文字时，减小此值
        ocrEngine.setBoxThresh(boxThresh); //请自行试验
        ocrEngine.setMinArea(minArea); //请自行试验
        ocrEngine.setUnClipRatio(unClipRatio); //单个文字框大小倍率，越大时单个文字框越大
        ocrEngine.setDoAngle(doAngle); //启用(1)/禁用(0) 文字方向检测，只有图片倒置的情况下(旋转90~270度的图片)，才需要启用文字方向检测
        ocrEngine.setMostAngle(mostAngle); //启用(1)/禁用(0) 角度投票(整张图片以最大可能文字方向来识别)，当禁用文字方向检测时，此项也不起作用
        //------- start detect -------
        OcrResult ocrResult = ocrEngine.detect(imagePath, imgResize); //按图像长边进行总体缩放，放大增加识别耗时但精度更高，缩小减小耗时但精度降低，reSize=0代表不缩放

        //------- print result -------
        System.out.println(ocrResult.toString());
        return;
    }
}
