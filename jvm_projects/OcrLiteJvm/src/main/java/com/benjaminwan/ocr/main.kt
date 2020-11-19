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

    //------- get jni version -------
    val ocrEngine = OcrEngine()
    val version = ocrEngine.getVersion()
    println("version=$version")

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
    ocrEngine.boxScoreThresh = 0.6f //文字框置信度门限，文字框没有正确框住所有文字时，减小此值
    ocrEngine.boxThresh = 0.3f //请自行试验
    ocrEngine.miniArea = 3f //请自行试验
    ocrEngine.unClipRatio = 1.8f //单个文字框大小倍率，越大时单个文字框越大
    ocrEngine.doAngle = true //启用(1)/禁用(0) 文字方向检测，只有图片倒置的情况下(旋转90~270度的图片)，才需要启用文字方向检测
    ocrEngine.mostAngle = true //启用(1)/禁用(0) 角度投票(整张图片以最大可能文字方向来识别)，当禁用文字方向检测时，此项也不起作用

    //------- start detect -------
    val ocrResult = ocrEngine.detect(imagePath, reSize = 0) //按图像长边进行总体缩放，放大增加识别耗时但精度更高，缩小减小耗时但精度降低，reSize=0代表不缩放

    //------- print result -------
    println(ocrResult.toString())
    return
}