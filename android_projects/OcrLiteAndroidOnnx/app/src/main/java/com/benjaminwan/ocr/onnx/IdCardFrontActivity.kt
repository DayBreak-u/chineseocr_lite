package com.benjaminwan.ocr.onnx

import android.app.Service
import android.content.Intent
import android.graphics.Bitmap
import android.os.Bundle
import android.os.VibrationEffect
import android.os.Vibrator
import android.view.View
import android.widget.EditText
import androidx.appcompat.app.AppCompatActivity
import androidx.camera.core.*
import androidx.camera.lifecycle.ProcessCameraProvider
import androidx.camera.view.PreviewView
import androidx.core.content.ContextCompat
import com.afollestad.assent.Permission
import com.afollestad.assent.askForPermissions
import com.afollestad.assent.isAllGranted
import com.afollestad.assent.rationale.createDialogRationale
import com.benjaminwan.ocr.onnx.app.App
import com.benjaminwan.ocr.onnx.models.IdCardFront
import com.benjaminwan.ocr.onnx.utils.getMatchIdCardNumberStr
import com.benjaminwan.ocr.onnx.utils.showToast
import com.benjaminwan.ocr.onnx.utils.trimBlankAndSymbols
import com.benjaminwan.ocrlibrary.OcrFailed
import com.benjaminwan.ocrlibrary.OcrResult
import com.benjaminwan.ocrlibrary.OcrStop
import com.orhanobut.logger.Logger
import com.uber.autodispose.android.lifecycle.autoDisposable
import io.reactivex.Observable
import io.reactivex.android.schedulers.AndroidSchedulers
import io.reactivex.schedulers.Schedulers
import kotlinx.android.synthetic.main.activity_idcard_front.*
import kotlin.math.max

class IdCardFrontActivity : AppCompatActivity(), View.OnClickListener {

    private var preview: Preview? = null
    private var imageCapture: ImageCapture? = null
    private var camera: Camera? = null
    private lateinit var viewFinder: PreviewView
    private var detectStart: Boolean = false

    private val vibrator: Vibrator by lazy {
        applicationContext.getSystemService(Service.VIBRATOR_SERVICE) as Vibrator
    }

