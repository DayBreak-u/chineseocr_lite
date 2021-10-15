package com.benjaminwan.ocr.ncnn

import android.graphics.Bitmap
import android.os.Bundle
import android.view.View
import android.widget.Button
import android.widget.ImageView
import android.widget.SeekBar
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import androidx.camera.core.*
import androidx.camera.lifecycle.ProcessCameraProvider
import androidx.camera.view.PreviewView
import androidx.core.content.ContextCompat
import androidx.lifecycle.lifecycleScope
import com.afollestad.assent.Permission
import com.afollestad.assent.askForPermissions
import com.afollestad.assent.isAllGranted
import com.afollestad.assent.rationale.createDialogRationale
import com.benjaminwan.ocr.ncnn.app.App
import com.benjaminwan.ocr.ncnn.dialog.DebugDialog
import com.benjaminwan.ocr.ncnn.dialog.TextResultDialog
import com.benjaminwan.ocr.ncnn.utils.showToast
import com.benjaminwan.ocrlibrary.OcrResult
import com.bumptech.glide.Glide
import com.orhanobut.logger.Logger
import jsc.kit.cameramask.CameraLensView
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.flow.*
import kotlin.math.max

class CameraActivity : AppCompatActivity(), View.OnClickListener, SeekBar.OnSeekBarChangeListener {

    private var ocrResult: OcrResult? = null

    private var preview: Preview? = null
    private var imageCapture: ImageCapture? = null
    private var camera: Camera? = null
    private lateinit var viewFinder: PreviewView

    private lateinit var clearBtn: Button
    private lateinit var detectBtn: Button
    private lateinit var resultBtn: Button
    private lateinit var debugBtn: Button
    private lateinit var paddingSeekBar: SeekBar
    private lateinit var boxScoreThreshSeekBar: SeekBar
    private lateinit var boxThreshSeekBar: SeekBar
    private lateinit var maxSideLenSeekBar: SeekBar
    private lateinit var scaleUnClipRatioSeekBar: SeekBar
    private lateinit var cameraLensView: CameraLensView
    private lateinit var maxSideLenTv: TextView
    private lateinit var paddingTv: TextView
    private lateinit var boxScoreThreshTv: TextView
    private lateinit var boxThreshTv: TextView
    private lateinit var unClipRatioTv: TextView
    private lateinit var timeTV: TextView
    private lateinit var loadingImg: ImageView

    private fun findViews() {
        clearBtn = findViewById(R.id.clearBtn)
        detectBtn = findViewById(R.id.detectBtn)
        resultBtn = findViewById(R.id.resultBtn)
        debugBtn = findViewById(R.id.debugBtn)
        paddingSeekBar = findViewById(R.id.paddingSeekBar)
        boxScoreThreshSeekBar = findViewById(R.id.boxScoreThreshSeekBar)
        boxThreshSeekBar = findViewById(R.id.boxThreshSeekBar)
        maxSideLenSeekBar = findViewById(R.id.maxSideLenSeekBar)
        scaleUnClipRatioSeekBar = findViewById(R.id.scaleUnClipRatioSeekBar)
        cameraLensView = findViewById(R.id.cameraLensView)
        maxSideLenTv = findViewById(R.id.maxSideLenTv)
        paddingTv = findViewById(R.id.paddingTv)
        boxScoreThreshTv = findViewById(R.id.boxScoreThreshTv)
        boxThreshTv = findViewById(R.id.boxThreshTv)
        unClipRatioTv = findViewById(R.id.unClipRatioTv)
        timeTV = findViewById(R.id.timeTV)
        loadingImg = findViewById(R.id.loadingImg)
    }

    private fun initViews() {
        clearBtn.setOnClickListener(this)
        detectBtn.setOnClickListener(this)
        resultBtn.setOnClickListener(this)
        debugBtn.setOnClickListener(this)
        updatePadding(App.ocrEngine.padding)
        updateBoxScoreThresh((App.ocrEngine.boxScoreThresh * 100).toInt())
        updateBoxThresh((App.ocrEngine.boxThresh * 100).toInt())
        updateUnClipRatio((App.ocrEngine.unClipRatio * 10).toInt())
        paddingSeekBar.setOnSeekBarChangeListener(this)
        boxScoreThreshSeekBar.setOnSeekBarChangeListener(this)
        boxThreshSeekBar.setOnSeekBarChangeListener(this)
        maxSideLenSeekBar.setOnSeekBarChangeListener(this)
        scaleUnClipRatioSeekBar.setOnSeekBarChangeListener(this)
        viewFinder = findViewById(R.id.viewFinder)
        cameraLensView.postDelayed({
            updateMaxSideLen(100)
        }, 500)
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        App.ocrEngine.doAngle = false//摄像头一般不需要考虑倒过来的情况
        setContentView(R.layout.activity_camera)
        findViews()
        initViews()
    }

