package com.benjaminwan.ocr.ncnn.utils

import com.orhanobut.logger.Logger
import java.util.regex.Matcher
import java.util.regex.Pattern

const val idCardNumberRegex =
    "^[1-9]\\d{7}((0\\d)|(1[0-2]))(([0|1|2]\\d)|3[0-1])\\d{3}\$|^[1-9]\\d{5}[1-9]\\d{3}((0\\d)|(1[0-2]))(([0|1|2]\\d)|3[0-1])\\d{3}([0-9]|X)\$"


fun getMatchIdCardNumberStr(text: String): String? {
    val matchALL = Regex(idCardNumberRegex).find(text)
    if (matchALL != null) {
        Logger.i("match结果 matchALL:${matchALL.value}")
        return matchALL.value
    }
    return null
}

fun String.trimSymbols(): String {
    var dest = ""
    if (this.isNotEmpty()) {
        val p: Pattern = Pattern.compile("\\p{P}|\\p{S}")
        val m: Matcher = p.matcher(this)
        dest = m.replaceAll("")
    }
    return dest
}