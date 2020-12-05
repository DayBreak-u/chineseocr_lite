Imports System.Drawing
Imports Emgu.CV
Imports Emgu.CV.CvEnum
Imports Emgu.CV.Structure
Imports Microsoft.ML.OnnxRuntime.Tensors

Namespace OcrLiteLib
    Public Class OcrUtils
        Public Shared Function SubstractMeanNormalize(src As Mat, meanVals As Single(), normVals As Single()) As Tensor(Of Single)
            Dim cols As Integer = src.Cols
            Dim rows As Integer = src.Rows
            Dim channels As Integer = src.NumberOfChannels
            Dim srcImg As Image(Of Rgb, Byte) = src.ToImage(Of Rgb, Byte)
            Dim imgData As Byte(,,) = srcImg.Data
            Dim inputTensor As Tensor(Of Single) = New DenseTensor(Of Single)({1, channels, rows, cols})
            For r As Integer = 0 To rows - 1
                For c As Integer = 0 To cols - 1
                    For ch As Integer = 0 To channels - 1
                        Dim value = imgData(r, c, ch)
                        Dim data As Single = CSng(value) * normVals(ch) - meanVals(ch) * normVals(ch)
                        inputTensor(0, ch, r, c) = data
                    Next
                Next
            Next
            Return inputTensor
        End Function

        Public Shared Function MakePadding(src As Mat, padding As Integer) As Mat
            If padding <= 0 Then
                Return src
            End If
            Dim paddingScalar As New MCvScalar(255, 255, 255)
            Dim paddingSrc As New Mat()
            CvInvoke.CopyMakeBorder(src, paddingSrc, padding, padding, padding, padding,
                BorderType.Isolated, paddingScalar)
            Return paddingSrc
        End Function

        Public Shared Function GetThickness(boxImg As Mat) As Integer
            Dim minSize As Integer = If(boxImg.Cols > boxImg.Rows, boxImg.Rows, boxImg.Cols)
            Dim thickness As Integer = minSize / 1000 + 2
            Return thickness
        End Function

        Public Shared Sub DrawTextBox(boxImg As Mat, box As List(Of Point), thickness As Integer)
            If box Is Nothing OrElse box.Count = 0 Then
                Return
            End If
            Dim color = New MCvScalar(255, 0, 0)
            'R(255) G(0) B(0)
            CvInvoke.Line(boxImg, box(0), box(1), color, thickness)
            CvInvoke.Line(boxImg, box(1), box(2), color, thickness)
            CvInvoke.Line(boxImg, box(2), box(3), color, thickness)
            CvInvoke.Line(boxImg, box(3), box(0), color, thickness)
        End Sub

        Public Shared Sub DrawTextBoxes(src As Mat, textBoxes As List(Of TextBox), thickness As Integer)
            For i As Integer = 0 To textBoxes.Count - 1
                Dim t As TextBox = textBoxes(i)
                DrawTextBox(src, t.Points, thickness)
            Next
        End Sub

        Public Shared Function GetPartImages(src As Mat, textBoxes As List(Of TextBox)) As List(Of Mat)
            Dim partImages As New List(Of Mat)()
            For i As Integer = 0 To textBoxes.Count - 1
                Dim partImg As Mat = GetRotateCropImage(src, textBoxes(i).Points)
                'Mat partImg = new Mat();
                'GetRoiFromBox(src, partImg, textBoxes[i].Points);
                partImages.Add(partImg)
            Next
            Return partImages
        End Function

        Public Shared Function GetRotateCropImage(src As Mat, box As List(Of Point)) As Mat
            Dim image As New Mat()
            src.CopyTo(image)
            Dim points As New List(Of Point)
            points.AddRange(box)

            Dim collectX As Integer() = {box(0).X, box(1).X, box(2).X, box(3).X}
            Dim collectY As Integer() = {box(0).Y, box(1).Y, box(2).Y, box(3).Y}
            Dim left As Integer = collectX.Min()
            Dim right As Integer = collectX.Max()
            Dim top As Integer = collectY.Min()
            Dim bottom As Integer = collectY.Max()

            Dim rect As New Rectangle(left, top, right - left, bottom - top)
            Dim imgCrop As New Mat(image, rect)

            For i As Integer = 0 To points.Count - 1
                Dim pt = points(i)
                pt.X -= left
                pt.Y -= top
                points(i) = pt
            Next

            Dim imgCropWidth As Single = CSng(Math.Sqrt(Math.Pow(points(0).X - points(1).X, 2) + Math.Pow(points(0).Y - points(1).Y, 2)))
            Dim imgCropHeight As Single = CSng(Math.Sqrt(Math.Pow(points(0).X - points(3).X, 2) + Math.Pow(points(0).Y - points(3).Y, 2)))

            Dim ptsDst0 = New PointF(0, 0)
            Dim ptsDst1 = New PointF(imgCropWidth, 0)
            Dim ptsDst2 = New PointF(imgCropWidth, imgCropHeight)
            Dim ptsDst3 = New PointF(0, imgCropHeight)

            Dim ptsDst As PointF() = {ptsDst0, ptsDst1, ptsDst2, ptsDst3}

            Dim ptsSrc0 = New PointF(points(0).X, points(0).Y)
            Dim ptsSrc1 = New PointF(points(1).X, points(1).Y)
            Dim ptsSrc2 = New PointF(points(2).X, points(2).Y)
            Dim ptsSrc3 = New PointF(points(3).X, points(3).Y)

            Dim ptsSrc As PointF() = {ptsSrc0, ptsSrc1, ptsSrc2, ptsSrc3}

            Dim M As Mat = CvInvoke.GetPerspectiveTransform(ptsSrc, ptsDst)

            Dim partImg As New Mat()
            CvInvoke.WarpPerspective(imgCrop, partImg, M, New Size(imgCropWidth, imgCropHeight), Inter.Nearest, Warp.[Default],
                BorderType.Replicate)

            If partImg.Rows >= partImg.Cols * 1.5 Then
                Dim srcCopy As New Mat()
                CvInvoke.Transpose(partImg, srcCopy)
                CvInvoke.Flip(srcCopy, srcCopy, 0)
                Return srcCopy
            Else
                Return partImg
            End If
        End Function

        Public Shared Function MatRotateClockWise180(src As Mat) As Mat
            CvInvoke.Flip(src, src, FlipType.Vertical)
            CvInvoke.Flip(src, src, FlipType.Horizontal)
            Return src
        End Function

        Public Shared Function MatRotateClockWise90(src As Mat) As Mat
            CvInvoke.Rotate(src, src, RotateFlags.Rotate90CounterClockwise)
            Return src
        End Function
    End Class
End Namespace