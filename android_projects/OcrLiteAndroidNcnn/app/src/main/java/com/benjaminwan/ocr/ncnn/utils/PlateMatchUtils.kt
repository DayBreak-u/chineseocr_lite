package com.benjaminwan.ocr.ncnn.utils

import com.orhanobut.logger.Logger
import java.util.regex.Matcher
import java.util.regex.Pattern

const val plateRegex = "^(([京津沪渝冀豫云辽黑湘皖鲁新苏浙赣鄂桂甘晋蒙陕吉闽贵粤青藏川宁琼使领][A-Z](([0-9]{5}[DF])|([DF]([A-HJ-NP-Z0-9])[0-9]{4})))|([京津沪渝冀豫云辽黑湘皖鲁新苏浙赣鄂桂甘晋蒙陕吉闽贵粤青藏川宁琼使领][A-Z][A-HJ-NP-Z0-9]{4}[A-HJ-NP-Z0-9挂学警港澳使领]))\$"

fun getMatchPlateStr(text: String): String? {
    val matchALL = Regex(plateRegex).find(text)
    if (matchALL != null) {
        Logger.i("match结果 matchALL:${matchALL.value}")
        return matchALL.value
    }
    return null
}

fun String.trimBlankAndSymbols(): String {
    var dest = ""
    if (this.isNotEmpty()) {
        val p: Pattern = Pattern.compile("\\p{P}|\\p{S}|\\s*|\t|\r|\n")
        val m: Matcher = p.matcher(this)
        dest = m.replaceAll("")
    }
    return dest
}