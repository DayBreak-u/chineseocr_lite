package com.benjaminwan.ocr.ncnn.utils

import android.content.ClipData
import android.content.ClipboardManager
import android.content.Context

fun Context.toClipboard(text: String) {
    val clipboardManager =
        this.getSystemService(Context.CLIPBOARD_SERVICE) as ClipboardManager
    // 创建普通字符型ClipData
    val mClipData = ClipData.newPlainText(packageName, text)
    // 将ClipData内容放到系统剪贴板里。
    clipboardManager.setPrimaryClip(mClipData)
    this.showToast("已复制到剪切板!")
}

fun Context.fromClipboard(): String? {
    val clipboardManager =
        this.getSystemService(Context.CLIPBOARD_SERVICE) as ClipboardManager
    return if (clipboardManager.hasPrimaryClip()) {
        val clipData = clipboardManager.primaryClip
        if (clipData != null) {
            if (clipData.itemCount > 0) {
                clipData.getItemAt(0).text.toString()
            } else {
                null
            }
        } else {
            null
        }
    } else {
        null
    }
}