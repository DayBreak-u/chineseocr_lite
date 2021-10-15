package com.benjaminwan.ocr.onnx

import android.content.Intent
import android.os.Bundle
import android.view.View
import android.widget.Button
import androidx.appcompat.app.AppCompatActivity

class MainActivity : AppCompatActivity(), View.OnClickListener {

    private lateinit var galleryBtn: Button
    private lateinit var cameraBtn: Button
    private lateinit var imeiBtn: Button
    private lateinit var plateBtn: Button
    private lateinit var idCardBtn: Button

    private fun initViews() {
        galleryBtn = findViewById(R.id.galleryBtn)
        cameraBtn = findViewById(R.id.cameraBtn)
        imeiBtn = findViewById(R.id.imeiBtn)
        plateBtn = findViewById(R.id.plateBtn)
        idCardBtn = findViewById(R.id.idCardBtn)
        galleryBtn.setOnClickListener(this)
        cameraBtn.setOnClickListener(this)
        imeiBtn.setOnClickListener(this)
        plateBtn.setOnClickListener(this)
        idCardBtn.setOnClickListener(this)
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        initViews()
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