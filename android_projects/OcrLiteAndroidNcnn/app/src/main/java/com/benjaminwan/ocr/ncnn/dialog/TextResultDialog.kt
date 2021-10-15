package com.benjaminwan.ocr.ncnn.dialog

import android.os.Bundle
import android.view.Gravity
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.EditText
import android.widget.TextView
import com.benjaminwan.ocr.ncnn.R
import com.benjaminwan.ocr.ncnn.utils.hideSoftInput
import com.benjaminwan.ocr.ncnn.utils.toClipboard

class TextResultDialog : BaseDialog(), View.OnClickListener {
    companion object {
        val instance: TextResultDialog
            get() {
                val dialog = TextResultDialog()
                dialog.setCanceledBack(true)
                dialog.setCanceledOnTouchOutside(false)
                dialog.setGravity(Gravity.CENTER)
                dialog.setAnimStyle(R.style.diag_top_down_up_animation)
                return dialog
            }
    }

    private var content: String = ""
    private var title: String = ""

    private lateinit var negativeBtn: Button
    private lateinit var positiveBtn: Button
    private lateinit var titleTV: TextView
    private lateinit var contentEdit: EditText

    override fun onCreateView(
        inflater: LayoutInflater,
        viewGroup: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        return inflater.inflate(R.layout.dialog_text_result, viewGroup, false)
    }

    private fun findViews(view: View) {
        negativeBtn = view.findViewById(R.id.negativeBtn)
        positiveBtn = view.findViewById(R.id.positiveBtn)
        titleTV = view.findViewById(R.id.titleTV)
        contentEdit = view.findViewById(R.id.contentEdit)
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
        negativeBtn.setOnClickListener(this)
        positiveBtn.setOnClickListener(this)
        contentEdit.setText(content)
        if (title.isNotEmpty()) {
            titleTV.text = title
        }
    }

    fun setTitle(title: String): TextResultDialog {
        this.title = title
        return this
    }

    fun setContent(textContent: String): TextResultDialog {
        content = textContent
        return this
    }

    override fun onClick(view: View) {
        val resId = view.id
        if (resId == R.id.negativeBtn) {
            dismiss()
        } else if (resId == R.id.positiveBtn) {
            requireContext().toClipboard(content)
            this.dismiss()
        }
    }

}