    //手机振动
    private fun vibration() {
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.O) {
            val vibrationEffect = VibrationEffect.createOneShot(
                100, VibrationEffect.DEFAULT_AMPLITUDE
            )
            vibrator.vibrate(vibrationEffect)
        } else {
            vibrator.vibrate(100)
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        App.ocrEngine.doAngle = false//摄像头拍摄一般都是正的，不需要判断方向
        App.ocrEngine.padding = 200
        App.ocrEngine.boxScoreThresh = 0.1f
        App.ocrEngine.unClipRatio = 2.0f
        setContentView(R.layout.activity_idcard_front)
        viewFinder = findViewById(R.id.viewFinder)
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
                    viewFinder.postDelayed({ detectLoop() }, 100)
                }
            }
        } else {
            startCamera()
            viewFinder.postDelayed({ detectLoop() }, 100)
        }
    }

    override fun onStop() {
        super.onStop()
        setDetectState(false)
    }

    private fun setResult() {
        val text = numberEdit.text.toString().trim()
        if (text.isNotEmpty()) {
            val result = Intent().apply {
                putExtra("scanResult", text)
            }
            setResult(1, result)
        }
    }

    override fun onBackPressed() {
        setResult()
        super.onBackPressed()
    }

    private fun setEdit(editText: EditText, content: String) {
        editText.setText(content)
        editText.setSelection(content.length)
    }

    private fun showResult(result: IdCardFront) {
        setEdit(nameEdit, result.name)
        setEdit(genderEdit, result.gender)
        setEdit(nationEdit, result.nation)
        setEdit(birthDateEdit, result.birth)
        setEdit(addressEdit, result.address)
        setEdit(numberEdit, result.number)
    }

    private fun initViews() {
        startBtn.setOnClickListener(this)
        stopBtn.setOnClickListener(this)
        clearNameBtn.setOnClickListener(this)
        clearGenderBtn.setOnClickListener(this)
        clearNationBtn.setOnClickListener(this)
        clearBirthBtn.setOnClickListener(this)
        clearAddressBtn.setOnClickListener(this)
        clearNumberBtn.setOnClickListener(this)
    }

    override fun onClick(view: View?) {
        view ?: return
        when (view.id) {
            R.id.startBtn -> {
                detectLoop()
            }
            R.id.stopBtn -> {
                detectStart = false
            }
            R.id.clearNameBtn -> {
                nameEdit.setText("")
            }
            R.id.clearGenderBtn -> {
                genderEdit.setText("")
            }
            R.id.clearNationBtn -> {
                nationEdit.setText("")
            }
            R.id.clearBirthBtn -> {
                birthDateEdit.setText("")
            }
            R.id.clearAddressBtn -> {
                addressEdit.setText("")
            }
            R.id.clearNumberBtn -> {
                numberEdit.setText("")
            }
        }
    }

    private fun detectOnce(bitmap: Bitmap): OcrResult {
        val maxSize = max(bitmap.height, bitmap.width)
        val boxImg = Bitmap.createBitmap(bitmap.width, bitmap.height, Bitmap.Config.ARGB_8888)
        App.ocrEngine.padding = maxSize / 10
        return App.ocrEngine.detect(bitmap, boxImg, maxSize)
    }

    private fun setDetectState(isStart: Boolean) {
        detectStart = isStart
        startBtn.isEnabled = !isStart
        stopBtn.isEnabled = isStart
        nameEdit.isEnabled = !isStart
        genderEdit.isEnabled = !isStart
        nationEdit.isEnabled = !isStart
        birthDateEdit.isEnabled = !isStart
        addressEdit.isEnabled = !isStart
        numberEdit.isEnabled = !isStart
        clearNameBtn.isEnabled = !isStart
        clearGenderBtn.isEnabled = !isStart
        clearNationBtn.isEnabled = !isStart
        clearBirthBtn.isEnabled = !isStart
        clearAddressBtn.isEnabled = !isStart
        clearNumberBtn.isEnabled = !isStart
        if (isStart) idCardFrontView.bitmap = null
    }

    private fun detectLoop() {
        setDetectState(true)
        Observable.fromCallable {
            var success: IdCardFront? = null
            var numberStr: String? = null
            var nameStr: String? = null
            var nationStr: String? = null
            var addressStr: String? = null
            val start = System.currentTimeMillis()
            do {
                val cameraBitmap = viewFinder.bitmap ?: continue
                if (cameraBitmap.width <= 0 || cameraBitmap.height <= 0) continue
                //先识别身份证号
                if (numberStr == null) {
                    val numberBitmap = idCardFrontView.cropNumberBitmap(cameraBitmap)
                    val numberOnce = detectOnce(numberBitmap)
                    Logger.i(numberOnce.strRes)
                    numberStr =
                        getMatchIdCardNumberStr(numberOnce.strRes.trimBlankAndSymbols()) ?: continue
                }
                //识别姓名
                if (nameStr == null) {
                    val nameBitmap = idCardFrontView.cropNameBitmap(cameraBitmap)
                    val nameOnce = detectOnce(nameBitmap)
                    Logger.i(nameOnce.toString())
                    val nameLine = nameOnce.textBlocks.sortedBy { it.boxPoint.first().x }
                        .joinToString("") { it.text.trimBlankAndSymbols() }
                    if (nameLine.isEmpty()) continue
                    nameStr = if (nameLine.contains("姓名") && nameLine.length > 2) {
                        nameLine.substringAfter("姓名")
                    } else if (nameLine.contains("名") && nameLine.length > 1 && nameLine.indexOf("名") <= 1) {
                        nameLine.substringAfter("名")
                    } else {
                        null
                    }
                    nameStr ?: continue
                }
                //民族
                if (nationStr == null) {
                    val nationBitmap = idCardFrontView.cropNationBitmap(cameraBitmap)
                    val nationOnce = detectOnce(nationBitmap)
                    Logger.i(nationOnce.toString())
                    val nationLine = nationOnce.textBlocks.sortedBy { it.boxPoint.first().x }
                        .joinToString("") { it.text.trimBlankAndSymbols() }
                    if (nationLine.isEmpty()) continue
                    nationStr = if (nationLine.contains("民族") && nationLine.length > 2) {
                        nationLine.substringAfter("民族")
                    } else if (nationLine.contains("族") &&
                        nationLine.length > 1 && nationLine.indexOf("族") <= 1
                    ) {
                        nationLine.substringAfter("族")
                    } else {
                        null
                    }
                    nationStr ?: continue
                }

                //住址
                if (addressStr == null) {
                    val addressBitmap = idCardFrontView.cropAddressBitmap(cameraBitmap)
                    val addressOnce = detectOnce(addressBitmap)
                    Logger.i(addressOnce.toString())
                    val addressLine = addressOnce.textBlocks.sortedBy { it.boxPoint.first().y }
                        .joinToString("") { it.text.trimBlankAndSymbols() }
                    if (addressLine.isEmpty()) continue
                    addressStr = if (addressLine.contains("住址") && addressLine.length > 2) {
                        addressLine.substringAfter("住址")
                    } else if (addressLine.contains("址") &&
                        addressLine.length > 1 && addressLine.indexOf("址") <= 1
                    ) {
                        addressLine.substringAfter("址")
                    } else {
                        addressLine
                    }
                }

                success = IdCardFront(
                    nameStr,
                    nationStr,
                    addressStr,
                    numberStr
                )
            } while (success == null && detectStart)
            val end = System.currentTimeMillis()
            Logger.i("time=${end - start}")
            success ?: if (!detectStart) {
                OcrStop
            } else {
                OcrFailed
            }
        }.subscribeOn(Schedulers.io())
            .observeOn(AndroidSchedulers.mainThread())
            .autoDisposable(this)
            .subscribe({
                when (it) {
                    is IdCardFront -> {
                        setDetectState(false)
                        vibration()
                        showResult(it)
                    }
                    is OcrStop -> {
                        setDetectState(false)
                    }
                    is OcrFailed -> {
                        detectLoop()
                    }
                }
            }, {
                detectLoop()
            })
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
                Logger.e("Use case binding failed", exc)
            }

        }, ContextCompat.getMainExecutor(this))
    }

}