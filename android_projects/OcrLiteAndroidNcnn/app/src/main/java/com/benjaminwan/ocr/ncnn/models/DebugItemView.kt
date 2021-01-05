package com.benjaminwan.ocr.ncnn.models

import android.content.Context
import android.util.AttributeSet
import android.widget.LinearLayout
import com.airbnb.epoxy.AfterPropsSet
import com.airbnb.epoxy.ModelView
import com.airbnb.epoxy.TextProp
import com.benjaminwan.ocr.ncnn.R
import kotlinx.android.synthetic.main.rv_debug_item_content.view.*
import kotlinx.android.synthetic.main.rv_debug_view_item.view.*

// The ModelView annotation is used on Views to have models generated from those views.
// This is pretty straightforward with Kotlin, but properties need some special handling.
@ModelView(autoLayout = ModelView.Size.MATCH_WIDTH_WRAP_HEIGHT)
class DebugItemView @JvmOverloads constructor(
    context: Context,
    attrs: AttributeSet? = null,
    defStyleAttr: Int = 0
) : LinearLayout(context, attrs, defStyleAttr) {

    init {
        inflate(context, R.layout.rv_debug_view_item, this)
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
