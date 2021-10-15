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
class DebugItemView @JvmOverloads constructor(
    context: Context,
    attrs: AttributeSet? = null,
    defStyleAttr: Int = 0
) : LinearLayout(context, attrs, defStyleAttr) {

    private lateinit var blockIndexTv: TextView
    private lateinit var boxPointTv: TextView
    private lateinit var boxScoreTv: TextView
    private lateinit var angleIndexTv: TextView
    private lateinit var angleScoreTv: TextView
    private lateinit var angleTimeTv: TextView
    private lateinit var textTv: TextView
    private lateinit var charScoresTv: TextView
    private lateinit var crnnTimeTv: TextView
    private lateinit var blockTimeTv: TextView

    init {
        inflate(context, R.layout.rv_debug_view_item, this)
        blockIndexTv = findViewById(R.id.blockIndexTv)
        boxPointTv = findViewById(R.id.boxPointTv)
        boxScoreTv = findViewById(R.id.boxScoreTv)
        angleIndexTv = findViewById(R.id.angleIndexTv)
        angleScoreTv = findViewById(R.id.angleScoreTv)
        angleTimeTv = findViewById(R.id.angleTimeTv)
        textTv = findViewById(R.id.textTv)
        charScoresTv = findViewById(R.id.charScoresTv)
        crnnTimeTv = findViewById(R.id.crnnTimeTv)
        blockTimeTv = findViewById(R.id.blockTimeTv)
    }

    // 2. Or you can use lateinit
    @TextProp
    lateinit var index: CharSequence

    @TextProp
    lateinit var boxPoint: CharSequence

    @TextProp
    lateinit var boxScore: CharSequence

    @TextProp
    lateinit var angleIndex: CharSequence

    @TextProp
    lateinit var angleScore: CharSequence

    @TextProp
    lateinit var angleTime: CharSequence

    @TextProp
    lateinit var text: CharSequence

    @TextProp
    lateinit var charScores: CharSequence

    @TextProp
    lateinit var crnnTime: CharSequence

    @TextProp
    lateinit var blockTime: CharSequence

    @AfterPropsSet
    fun useProps() {
        blockIndexTv.text = index
        boxPointTv.text = boxPoint
        boxScoreTv.text = boxScore
        angleIndexTv.text = angleIndex
        angleScoreTv.text = angleScore
        angleTimeTv.text = angleTime
        textTv.text = text
        charScoresTv.text = charScores
        crnnTimeTv.text = crnnTime
        blockTimeTv.text = blockTime
    }
}
