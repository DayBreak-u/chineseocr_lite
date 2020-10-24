package com.benjaminwan.ocr.onnx

import android.app.Activity
import android.content.Intent
import android.graphics.Bitmap
import android.os.Bundle
import android.util.Log
import android.view.View
import android.widget.SeekBar
import androidx.appcompat.app.AppCompatActivity
import com.benjaminwan.ocr.onnx.dialog.DebugDialog
import com.benjaminwan.ocr.onnx.dialog.TextResultDialog
import com.benjaminwan.ocrlibrary.OcrEngine
import com.benjaminwan.ocrlibrary.OcrResult
import com.bumptech.glide.Glide
import com.bumptech.glide.load.engine.DiskCacheStrategy
import com.bumptech.glide.request.RequestOptions
import com.uber.autodispose.android.lifecycle.autoDisposable
import io.reactivex.Single
import io.reactivex.android.schedulers.AndroidSchedulers
import io.reactivex.schedulers.Schedulers
import kotlinx.android.synthetic.main.activity_gallery.*
import kotlin.math.max

class GalleryActivity : AppCompatActivity(), View.OnClickListener, SeekBar.OnSeekBarChangeListener {

    private lateinit var ocrEngine: OcrEngine
    private var selectedImg: Bitmap? = null
    private var ocrResult: OcrResult? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_gallery)
        ocrEngine = OcrEngine(applicationContext)
        selectBtn.setOnClickListener(this)
        detectBtn.setOnClickListener(this)
        resultBtn.setOnClickListener(this)
        debugBtn.setOnClickListener(this)
        updatePadding(ocrEngine.padding)
        updateBoxScoreThresh((ocrEngine.boxScoreThresh * 100).toInt())
        updateBoxThresh((ocrEngine.boxThresh * 100).toInt())
        updateMinArea(ocrEngine.miniArea.toInt())
        updateScaleWidth((ocrEngine.scaleWidth * 10).toInt())
        updateScaleHeight((ocrEngine.scaleHeight * 10).toInt())
        paddingSeekBar.setOnSeekBarChangeListener(this)
        boxScoreThreshSeekBar.setOnSeekBarChangeListener(this)
        boxThreshSeekBar.setOnSeekBarChangeListener(this)
        minAreaSeekBar.setOnSeekBarChangeListener(this)
        scaleSeekBar.setOnSeekBarChangeListener(this)
        scaleWidthSeekBar.setOnSeekBarChangeListener(this)
        scaleHeightSeekBar.setOnSeekBarChangeListener(this)
    }

    override fun onClick(view: View?) {
        view ?: return
        when (view.id) {
            R.id.selectBtn -> {
                val intent = Intent(Intent.ACTION_PICK).apply {
                    type = "image/*"
                }
                startActivityForResult(
                    intent, REQUEST_SELECT_IMAGE
                )
            }
            R.id.detectBtn -> {
                val img = selectedImg ?: return
                val scale = scaleSeekBar.progress.toFloat() / 100.toFloat()
                val maxSize = max(img.width, img.height)
                val reSize = (scale * maxSize).toInt()
                detect(img, reSize)
            }
            R.id.resultBtn -> {
                val result = ocrResult ?: return
                TextResultDialog.instance
                    .setTitle("识别结果")
                    .setContent(result.strRes)
                    .show(supportFragmentManager, "TextResultDialog")
            }
            R.id.debugBtn -> {
                val result = ocrResult ?: return
                DebugDialog.instance
                    .setTitle("调试信息")
                    .setResult(result)
                    .show(supportFragmentManager, "DebugDialog")
            }
            else -> {
            }
        }
    }

    override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
        seekBar ?: return
        when (seekBar.id) {
            R.id.scaleSeekBar -> {
                updateScale(progress)
            }
            R.id.paddingSeekBar -> {
                updatePadding(progress)
            }
            R.id.boxScoreThreshSeekBar -> {
                updateBoxScoreThresh(progress)
            }
            R.id.boxThreshSeekBar -> {
                updateBoxThresh(progress)
            }
            R.id.minAreaSeekBar -> {
                updateMinArea(progress)
            }
            R.id.scaleWidthSeekBar -> {
                updateScaleWidth(progress)
            }
            R.id.scaleHeightSeekBar -> {
                updateScaleHeight(progress)
            }
            else -> {
            }
        }
    }

    override fun onStartTrackingTouch(seekBar: SeekBar?) {
    }

    override fun onStopTrackingTouch(seekBar: SeekBar?) {
    }

    private fun updateScale(progress: Int) {
        val scale = progress.toFloat() / 100.toFloat()
        if (selectedImg != null) {
            val img = selectedImg ?: return
            val maxSize = max(img.width, img.height)
            val reSize = (scale * maxSize).toInt()
            scaleTv.text = "Size:$reSize(${scale * 100}%)"
        } else {
            scaleTv.text = "Size:0(${scale * 100}%)"
        }
    }

    private fun updatePadding(progress: Int) {
        paddingTv.text = "Padding:$progress"
        ocrEngine.padding = progress
    }

    private fun updateBoxScoreThresh(progress: Int) {
        val thresh = progress.toFloat() / 100.toFloat()
        boxScoreThreshTv.text = "${getString(R.string.box_score_thresh)}:$thresh"
        ocrEngine.boxScoreThresh = thresh
    }

    private fun updateBoxThresh(progress: Int) {
        val thresh = progress.toFloat() / 100.toFloat()
        boxThreshTv.text = "BoxThresh:$thresh"
        ocrEngine.boxThresh = thresh
    }

    private fun updateMinArea(progress: Int) {
        minAreaTv.text = "${getString(R.string.min_area)}:$progress"
        ocrEngine.miniArea = progress.toFloat()
    }

    private fun updateScaleWidth(progress: Int) {
        val scale = progress.toFloat() / 10.toFloat()
        scaleWidthTv.text = "${getString(R.string.box_scale_width)}:$scale"
        ocrEngine.scaleWidth = scale
    }

    private fun updateScaleHeight(progress: Int) {
        val scale = progress.toFloat() / 10.toFloat()
        scaleHeightTv.text = "${getString(R.string.box_scale_height)}:$scale"
        ocrEngine.scaleHeight = scale
    }


    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)
        data ?: return
        if (resultCode == Activity.RESULT_OK && requestCode == REQUEST_SELECT_IMAGE) {
            val imgUri = data.data ?: return
            val options =
                RequestOptions().skipMemoryCache(true).diskCacheStrategy(DiskCacheStrategy.NONE)
            Glide.with(this).load(imgUri).apply(options).into(imageView)
            selectedImg = decodeUri(imgUri)
            updateScale(scaleSeekBar.progress)
            clearLastResult()
        }
    }

    private fun showLoading() {
        Glide.with(this).load(R.drawable.loading_anim).into(imageView)
    }

    private fun clearLastResult() {
        timeTV.text = ""
        ocrResult = null
    }

    private fun detect(img: Bitmap, reSize: Int) {
        Single.fromCallable {
            val boxImg: Bitmap = Bitmap.createBitmap(
                img.width, img.height, Bitmap.Config.ARGB_8888
            )
            Log.i(TAG, "selectedImg=${img.height},${img.width} ${img.config}")
            val start = System.currentTimeMillis()
            val ocrResult = ocrEngine.detect(img, boxImg, reSize)
            val end = System.currentTimeMillis()
            val time = "time=${end - start}ms"
            ocrResult
        }.subscribeOn(Schedulers.io())
            .observeOn(AndroidSchedulers.mainThread())
            .doOnSubscribe { showLoading() }
            .doFinally { /*hideLoading()*/ }
            .autoDisposable(this)
            .subscribe { t1, t2 ->
                ocrResult = t1
                timeTV.text = "识别时间:${t1.detectTime.toInt()}ms"
                val options =
                    RequestOptions().skipMemoryCache(true).diskCacheStrategy(DiskCacheStrategy.NONE)
                Glide.with(this).load(t1.boxImg).apply(options).into(imageView)
                Log.i(TAG, "$t1")
            }
    }

    companion object {
        const val REQUEST_SELECT_IMAGE = 666
        const val TAG = "OcrLite"
    }


}