package com.benjaminwan.ocrlibrary

import android.content.Context
import android.content.res.AssetManager
import android.graphics.Bitmap

class OcrEngine(context: Context,dbnetmodelPath: String,angelmodelPath: String, crnnmodelPath: String) {
    companion object {
        const val numThread: Int = 4
    }

    init {
        System.loadLibrary("OcrLite")
        val ret = init(context.assets, numThread, dbnetmodelPath,angelmodelPath,crnnmodelPath)
        if (!ret) throw IllegalArgumentException()
    }

    var padding: Int = 50
    var boxScoreThresh: Float = 0.5f
    var boxThresh: Float = 0.3f
    var unClipRatio: Float = 2.0f
    var doAngle: Boolean = true
    var mostAngle: Boolean = true

    fun detect(input: Bitmap, output: Bitmap, maxSideLen: Int) =
        detect(
            input, output, padding, maxSideLen,
            boxScoreThresh, boxThresh,
            unClipRatio, doAngle, mostAngle
        )

    external fun init(assetManager: AssetManager, numThread: Int,dbnetmodelPath: String,angelmodelPath: String, crnnmodelPath: String): Boolean
    external fun detect(
        input: Bitmap, output: Bitmap, padding: Int, maxSideLen: Int,
        boxScoreThresh: Float, boxThresh: Float,
        unClipRatio: Float, doAngle: Boolean, mostAngle: Boolean
    ): OcrResult

    external fun benchmark(input: Bitmap, loop: Int): Double

}