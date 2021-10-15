package com.benjaminwan.ocr.onnx.dialog

import android.os.Bundle
import android.view.Gravity
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.TextView
import androidx.recyclerview.widget.DefaultItemAnimator
import com.airbnb.epoxy.EpoxyRecyclerView
import com.benjaminwan.ocr.onnx.R
import com.benjaminwan.ocr.onnx.models.dbNetTimeItemView
import com.benjaminwan.ocr.onnx.models.debugItemView
import com.benjaminwan.ocr.onnx.utils.format
import com.benjaminwan.ocr.onnx.utils.hideSoftInput
import com.benjaminwan.ocr.onnx.utils.setMarginItemDecoration
import com.benjaminwan.ocrlibrary.OcrResult
import com.benjaminwan.ocrlibrary.TextBlock

class DebugDialog : BaseDialog(), View.OnClickListener {
    companion object {
        val instance: DebugDialog
            get() {
                val dialog = DebugDialog()
                dialog.setCanceledBack(true)
                dialog.setCanceledOnTouchOutside(false)
                dialog.setGravity(Gravity.CENTER)
                dialog.setAnimStyle(R.style.diag_top_down_up_animation)
                return dialog
            }
    }

    private var title: String = ""
    private var textBlocks: MutableList<TextBlock> = mutableListOf()
    private var dbnetTime: Double = 0.0

    private lateinit var debugRV: EpoxyRecyclerView
    private lateinit var negativeBtn: Button
    private lateinit var positiveBtn: Button
    private lateinit var titleTV: TextView

    override fun onCreateView(
        inflater: LayoutInflater,
        viewGroup: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        return inflater.inflate(R.layout.dialog_debug, viewGroup, false)
    }

    private fun findViews(view: View) {
        debugRV = view.findViewById(R.id.debugRV)
        negativeBtn = view.findViewById(R.id.negativeBtn)
        positiveBtn = view.findViewById(R.id.positiveBtn)
        titleTV = view.findViewById(R.id.titleTV)
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        findViews(view)
        initViews()
    }

    override fun dismiss() {
        hideSoftInput()
        super.dismiss()
    }

    private fun initViews() {
        debugRV.setHasFixedSize(true)
        debugRV.itemAnimator = DefaultItemAnimator()
        debugRV.setMarginItemDecoration(2, 1, 2, 1)

        debugRV.withModels {
            dbNetTimeItemView {
                id("dbnet time item")
                dbNetTimeStr(dbnetTime.format("#0.00") + "ms")
            }
            textBlocks.withIndex().forEach { (id, item) ->
                val boxPointStr = item.boxPoint.map { "[${it.x},${it.y}]" }.joinToString()
                val charScoresStr = item.charScores.map { it.format("#0.00") }.joinToString()
                debugItemView {
                    id("debug view $id")
                    index("$id")
                    boxPoint(boxPointStr)
                    boxScore(item.boxScore.format("#0.00"))
                    angleIndex(item.angleIndex.toString())
                    angleScore(item.angleScore.format("#0.00"))
                    angleTime(item.angleTime.format("#0.00") + "ms")
                    text(item.text)
                    charScores(charScoresStr)
                    crnnTime(item.crnnTime.format("#0.00") + "ms")
                    blockTime(item.blockTime.format("#0.00") + "ms")
                }
            }
        }

        negativeBtn.setOnClickListener(this)
        positiveBtn.setOnClickListener(this)
        if (title.isNotEmpty()) {
            titleTV.text = title
        }

    }

    fun setTitle(title: String): DebugDialog {
        this.title = title
        return this
    }

    fun setResult(result: OcrResult): DebugDialog {
        textBlocks.clear()
        textBlocks.addAll(result.textBlocks)
        dbnetTime = result.dbNetTime
        return this
    }

    override fun onClick(view: View) {
        val resId = view.id
        if (resId == R.id.negativeBtn) {
            dismiss()
        } else if (resId == R.id.positiveBtn) {
            this.dismiss()
        }
    }

}
