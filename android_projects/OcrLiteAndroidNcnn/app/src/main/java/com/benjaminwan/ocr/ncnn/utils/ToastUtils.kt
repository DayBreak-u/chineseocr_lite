package com.benjaminwan.ocr.ncnn.utils

import android.app.Activity
import android.content.Context
import android.widget.Toast
import androidx.annotation.StringRes
import androidx.fragment.app.Fragment

fun Context.showToast(msg: CharSequence, duration: Int = Toast.LENGTH_SHORT) {
    Toast.makeText(this, msg, duration).show()
}

fun Context.showToast(@StringRes resId: Int, duration: Int = Toast.LENGTH_SHORT) {
    Toast.makeText(this, resId, duration).show()
}

/**
 * 显示Toast
 * @param  msg
 */
fun Activity.showToast(msg: CharSequence, duration: Int = Toast.LENGTH_SHORT) {
    applicationContext.showToast(msg, duration)
}

/**
 * 显示Toast
 * @param  msg
 */
fun Activity.showToast(@StringRes msg: Int, duration: Int = Toast.LENGTH_SHORT) {
    applicationContext.showToast(msg, duration)
}

/**
 * 显示Toast
 * @param  msg
 */
fun Fragment.showToast(msg: CharSequence, duration: Int = Toast.LENGTH_SHORT) {
    requireActivity().applicationContext.showToast(msg, duration)
}

/**
 * 显示Toast
 * @param  msg
 */
fun Fragment.showToast(@StringRes msg: Int, duration: Int = Toast.LENGTH_SHORT) {
    requireActivity().applicationContext.showToast(msg, duration)
}