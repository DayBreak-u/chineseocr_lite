package com.benjaminwan.ocr

import com.benjaminwan.ocrlibrary.OcrEngine

fun main(args: Array<String>) {
    val jniLibDir = System.getProperty("java.library.path")
    println("java.library.path=$jniLibDir")
    if (args.isEmpty() || args.size < 2) {
        println("Usage: models/dir image/path")
        return
    }

    //------- init models/dir image/path -------
    val modelsDir = args[0]
    val imagePath = args[1]
    println("modelsDir=$modelsDir, imagePath=$imagePath")

    //------- numThread -------
    val numThread = if (args.size > 2) args[2].trim().toIntOrNull() ?: 4 else 4

    //------- padding -------
    val padding = if (args.size > 3) args[3].trim().toIntOrNull() ?: 50 else 50

    //------- imgResize -------
    val imgResize = if (args.size > 4) args[4].trim().toIntOrNull() ?: 0 else 0

    //------- boxScoreThresh -------
    val boxScoreThresh = if (args.size > 5) args[5].trim().toFloatOrNull() ?: 0.6f else 0.6f

    //------- boxThresh -------
    val boxThresh = if (args.size > 6) args[6].trim().toFloatOrNull() ?: 0.3f else 0.3f

    //------- minArea -------
    val minArea = if (args.size > 7) args[7].trim().toFloatOrNull() ?: 3f else 3f

    //------- unClipRatio -------
    val unClipRatio = if (args.size > 8) args[8].trim().toFloatOrNull() ?: 2f else 2f

    //------- doAngle -------
    val doAngleFlag = if (args.size > 9) args[9].trim().toIntOrNull() ?: 1 else 1
    val doAngle = (doAngleFlag == 1)

    //------- mostAngle -------
    val mostAngleFlag = if (args.size > 10) args[10].trim().toIntOrNull() ?: 1 else 1
    val mostAngle = (mostAngleFlag == 1)

    //------- get jni version -------
    val ocrEngine = OcrEngine()
    val version = ocrEngine.getVersion()
    println("version=$version")

    //------- setNumThread -------
    ocrEngine.setNumThread(numThread)

    //------- init Logger -------
    ocrEngine.initLogger(
        isConsole = true,//jni启用命令行输出
        isPartImg = true,
        isResultImg = true
    )
    ocrEngine.enableResultText(imagePath)

    //------- init Models -------
    val ret = ocrEngine.initModels(modelsDir)
    println("init Models $ret")

    //------- set param -------
    println("padding($padding) boxScoreThresh($boxScoreThresh) boxThresh($boxThresh) minArea($minArea) unClipRatio($unClipRatio) doAngle($doAngle) mostAngle($mostAngle)")
    ocrEngine.padding = padding //图像外接白框，用于提升识别率，文字框没有正确框住所有文字时，增加此值。
    ocrEngine.boxScoreThresh = boxScoreThresh //文字框置信度门限，文字框没有正确框住所有文字时，减小此值
    ocrEngine.boxThresh = boxThresh //请自行试验
    ocrEngine.minArea = minArea //请自行试验
    ocrEngine.unClipRatio = unClipRatio //单个文字框大小倍率，越大时单个文字框越大
    ocrEngine.doAngle = doAngle //启用(1)/禁用(0) 文字方向检测，只有图片倒置的情况下(旋转90~270度的图片)，才需要启用文字方向检测
    ocrEngine.mostAngle = mostAngle //启用(1)/禁用(0) 角度投票(整张图片以最大可能文字方向来识别)，当禁用文字方向检测时，此项也不起作用

    //------- start detect -------
    val ocrResult =
        ocrEngine.detect(imagePath, reSize = imgResize) //按图像长边进行总体缩放，放大增加识别耗时但精度更高，缩小减小耗时但精度降低，reSize=0代表不缩放

    //------- print result -------
    println(ocrResult.toString())
    return
}