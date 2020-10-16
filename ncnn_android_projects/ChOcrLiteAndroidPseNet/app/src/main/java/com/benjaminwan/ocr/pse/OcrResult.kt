package com.benjaminwan.ocr.pse

import android.graphics.Bitmap

data class OcrResult(
    val bitmap: Bitmap,
    val text: String = "",
    val time: String = ""
)