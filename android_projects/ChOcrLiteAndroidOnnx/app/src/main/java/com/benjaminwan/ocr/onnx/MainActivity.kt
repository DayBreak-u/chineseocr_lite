package com.benjaminwan.ocr.onnx

import android.content.Intent
import android.os.Bundle
import android.view.View
import androidx.appcompat.app.AppCompatActivity
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity(), View.OnClickListener {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        galleryBtn.setOnClickListener(this)
        cameraBtn.setOnClickListener(this)
        imeiBtn.setOnClickListener(this)
        plateBtn.setOnClickListener(this)
        idCardBtn.setOnClickListener(this)
    }

    override fun onClick(view: View?) {
        view ?: return
        when (view.id) {
            R.id.galleryBtn -> {
                startActivity(Intent(this, GalleryActivity::class.java))
            }
            R.id.cameraBtn -> {
                startActivity(Intent(this, CameraActivity::class.java))
            }
            R.id.imeiBtn -> {
                startActivity(Intent(this, ImeiActivity::class.java))
            }
            R.id.plateBtn -> {
                startActivity(Intent(this, PlateActivity::class.java))
            }
            R.id.idCardBtn -> {
                startActivity(Intent(this, IdCardFrontActivity::class.java))
            }
            else -> {
            }
        }
    }
}