package com.benjaminwan.ocr.onnx.utils

import java.text.DecimalFormat

/**
 * 数字格式化
 * @param  pattern 格式
 * 例"#.000000" 保留6位小数
 * @return 返回格式化后的字符串
 */
fun Double.format(pattern: String): String {
    if (pattern.isEmpty()) {
        return this.toString()
    }
    return DecimalFormat(pattern).format(this)
}

/**
 * 数字格式化
 * @param  pattern 格式
 * @return 返回格式化后的字符串
 */
fun Long.format(pattern: String): String {
    if (pattern.isEmpty()) {
        return this.toString()
    }
    return DecimalFormat(pattern).format(this)
}

/**
 * 数字格式化
 * @param  pattern 格式
 * @return 返回格式化后的字符串
 */
fun Int.format(pattern: String): String {
    if (pattern.isEmpty()) {
        return this.toString()
    }
    return DecimalFormat(pattern).format(this)
}

/**
 * 数字格式化
 * @param  pattern 格式
 * @return 返回格式化后的字符串
 */
fun Float.format(pattern: String): String {
    if (pattern.isEmpty()) {
        return this.toString()
    }
    return DecimalFormat(pattern).format(this)
}