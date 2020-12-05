Imports System.Drawing
Imports System.IO
Imports System.Text
Imports Emgu.CV
Imports Microsoft.ML.OnnxRuntime
Imports Microsoft.ML.OnnxRuntime.Tensors

Namespace OcrLiteLib
    Public Class CrnnNet
        Private ReadOnly MeanValues As Single() = {127.5F, 127.5F, 127.5F}
        Private ReadOnly NormValues As Single() = {1.0F / 127.5F, 1.0F / 127.5F, 1.0F / 127.5F}
        Private Const crnnDstHeight As Integer = 32
        Private Const crnnCols As Integer = 5531

        Private crnnNet As InferenceSession
        Private keys As List(Of String)

        Public Sub New(path As String, keysPath As String, numThread As Integer)
            Try
                Dim op As New SessionOptions()
                op.GraphOptimizationLevel = GraphOptimizationLevel.ORT_ENABLE_EXTENDED
                op.InterOpNumThreads = numThread
                op.IntraOpNumThreads = numThread
                crnnNet = New InferenceSession(path, op)
                keys = InitKeys(keysPath)
            Catch ex As Exception
                Console.WriteLine(ex.Message + ex.StackTrace)
                Throw ex
            End Try
        End Sub

        Protected Overrides Sub Finalize()
            Try
                crnnNet.Dispose()
            Finally
                MyBase.Finalize()
            End Try
        End Sub

        Private Function InitKeys(path As String) As List(Of String)
            Dim sr As New StreamReader(path, Encoding.UTF8)
            Dim keys As New List(Of String)()
            Dim line As [String]
            While (InlineAssignHelper(line, sr.ReadLine())) IsNot Nothing
                'Console.WriteLine(line.ToString());
                keys.Add(line)
            End While
            Console.WriteLine($"keys Size = {keys.Count}")
            Return keys
        End Function

        Public Function GetTextLines(partImgs As List(Of Mat)) As List(Of TextLine)
            Dim textLines As New List(Of TextLine)
            For i As Integer = 0 To partImgs.Count - 1
                Dim startTicks = DateTime.Now.Ticks
                Dim textLine = GetTextLine(partImgs(i))
                Dim endTicks = DateTime.Now.Ticks
                Dim crnnTime = (endTicks - startTicks) / 10000.0F
                textLine.Time = crnnTime
                textLines.Add(textLine)
            Next
            Return textLines
        End Function

        Private Function GetTextLine(src As Mat) As TextLine
            Dim textLine As New TextLine()

            Dim scale As Single = CSng(crnnDstHeight) / CSng(src.Rows)
            Dim dstWidth As Integer = CInt(CSng(src.Cols) * scale)

            Dim srcResize As New Mat()
            CvInvoke.Resize(src, srcResize, New Size(dstWidth, crnnDstHeight))
            Dim inputTensors As Tensor(Of Single) = OcrUtils.SubstractMeanNormalize(srcResize, MeanValues, NormValues)
            Dim inputs = New List(Of NamedOnnxValue) From {
                NamedOnnxValue.CreateFromTensor("input", inputTensors)
            }
            Try
                Using results As IDisposableReadOnlyCollection(Of DisposableNamedOnnxValue) = crnnNet.Run(inputs)
                    Dim resultsArray = results.ToArray()
                    Console.WriteLine(resultsArray)
                    Dim outputData As Single() = resultsArray(0).AsEnumerable(Of Single)().ToArray()

                    Dim crnnRows As Integer = outputData.Count() / crnnCols

                    Return ScoreToTextLine(outputData, crnnRows, crnnCols)
                End Using
            Catch ex As Exception
                'throw ex;
                Console.WriteLine(ex.Message + ex.StackTrace)
            End Try

            Return textLine
        End Function

        Private Function ScoreToTextLine(srcData As Single(), rows As Integer, cols As Integer) As TextLine
            Dim sb As New StringBuilder()
            Dim textLine As New TextLine()

            Dim lastIndex As Integer = 0
            Dim scores As New List(Of Single)

            For i As Integer = 0 To rows - 1
                Dim maxIndex As Integer = 0
                Dim maxValue As Single = -1000.0F

                'do softmax
                Dim expList As New List(Of Single)
                For j As Integer = 0 To cols - 1
                    Dim expSingle As Single = CSng(Math.Exp(srcData(i * cols + j)))
                    expList.Add(expSingle)
                Next
                Dim partition As Single = expList.Sum()
                For j As Integer = 0 To cols - 1
                    Dim softmax As Single = expList(j) / partition
                    If softmax > maxValue Then
                        maxValue = softmax
                        maxIndex = j
                    End If
                Next

                If maxIndex > 0 AndAlso maxIndex < keys.Count AndAlso (Not (i > 0 AndAlso maxIndex = lastIndex)) Then
                    scores.Add(maxValue)
                    sb.Append(keys(maxIndex - 1))
                End If
                lastIndex = maxIndex
            Next
            textLine.Text = sb.ToString()
            textLine.CharScores = scores
            Return textLine
        End Function
        Private Shared Function InlineAssignHelper(Of T)(ByRef target As T, value As T) As T
            target = value
            Return value
        End Function
    End Class
End Namespace
