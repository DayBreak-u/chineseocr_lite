package com.benjaminwan.ocrlibrary

sealed class OcrOutput

object OcrStop : OcrOutput()
object OcrFailed : OcrOutput()

data class OcrResult(
    val dbNetTime: Double,
    val textBlocks: ArrayList<TextBlock>,
    var detectTime: Double,
    var strRes: String
) : OcrOutput()

data class Point(var x: Int, var y: Int)

data class TextBlock(
    val boxPoint: ArrayList<Point>, var boxScore: Float,
    val angleIndex: Int, val angleScore: Float, val angleTime: Double,
    val text: String, val charScores: FloatArray, val crnnTime: Double,
    val blockTime: Double
) {
    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (javaClass != other?.javaClass) return false

        other as TextBlock

        if (boxPoint != other.boxPoint) return false
        if (boxScore != other.boxScore) return false
        if (angleIndex != other.angleIndex) return false
        if (angleScore != other.angleScore) return false
        if (angleTime != other.angleTime) return false
        if (text != other.text) return false
        if (!charScores.contentEquals(other.charScores)) return false
        if (crnnTime != other.crnnTime) return false
        if (blockTime != other.blockTime) return false

        return true
    }

    override fun hashCode(): Int {
        var result = boxPoint.hashCode()
        result = 31 * result + boxScore.hashCode()
        result = 31 * result + angleIndex
        result = 31 * result + angleScore.hashCode()
        result = 31 * result + angleTime.hashCode()
        result = 31 * result + text.hashCode()
        result = 31 * result + charScores.contentHashCode()
        result = 31 * result + crnnTime.hashCode()
        result = 31 * result + blockTime.hashCode()
        return result
    }
}
