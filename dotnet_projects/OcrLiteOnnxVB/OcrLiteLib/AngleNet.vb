Imports System.Drawing
Imports Emgu.CV
Imports Emgu.CV.CvEnum
Imports Emgu.CV.Structure
Imports Microsoft.ML.OnnxRuntime
Imports Microsoft.ML.OnnxRuntime.Tensors

Namespace OcrLiteLib
    Public Class AngleNet
        Private ReadOnly MeanValues As Single() = {127.5F, 127.5F, 127.5F}
        Private ReadOnly NormValues As Single() = {1.0F / 127.5F, 1.0F / 127.5F, 1.0F / 127.5F}
        Private Const angleDstWidth As Integer = 192
        Private Const angleDstHeight As Integer = 32
        Private Const angleCols As Integer = 2
        Private angleNet As InferenceSession

        Public Sub New(path As String, numThread As Integer)
            Try
                Dim op As New SessionOptions()
                op.GraphOptimizationLevel = GraphOptimizationLevel.ORT_ENABLE_EXTENDED
                op.InterOpNumThreads = numThread
                op.IntraOpNumThreads = numThread
                angleNet = New InferenceSession(path)
            Catch ex As Exception
                Console.WriteLine(ex.Message + ex.StackTrace)
                Throw ex
            End Try
        End Sub

        Protected Overrides Sub Finalize()
            Try
                angleNet.Dispose()
            Finally
                MyBase.Finalize()
            End Try
        End Sub

        Public Function GetAngles(partImgs As List(Of Mat), doAngle As Boolean, mostAngle As Boolean) As List(Of Angle)
            Dim angles As New List(Of Angle)()
            If doAngle Then
                For i As Integer = 0 To partImgs.Count - 1
                    Dim startTicks = DateTime.Now.Ticks
                    Dim angle = GetAngle(partImgs(i))
                    Dim endTicks = DateTime.Now.Ticks
                    Dim angleTime = (endTicks - startTicks) / 10000.0F
                    angle.Time = angleTime
                    angles.Add(angle)
                Next
            Else
                For i As Integer = 0 To partImgs.Count - 1
                    Dim angle = New Angle(-1, 0, 0)
                    angles.Add(angle)
                Next
            End If
            'Most Possible AngleIndex
            If doAngle AndAlso mostAngle Then
                Dim angleIndexes As New List(Of Integer)
                angles.ForEach(Sub(x) angleIndexes.Add(x.Index))

                Dim sum As Double = angleIndexes.Sum()
                Dim halfPercent As Double = angles.Count / 2.0F
                Dim mostAngleIndex As Integer
                If sum < halfPercent Then
                    'all angle set to 0
                    mostAngleIndex = 0
                Else
                    'all angle set to 1
                    mostAngleIndex = 1
                End If
                Console.WriteLine("Set All Angle to mostAngleIndex({mostAngleIndex})")
                For i As Integer = 0 To angles.Count - 1
                    Dim angle As Angle = angles(i)
                    angle.Index = mostAngleIndex
                    angles(i) = angle
                Next
            End If
            Return angles
        End Function

        Private Function GetAngle(src As Mat) As Angle
            Dim angle As New Angle()
            Dim angleImg As Mat = AdjustTargetImg(src, angleDstWidth, angleDstHeight)
            Dim inputTensors As Tensor(Of Single) = OcrUtils.SubstractMeanNormalize(angleImg, MeanValues, NormValues)
            Dim inputs = New List(Of NamedOnnxValue)() From {
                NamedOnnxValue.CreateFromTensor("input", inputTensors)
            }
            Try
                Using results As IDisposableReadOnlyCollection(Of DisposableNamedOnnxValue) = angleNet.Run(inputs)
                    Dim resultsArray = results.ToArray()
                    Console.WriteLine(resultsArray)
                    Dim outputData As Single() = resultsArray(0).AsEnumerable(Of Single)().ToArray()
                    Return ScoreToAngle(outputData, angleCols)
                End Using
            Catch ex As Exception
                'throw ex;
                Console.WriteLine(ex.Message + ex.StackTrace)
            End Try
            Return angle

        End Function

        Private Function ScoreToAngle(srcData As Single(), angleCols As Integer) As Angle
            Dim angleIndex As Integer = 0
            Dim maxValue As Single = -1000.0F
            For i As Integer = 0 To angleCols - 1
                If i = 0 Then
                    maxValue = srcData(i)
                ElseIf srcData(i) > maxValue Then
                    angleIndex = i
                    maxValue = srcData(i)
                End If
            Next
            Dim angle As New Angle(angleIndex, maxValue)
            Return angle
        End Function

        Private Function AdjustTargetImg(src As Mat, dstWidth As Integer, dstHeight As Integer) As Mat
            Dim srcResize As New Mat()
            Dim scale As Single = CSng(dstHeight) / CSng(src.Rows)
            Dim angleWidth As Integer = CInt(CSng(src.Cols) * scale)
            CvInvoke.Resize(src, srcResize, New Size(angleWidth, dstHeight))
            Dim srcFit As New Mat(dstHeight, dstWidth, DepthType.Cv8U, 3)
            'srcFit.SetTo(new MCvScalar(255,255,255));
            If angleWidth < dstWidth Then
                CvInvoke.CopyMakeBorder(srcResize, srcFit, 0, 0, 0, dstWidth - angleWidth,
                    BorderType.Isolated, New MCvScalar(255, 255, 255))
            Else
                Dim rect As New Rectangle(0, 0, dstWidth, dstHeight)
                Dim partAngle As New Mat(srcResize, rect)
                partAngle.CopyTo(srcFit)
            End If
            Return srcFit
        End Function
    End Class
End Namespace
