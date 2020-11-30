package com.benjaminwan.ocr.onnx.models

import android.os.Parcelable
import com.benjaminwan.ocrlibrary.OcrOutput
import kotlinx.android.parcel.Parcelize

@Parcelize
data class IdCardFront(
    val name: String,//姓名
    val nation: String,//民族
    val address: String,//地址
    val number: String,//身份证号
) : Parcelable, OcrOutput() {
    //性别
    val gender: String
        get() {
            val genderNumber = number[16].toInt()
            return if (genderNumber % 2 == 1) "男" else "女"
        }

    //出生日期
    val birth: String
        get() {
            val year = number.substring(6..9)
            val month = number.substring(10..11)
            val date = number.substring(12..13)
            return "$year-$month-$date"
        }

    fun isEmpty(): Boolean =
        name.isEmpty() && nation.isEmpty() && address.isEmpty() && number.isEmpty()


}