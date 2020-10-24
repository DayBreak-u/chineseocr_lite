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
    }

    override fun onClick(view: View?) {
        view ?: return
        when (view.id) {
            R.id.galleryBtn -> {
                val intent = Intent(this, GalleryActivity::class.java)
                startActivity(intent)
            }
            R.id.cameraBtn -> {
                val intent = Intent(this, CameraActivity::class.java)
                startActivity(intent)
            }
            else -> {
            }
        }
    }
}