package com.benjaminwan.ocr.ncnn

import android.app.Activity
import android.content.Intent
import android.graphics.Bitmap
import android.os.Bundle
import android.view.View
import android.widget.Button
import android.widget.ImageView
import android.widget.SeekBar
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import androidx.appcompat.widget.SwitchCompat
import androidx.lifecycle.lifecycleScope
import com.afollestad.assent.Permission
import com.afollestad.assent.askForPermissions
import com.afollestad.assent.isAllGranted
import com.afollestad.assent.rationale.createDialogRationale
import com.benjaminwan.ocr.ncnn.app.App
import com.benjaminwan.ocr.ncnn.dialog.DebugDialog
import com.benjaminwan.ocr.ncnn.dialog.TextResultDialog
import com.benjaminwan.ocr.ncnn.utils.decodeUri
import com.benjaminwan.ocr.ncnn.utils.showToast
import com.benjaminwan.ocrlibrary.OcrResult
import com.bumptech.glide.Glide
import com.bumptech.glide.load.engine.DiskCacheStrategy
import com.bumptech.glide.request.RequestOptions
import com.orhanobut.logger.Logger
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.flow.*
import kotlin.math.max

class GalleryActivity : AppCompatActivity(), View.OnClickListener, SeekBar.OnSeekBarChangeListener {

    private var selectedImg: Bitmap? = null
    private var ocrResult: OcrResult? = null

    private lateinit var selectBtn: Button
    private lateinit var detectBtn: Button
    private lateinit var resultBtn: Button
    private lateinit var debugBtn: Button
    private lateinit var benchBtn: Button
    private lateinit var doAngleSw: SwitchCompat
    private lateinit var mostAngleSw: SwitchCompat
    private lateinit var paddingSeekBar: SeekBar
    private lateinit var boxScoreThreshSeekBar: SeekBar
    private lateinit var boxThreshSeekBar: SeekBar
    private lateinit var maxSideLenSeekBar: SeekBar
    private lateinit var scaleUnClipRatioSeekBar: SeekBar
    private lateinit var maxSideLenTv: TextView
    private lateinit var paddingTv: TextView
    private lateinit var boxScoreThreshTv: TextView
    private lateinit var boxThreshTv: TextView
    private lateinit var unClipRatioTv: TextView
    private lateinit var timeTV: TextView
    private lateinit var imageView: ImageView

    private fun findViews() {
        selectBtn = findViewById(R.id.selectBtn)
        detectBtn = findViewById(R.id.detectBtn)
        resultBtn = findViewById(R.id.resultBtn)
        debugBtn = findViewById(R.id.debugBtn)
        benchBtn = findViewById(R.id.benchBtn)
        doAngleSw = findViewById(R.id.doAngleSw)
        mostAngleSw = findViewById(R.id.mostAngleSw)
        paddingSeekBar = findViewById(R.id.paddingSeekBar)
        boxScoreThreshSeekBar = findViewById(R.id.boxScoreThreshSeekBar)
        boxThreshSeekBar = findViewById(R.id.boxThreshSeekBar)
        maxSideLenSeekBar = findViewById(R.id.maxSideLenSeekBar)
        scaleUnClipRatioSeekBar = findViewById(R.id.scaleUnClipRatioSeekBar)
        maxSideLenTv = findViewById(R.id.maxSideLenTv)
        paddingTv = findViewById(R.id.paddingTv)
        boxScoreThreshTv = findViewById(R.id.boxScoreThreshTv)
        boxThreshTv = findViewById(R.id.boxThreshTv)
        unClipRatioTv = findViewById(R.id.unClipRatioTv)
        timeTV = findViewById(R.id.timeTV)
        imageView = findViewById(R.id.imageView)
    }

