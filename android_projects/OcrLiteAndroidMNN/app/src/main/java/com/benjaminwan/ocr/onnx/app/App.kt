package com.benjaminwan.ocr.onnx.app

import android.app.Application
import android.content.Context
import android.util.Log
import com.benjaminwan.ocr.onnx.BuildConfig
import com.benjaminwan.ocrlibrary.OcrEngine
import com.orhanobut.logger.AndroidLogAdapter
import com.orhanobut.logger.LogStrategy
import com.orhanobut.logger.Logger
import com.orhanobut.logger.PrettyFormatStrategy
import java.io.File
import java.io.FileOutputStream

class App : Application() {
    companion object {
        lateinit var INSTANCE: App
        lateinit var ocrEngine: OcrEngine
    }

    override fun onCreate() {
        super.onCreate()

        val dbnetModelName = "dbnet.mnn"
        val dbnetmodelPath = "$cacheDir/$dbnetModelName"

        val angelModelName = "angle_net.mnn"
        val angelmodelPath = "$cacheDir/$angelModelName"

        val crnnModelName = "crnn_lite_lstm.mnn"
        val crnnmodelPath = "$cacheDir/$crnnModelName"

        try {
            copyAssetResource2File(baseContext,dbnetModelName,dbnetmodelPath)
            copyAssetResource2File(baseContext,angelModelName,angelmodelPath)
            copyAssetResource2File(baseContext,crnnModelName,crnnmodelPath)
        } catch (e: Throwable) {
            throw RuntimeException(e)
        }

        INSTANCE = this
        initLogger()
        initOCREngine(dbnetmodelPath,angelmodelPath,crnnmodelPath)
    }


    private fun initOCREngine(dbnetmodelPath: String,angelmodelPath: String, crnnmodelPath: String) {
        ocrEngine = OcrEngine(this.applicationContext,dbnetmodelPath,angelmodelPath,crnnmodelPath)
    }

    private fun initLogger() {
        val formatStrategy = PrettyFormatStrategy.newBuilder()
            .showThreadInfo(true)
            .tag("OcrLite")
            .logStrategy(LogCatStrategy())
            .build()
        Logger.addLogAdapter(object : AndroidLogAdapter(formatStrategy) {
            override fun isLoggable(priority: Int, tag: String?): Boolean = BuildConfig.DEBUG
        })
    }

    inner class LogCatStrategy : LogStrategy {

        private var last: Int = 0

        override fun log(priority: Int, tag: String?, message: String) {
            Log.println(priority, randomKey() + tag!!, message)
        }

        private fun randomKey(): String {
            var random = (10 * Math.random()).toInt()
            if (random == last) {
                random = (random + 1) % 10
            }
            last = random
            return random.toString()
        }
    }

    fun copyAssetResource2File(
        context: Context,
        assetsFile: String?,
        outFile: String?
    ) {
        val `is` = context.assets.open(assetsFile!!)
        val outF = File(outFile)
        val fos = FileOutputStream(outF)
        var byteCount: Int
        val buffer = ByteArray(1024)
        while (`is`.read(buffer).also { byteCount = it } != -1) {
            fos.write(buffer, 0, byteCount)
        }
        fos.flush()
        `is`.close()
        fos.close()
        outF.setReadable(true)
    }
}