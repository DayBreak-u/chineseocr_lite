package com.benjaminwan.ocr.dbnet

import android.graphics.Bitmap

data class OcrResult(
    val bitmap: Bitmap,
    val text: String = "",
    val time: String = ""
)