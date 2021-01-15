package com.benjaminwan.ocrlibrary

class OcrEngine() {
    init {
        try {
            System.loadLibrary("OcrLiteNcnn")
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }

    var padding: Int = 50
    var boxScoreThresh: Float = 0.6f
    var boxThresh: Float = 0.3f
    var unClipRatio: Float = 2.0f
    var doAngle: Boolean = true
    var mostAngle: Boolean = true

    fun detect(input: String, maxSideLen: Int) = detect(
        input, padding, maxSideLen,
        boxScoreThresh, boxThresh,
        unClipRatio, doAngle, mostAngle
    )

    external fun setNumThread(numThread: Int): Boolean

    external fun initLogger(
        isConsole: Boolean,
        isPartImg: Boolean,
        isResultImg: Boolean
    )

    external fun enableResultText(imagePath: String)

    external fun initModels(
        modelsDir: String,
        detName: String,
        clsName: String,
        recName: String,
        keysName: String
    ): Boolean

    external fun setGpuIndex(gpuIndex: Int)

    external fun getVersion(): String

    external fun detect(
        input: String, padding: Int, maxSideLen: Int,
        boxScoreThresh: Float, boxThresh: Float,
        unClipRatio: Float, doAngle: Boolean, mostAngle: Boolean
    ): OcrResult

}