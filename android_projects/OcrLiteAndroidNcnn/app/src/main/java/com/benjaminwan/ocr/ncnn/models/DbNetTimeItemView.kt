package com.benjaminwan.ocr.ncnn.models

import android.content.Context
import android.util.AttributeSet
import android.widget.LinearLayout
import android.widget.TextView
import com.airbnb.epoxy.AfterPropsSet
import com.airbnb.epoxy.ModelView
import com.airbnb.epoxy.TextProp
import com.benjaminwan.ocr.ncnn.R

// The ModelView annotation is used on Views to have models generated from those views.
// This is pretty straightforward with Kotlin, but properties need some special handling.
@ModelView(autoLayout = ModelView.Size.MATCH_WIDTH_WRAP_HEIGHT)
class DbNetTimeItemView @JvmOverloads constructor(
    context: Context,
    attrs: AttributeSet? = null,
    defStyleAttr: Int = 0
) : LinearLayout(context, attrs, defStyleAttr) {
    private lateinit var dbNetTimeTv: TextView

    init {
        inflate(context, R.layout.rv_dbnet_time_item, this)
        dbNetTimeTv = findViewById(R.id.dbNetTimeTv)
    }

    // 2. Or you can use lateinit
    @TextProp
    lateinit var dbNetTimeStr: CharSequence

    @AfterPropsSet
    fun useProps() {
        dbNetTimeTv.text = dbNetTimeStr
    }
}
