package com.benjaminwan.ocr.onnx

import android.graphics.Bitmap

data class OcrResult(
    val bitmap: Bitmap,
    val text: String = "",
    val time: String = ""
)