    private fun initViews() {
        selectBtn.setOnClickListener(this)
        detectBtn.setOnClickListener(this)
        resultBtn.setOnClickListener(this)
        debugBtn.setOnClickListener(this)
        benchBtn.setOnClickListener(this)
        doAngleSw.isChecked = App.ocrEngine.doAngle
        mostAngleSw.isChecked = App.ocrEngine.mostAngle
        updatePadding(App.ocrEngine.padding)
        updateBoxScoreThresh((App.ocrEngine.boxScoreThresh * 100).toInt())
        updateBoxThresh((App.ocrEngine.boxThresh * 100).toInt())
        updateUnClipRatio((App.ocrEngine.unClipRatio * 10).toInt())
        paddingSeekBar.setOnSeekBarChangeListener(this)
        boxScoreThreshSeekBar.setOnSeekBarChangeListener(this)
        boxThreshSeekBar.setOnSeekBarChangeListener(this)
        maxSideLenSeekBar.setOnSeekBarChangeListener(this)
        scaleUnClipRatioSeekBar.setOnSeekBarChangeListener(this)
        doAngleSw.setOnCheckedChangeListener { _, isChecked ->
            App.ocrEngine.doAngle = isChecked
            mostAngleSw.isEnabled = isChecked
        }
        mostAngleSw.setOnCheckedChangeListener { _, isChecked ->
            App.ocrEngine.mostAngle = isChecked
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        App.ocrEngine.doAngle = true//相册识别时，默认启用文字方向检测
        setContentView(R.layout.activity_gallery)
        findViews()
        initViews()
    }

    private fun getPermissions() {
        val rationaleHandler = createDialogRationale(R.string.storage_permission) {
            onPermission(
                Permission.READ_EXTERNAL_STORAGE, "请点击允许"
            )
        }

        if (!isAllGranted(Permission.READ_EXTERNAL_STORAGE)) {
            askForPermissions(
                Permission.READ_EXTERNAL_STORAGE,
                rationaleHandler = rationaleHandler
            ) { result ->
                val permissionGranted: Boolean =
                    result.isAllGranted(
                        Permission.READ_EXTERNAL_STORAGE
                    )
                if (!permissionGranted) {
                    showToast("未获取权限，应用无法正常使用！")
                }
            }
        }
    }

    override fun onResume() {
        super.onResume()
        getPermissions()
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
                val ratio = maxSideLenSeekBar.progress.toFloat() / 100.toFloat()
                val maxSize = max(img.width, img.height)
                val maxSideLen = (ratio * maxSize).toInt()
                detect(img, maxSideLen)
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
            R.id.benchBtn -> {
                val img = selectedImg
                if (img == null) {
                    showToast("请先选择一张图片")
                    return
                }
                val loop = 100
                showToast("开始循环${loop}次的测试")
                benchmark(img, loop)
            }
            else -> {
            }
        }
    }

    override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
        seekBar ?: return
        when (seekBar.id) {
            R.id.maxSideLenSeekBar -> {
                updateMaxSideLen(progress)
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
            R.id.scaleUnClipRatioSeekBar -> {
                updateUnClipRatio(progress)
            }
            else -> {
            }
        }
    }

    override fun onStartTrackingTouch(seekBar: SeekBar?) {
    }

    override fun onStopTrackingTouch(seekBar: SeekBar?) {
    }

    private fun updateMaxSideLen(progress: Int) {
        val ratio = progress.toFloat() / 100.toFloat()
        if (selectedImg != null) {
            val img = selectedImg ?: return
            val maxSize = max(img.width, img.height)
            val maxSizeLen = (ratio * maxSize).toInt()
            maxSideLenTv.text = "MaxSideLen:$maxSizeLen(${ratio * 100}%)"
        } else {
            maxSideLenTv.text = "MaxSideLen:0(${ratio * 100}%)"
        }
    }

    private fun updatePadding(progress: Int) {
        paddingTv.text = "Padding:$progress"
        App.ocrEngine.padding = progress
    }

    private fun updateBoxScoreThresh(progress: Int) {
        val thresh = progress.toFloat() / 100.toFloat()
        boxScoreThreshTv.text = "${getString(R.string.box_score_thresh)}:$thresh"
        App.ocrEngine.boxScoreThresh = thresh
    }

    private fun updateBoxThresh(progress: Int) {
        val thresh = progress.toFloat() / 100.toFloat()
        boxThreshTv.text = "BoxThresh:$thresh"
        App.ocrEngine.boxThresh = thresh
    }

    private fun updateUnClipRatio(progress: Int) {
        val scale = progress.toFloat() / 10.toFloat()
        unClipRatioTv.text = "${getString(R.string.box_un_clip_ratio)}:$scale"
        App.ocrEngine.unClipRatio = scale
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
            updateMaxSideLen(maxSideLenSeekBar.progress)
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

    private fun benchmark(img: Bitmap, loop: Int) {
        flow {
            val aveTime = App.ocrEngine.benchmark(img, loop)
            //showToast("循环${loop}次，平均时间${aveTime}ms")
            emit(aveTime)
        }.flowOn(Dispatchers.IO)
            .onStart { showLoading() }
            .onEach {
                timeTV.text = "循环${loop}次，平均时间${it}ms"
                Glide.with(this).clear(imageView)
            }
            .launchIn(lifecycleScope)
    }

    private fun detect(img: Bitmap, reSize: Int) {
        flow {
            val boxImg: Bitmap = Bitmap.createBitmap(
                img.width, img.height, Bitmap.Config.ARGB_8888
            )
            Logger.i("selectedImg=${img.height},${img.width} ${img.config}")
            val start = System.currentTimeMillis()
            val ocrResult = App.ocrEngine.detect(img, boxImg, reSize)
            val end = System.currentTimeMillis()
            val time = "time=${end - start}ms"
            emit(ocrResult)
        }.flowOn(Dispatchers.IO)
            .onStart { showLoading() }
            .onEach {
                ocrResult = it
                timeTV.text = "识别时间:${it.detectTime.toInt()}ms"
                val options =
                    RequestOptions().skipMemoryCache(true).diskCacheStrategy(DiskCacheStrategy.NONE)
                Glide.with(this).load(it.boxImg).apply(options).into(imageView)
                Logger.i("$it")
            }.launchIn(lifecycleScope)
    }

    companion object {
        const val REQUEST_SELECT_IMAGE = 666
    }


}