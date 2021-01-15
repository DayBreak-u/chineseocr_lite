package com.benjaminwan.ocr;

import com.benjaminwan.ocrlibrary.OcrEngine;
import com.benjaminwan.ocrlibrary.OcrResult;

public class Main {
    public static void main(String[] args) {
        String jniLibDir = System.getProperty("java.library.path");
        System.out.println("java.library.path=" + jniLibDir);

        if (args.length < 6) {
            System.out.println("Usage: models/dir det/name cls/name rec/name keys/name image/path");
            return;
        }

        //------- init models/dir image/path -------
        String modelsDir = args[0];
        String detName = args[1];
        String clsName = args[2];
        String recName = args[3];
        String keysName = args[4];
        String imagePath = args[5];
        System.out.printf("modelsDir=%s, detName=%s, clsName=%s, recName=%s, keysName=%s, imagePath=%s", modelsDir, detName, clsName, recName, keysName, imagePath);

        //------- numThread -------
        int numThread = 4;
        if (args.length > 6 && args[6] != null && args[6].length() > 0) {
            try {
                numThread = Integer.parseInt(args[6].trim());
            } catch (NumberFormatException e) {
                e.printStackTrace();
            }
        }

        //------- padding -------
        int padding = 50;
        if (args.length > 7 && args[7] != null && args[7].length() > 0) {
            try {
                padding = Integer.parseInt(args[7].trim());
            } catch (NumberFormatException e) {
                e.printStackTrace();
            }
        }

        //------- maxSideLen -------
        int maxSideLen = 1024;
        if (args.length > 8 && args[8] != null && args[8].length() > 0) {
            try {
                maxSideLen = Integer.parseInt(args[8].trim());
            } catch (NumberFormatException e) {
                e.printStackTrace();
            }
        }

        //------- boxScoreThresh -------
        float boxScoreThresh = 0.6f;
        if (args.length > 9 && args[9] != null && args[9].length() > 0) {
            try {
                boxScoreThresh = Float.parseFloat(args[9].trim());
            } catch (NumberFormatException e) {
                e.printStackTrace();
            }
        }

        //------- boxThresh -------
        float boxThresh = 0.3f;
        if (args.length > 10 && args[10] != null && args[10].length() > 0) {
            try {
                boxThresh = Float.parseFloat(args[10].trim());
            } catch (NumberFormatException e) {
                e.printStackTrace();
            }
        }

        //------- unClipRatio -------
        float unClipRatio = 2f;
        if (args.length > 11 && args[11] != null && args[11].length() > 0) {
            try {
                unClipRatio = Float.parseFloat(args[11].trim());
            } catch (NumberFormatException e) {
                e.printStackTrace();
            }
        }

        //------- doAngle -------
        int doAngleFlag = 1;
        if (args.length > 12 && args[12] != null && args[12].length() > 0) {
            try {
                doAngleFlag = Integer.parseInt(args[12].trim());
            } catch (NumberFormatException e) {
                e.printStackTrace();
            }
        }
        boolean doAngle = (doAngleFlag == 1);

        //------- mostAngle -------
        int mostAngleFlag = 1;
        if (args.length > 13 && args[13] != null && args[13].length() > 0) {
            try {
                mostAngleFlag = Integer.parseInt(args[13].trim());
            } catch (NumberFormatException e) {
                e.printStackTrace();
            }
        }
        boolean mostAngle = (mostAngleFlag == 1);

        //------- gpuIndex -------
        int gpuIndex = 0;
        if (args.length > 14 && args[14] != null && args[14].length() > 0) {
            try {
                gpuIndex = Integer.parseInt(args[14].trim());
            } catch (NumberFormatException e) {
                e.printStackTrace();
            }
        }
        System.out.println("gpuIndex=" + gpuIndex);

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
        ocrEngine.setGpuIndex(gpuIndex);//GPU0一般为默认GPU，参数选项：使用CPU(-1)/使用GPU0(0)/使用GPU1(1)/...
        //------- init Models -------
        boolean initModelsRet = ocrEngine.initModels(modelsDir, detName, clsName, recName, keysName);
        if (!initModelsRet) {
            System.out.println("Error in models initialization, please check the models/keys path!");
            return;
        }

        //------- set param -------
        System.out.printf("padding(%d) boxScoreThresh(%f) boxThresh(%f) unClipRatio(%f) doAngle(%b) mostAngle(%b)", padding, boxScoreThresh, boxThresh, unClipRatio, doAngle, mostAngle);
        ocrEngine.setPadding(padding); //图像外接白框，用于提升识别率，文字框没有正确框住所有文字时，增加此值。
        ocrEngine.setBoxScoreThresh(boxScoreThresh); //文字框置信度门限，文字框没有正确框住所有文字时，减小此值
        ocrEngine.setBoxThresh(boxThresh); //请自行试验
        ocrEngine.setUnClipRatio(unClipRatio); //单个文字框大小倍率，越大时单个文字框越大
        ocrEngine.setDoAngle(doAngle); //启用(1)/禁用(0) 文字方向检测，只有图片倒置的情况下(旋转90~270度的图片)，才需要启用文字方向检测
        ocrEngine.setMostAngle(mostAngle); //启用(1)/禁用(0) 角度投票(整张图片以最大可能文字方向来识别)，当禁用文字方向检测时，此项也不起作用
        //------- start detect -------
        OcrResult ocrResult = ocrEngine.detect(imagePath, maxSideLen); //按图像长边进行总体缩放，放大增加识别耗时但精度更高，缩小减小耗时但精度降低，maxSideLen=0代表不缩放

        //使用native方法，可以让OcrEngine成为单例
        //OcrResult ocrResult = ocrEngine.detect(imagePath, padding, maxSideLen, boxScoreThresh, boxThresh, unClipRatio, doAngle, mostAngle);

        //------- print result -------
        System.out.println(ocrResult.toString());
        return;
    }
}
