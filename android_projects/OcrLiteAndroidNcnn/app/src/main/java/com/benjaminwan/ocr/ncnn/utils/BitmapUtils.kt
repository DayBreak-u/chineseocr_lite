package com.benjaminwan.ocr.ncnn.utils

import android.content.Context
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.net.Uri
import java.io.FileNotFoundException

@Throws(FileNotFoundException::class)
fun Context.decodeUri(imgUri: Uri): Bitmap? {
    // Decode image size
    val options = BitmapFactory.Options()
    options.inJustDecodeBounds = false
    options.inMutable = true
    options.inPreferredConfig = Bitmap.Config.ARGB_8888
    return BitmapFactory.decodeStream(contentResolver.openInputStream(imgUri), null, options)
}