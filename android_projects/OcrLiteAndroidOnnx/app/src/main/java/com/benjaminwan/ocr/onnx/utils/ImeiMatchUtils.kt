package com.benjaminwan.ocr.onnx.utils

import com.orhanobut.logger.Logger
import java.util.regex.Matcher
import java.util.regex.Pattern

const val imeiRegex = "\\d{15}|\\d{17}"

fun getMatchImeiStr(text: String): String? {
    val matchALL = Regex(imeiRegex).find(text)
    if (matchALL != null) {
        Logger.i("match结果 matchALL:${matchALL.value}")
        return matchALL.value
    }
    return null
}

fun String.replaceBlank(): String {
    var dest = ""
    if (this.isNotEmpty()) {
        val p: Pattern = Pattern.compile("\\s*|\t|\r|\n")
        val m: Matcher = p.matcher(this)
        dest = m.replaceAll("")
    }
    return dest
}