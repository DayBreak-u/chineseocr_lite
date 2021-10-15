package com.benjaminwan.ocr.ncnn.dialog

import android.app.Dialog
import android.util.DisplayMetrics
import android.view.Gravity
import android.view.ViewGroup
import android.view.WindowManager
import androidx.fragment.app.DialogFragment

abstract class BaseDialog : DialogFragment() {

    private var mGravity = Gravity.CENTER//对话框的位置
    private var mCanceledOnTouchOutside = true//是否触摸外部关闭
    private var mCanceledBack = true//是否返回键关闭
    private var mAnimStyle: Int = 0//显示动画

    /*override fun onActivityCreated(savedInstanceState: Bundle?) {
        //修复DialogFragment内存泄漏
        if (showsDialog) {
            showsDialog = false
        }
        super.onActivityCreated(savedInstanceState)
        showsDialog = true

        val tmpView = view
        if (tmpView != null) {
            check(tmpView.parent == null) { "DialogFragment can not be attached to a container view" }
            dialog?.setContentView(tmpView)
        }

        activity?.let {
            dialog?.setOwnerActivity(it)
        }

        if (savedInstanceState != null) {
            val dialogState = savedInstanceState.getBundle("android:savedDialogState")
            if (dialogState != null) {
                dialog?.onRestoreInstanceState(dialogState)
            }
        }
    }*/

    fun setGravity(gravity: Int) {
        mGravity = gravity
    }

    fun setCanceledBack(canceledBack: Boolean) {
        mCanceledBack = canceledBack
    }

    fun setCanceledOnTouchOutside(canceledOnTouchOutsidek: Boolean) {
        mCanceledOnTouchOutside = canceledOnTouchOutsidek
    }

    fun setAnimStyle(animStyle: Int) {
        mAnimStyle = animStyle
    }

    override fun onStart() {
        val baseDialog = dialog
        if (baseDialog != null) {
            baseDialog.setCanceledOnTouchOutside(mCanceledOnTouchOutside)
            baseDialog.setCancelable(mCanceledBack)
            setDialogGravity(baseDialog)//设置对话框布局
        }
        // 全屏显示Dialog，重新测绘宽高
        if (dialog != null) {
            val dm = DisplayMetrics()
            activity?.windowManager?.defaultDisplay?.getMetrics(dm)
            val attributes = dialog!!.window!!.attributes
            attributes.gravity = Gravity.CENTER//对齐方式
            dialog?.window?.attributes = attributes
            dialog?.window?.setLayout(
                ViewGroup.LayoutParams.MATCH_PARENT,
                (dm.heightPixels * 0.9).toInt()
            )
        }
        super.onStart()
    }

    /**
     * 对话框配置
     *
     * @param dialog
     */
    private fun setDialogGravity(dialog: Dialog) {
        val window = dialog.window ?: return
        val wlp = window.attributes
        wlp.gravity = mGravity
        wlp.width = WindowManager.LayoutParams.MATCH_PARENT
        wlp.height = WindowManager.LayoutParams.WRAP_CONTENT

        window.attributes = wlp
        //动画
        if (mAnimStyle != 0) {
            window.setWindowAnimations(mAnimStyle)
        }
    }
}