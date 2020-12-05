Imports Emgu.CV

Namespace OcrLiteLib
    Public Class ScaleParam
        Public Property SrcWidth As Integer
            Get
                Return mSrcWidth
            End Get
            Set(value As Integer)
                mSrcWidth = value
            End Set
        End Property
        Private mSrcWidth As Integer
        Public Property SrcHeight As Integer
            Get
                Return mSrcHeight
            End Get
            Set(value As Integer)
                mSrcHeight = value
            End Set
        End Property
        Private mSrcHeight As Integer

        Public Property DstWidth As Integer
            Get
                Return mDstWidth
            End Get
            Set(value As Integer)
                mDstWidth = value
            End Set
        End Property
        Private mDstWidth As Integer
        Public Property DstHeight As Integer
            Get
                Return mDstHeight
            End Get
            Set(value As Integer)
                mDstHeight = value
            End Set
        End Property
        Private mDstHeight As Integer
        Public Property ScaleWidth As Single
            Get
                Return mScaleWidth
            End Get
            Set(value As Single)
                mScaleWidth = value
            End Set
        End Property
        Private mScaleWidth As Single
        Public Property ScaleHeight As Single
            Get
                Return mScaleHeight
            End Get
            Set(value As Single)
                mScaleHeight = value
            End Set
        End Property
        Private mScaleHeight As Single
        Public Sub New(ByVal srcWidth As Integer, ByVal srcHeight As Integer, ByVal dstWidth As Integer, ByVal dstHeight As Integer, ByVal scaleWidth As Single, ByVal scaleHeight As Single)
            mSrcWidth = srcWidth
            mSrcHeight = srcHeight
            mDstWidth = dstWidth
            mDstHeight = dstHeight
            mScaleWidth = scaleWidth
            mScaleHeight = scaleHeight
        End Sub

        Friend Shared Function GetScaleParam(src As Mat, dstSize As Integer) As ScaleParam
            Dim SrcWidth As Integer
            Dim SrcHeight As Integer
            Dim DstWidth As Integer
            Dim DstHeight As Integer

            SrcWidth = src.Cols
            DstWidth = src.Cols
            SrcHeight = src.Rows
            DstHeight = src.Rows

            Dim scale As Single = 1.0F
            If (DstWidth > DstHeight) Then
                scale = CSng(dstSize) / CSng(DstWidth)
                DstWidth = dstSize
                DstHeight = CInt(CSng(DstHeight) * scale)
            Else
                scale = CSng(dstSize) / CSng(DstHeight)
                DstHeight = dstSize
                DstWidth = CInt(CSng(DstWidth) * scale)
            End If
            If (DstWidth Mod 32 <> 0) Then
                DstWidth = (CInt(DstWidth / 32) - 1) * 32
                DstWidth = Math.Max(DstWidth, 32)
            End If
            If (DstHeight Mod 32 <> 0) Then
                DstHeight = (CInt(DstHeight / 32) - 1) * 32
                DstHeight = Math.Max(DstHeight, 32)
            End If
            Dim ScaleWidth As Single = CSng(DstWidth) / CSng(SrcWidth)
            Dim ScaleHeight As Single = CSng(DstHeight) / CSng(SrcHeight)
            Return New ScaleParam(SrcWidth, SrcHeight, DstWidth, DstHeight, ScaleWidth, ScaleHeight)
        End Function
    End Class
End Namespace
