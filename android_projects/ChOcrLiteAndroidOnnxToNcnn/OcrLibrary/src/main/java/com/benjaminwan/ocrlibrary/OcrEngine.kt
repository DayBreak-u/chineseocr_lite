package com.benjaminwan.ocrlibrary

import android.content.Context
import android.content.res.AssetManager
import android.graphics.Bitmap

class OcrEngine(context: Context) {
    companion object {
        const val numThread: Int = 4
    }

    init {
        System.loadLibrary("OcrLite")
        val ret = init(context.assets, numThread)
        if (!ret) throw IllegalArgumentException()
    }

    var padding: Int = 50
    var boxScoreThresh: Float = 0.6f
    var boxThresh: Float = 0.3f
    var miniArea: Float = 3f
    var unClipRatio: Float = 2.0f
    var doAngle: Boolean = true
    var mostAngle: Boolean = true

    fun detect(input: Bitmap, output: Bitmap, reSize: Int) =
        detect(
            input, output, padding, reSize,
            boxScoreThresh, boxThresh, miniArea,
            unClipRatio, doAngle, mostAngle
        )

    external fun init(assetManager: AssetManager, numThread: Int): Boolean
    external fun detect(
        input: Bitmap, output: Bitmap, padding: Int, reSize: Int,
        boxScoreThresh: Float, boxThresh: Float, miniArea: Float,
        unClipRatio: Float, doAngle: Boolean, mostAngle: Boolean
    ): OcrResult

}