    override fun onResume() {
        super.onResume()
        val rationaleHandler = createDialogRationale(R.string.app_permission) {
            onPermission(
                Permission.CAMERA, "请点击允许"
            )
        }

        if (!isAllGranted(Permission.CAMERA)) {
            askForPermissions(
                Permission.CAMERA,
                rationaleHandler = rationaleHandler
            ) { result ->
                val permissionGranted: Boolean =
                    result.isAllGranted(
                        Permission.CAMERA
                    )
                if (!permissionGranted) {
                    showToast("未获取权限，应用无法正常使用！")
                } else {
                    startCamera()
                }
            }
        } else {
            startCamera()
        }
    }

    override fun onClick(view: View?) {
        view ?: return
        when (view.id) {
            R.id.clearBtn -> {
                clearLastResult()
            }
            R.id.detectBtn -> {
                val width = cameraLensView.measuredWidth * 9 / 10
                val height = cameraLensView.measuredHeight * 9 / 10
                val ratio = maxSideLenSeekBar.progress.toFloat() / 100.toFloat()
                val maxSize = max(width, height)
                val maxSideLen = (ratio * maxSize).toInt()
                detect(maxSideLen)
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
        val width = cameraLensView.measuredWidth * 9 / 10
        val height = cameraLensView.measuredHeight * 9 / 10
        val ratio = progress.toFloat() / 100.toFloat()
        val maxSize = max(width, height)
        val maxSideLen = (ratio * maxSize).toInt()
        Logger.i("======$width,$height,$ratio,$maxSize,$maxSideLen")
        maxSideLenTv.text = "MaxSideLen:$maxSideLen(${ratio * 100}%)"
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

    private fun showLoading() {
        loadingImg.visibility = View.VISIBLE
        Glide.with(this).load(R.drawable.loading_anim).into(loadingImg)
    }

    private fun hideLoading() {
        loadingImg.visibility = View.GONE
    }

    private fun clearLastResult() {
        cameraLensView.cameraLensBitmap = null
        timeTV.text = ""
        ocrResult = null
    }

    private fun detect(reSize: Int) {
        flow {
            emit(cameraLensView.cropCameraLensRectBitmap(viewFinder.bitmap, false))
        }.flowOn(Dispatchers.Main)
            .map { src ->
                val boxImg: Bitmap = Bitmap.createBitmap(
                    src.width, src.height, Bitmap.Config.ARGB_8888
                )
                Logger.i("selectedImg=${src.height},${src.width} ${src.config}")
                App.ocrEngine.detect(src, boxImg, reSize)
            }
            .flowOn(Dispatchers.IO)
            .onStart { showLoading() }
            .onCompletion { hideLoading() }
            .onEach {
                ocrResult = it
                timeTV.text = "识别时间:${it.detectTime.toInt()}ms"
                cameraLensView.cameraLensBitmap = it.boxImg
            }
            .launchIn(lifecycleScope)
    }

    private fun startCamera() {
        val cameraProviderFuture = ProcessCameraProvider.getInstance(this)

        cameraProviderFuture.addListener(Runnable {
            // Used to bind the lifecycle of cameras to the lifecycle owner
            val cameraProvider: ProcessCameraProvider = cameraProviderFuture.get()

            // Preview
            preview = Preview.Builder()
                .setTargetAspectRatio(AspectRatio.RATIO_4_3)
                .build()

            val imageCaptureBuilder = ImageCapture.Builder()
            imageCaptureBuilder.setTargetAspectRatio(AspectRatio.RATIO_4_3)
            imageCapture = imageCaptureBuilder.build()

            // Select back camera
            val cameraSelector =
                CameraSelector.Builder().requireLensFacing(CameraSelector.LENS_FACING_BACK).build()

            try {
                // Unbind use cases before rebinding
                cameraProvider.unbindAll()

                // Bind use cases to camera
                camera = cameraProvider.bindToLifecycle(this, cameraSelector, preview, imageCapture)
                preview?.setSurfaceProvider(viewFinder.surfaceProvider)
            } catch (exc: Exception) {
                Logger.e("Use case binding failed", exc.message.toString())
            }

        }, ContextCompat.getMainExecutor(this))
    }

    companion object {
        const val REQUEST_SELECT_IMAGE = 666
    }


}