Imports System.Drawing
Imports ClipperLib
Imports Emgu.CV
Imports Emgu.CV.CvEnum
Imports Emgu.CV.Structure
Imports Emgu.CV.Util
Imports Microsoft.ML.OnnxRuntime
Imports Microsoft.ML.OnnxRuntime.Tensors

Namespace OcrLiteLib
    Public Class DbNet
        Private ReadOnly MeanValues As Single() = {0.485F * 255.0F, 0.456F * 255.0F, 0.406F * 255.0F}
        Private ReadOnly NormValues As Single() = {1.0F / 0.229F / 255.0F, 1.0F / 0.224F / 255.0F, 1.0F / 0.225F / 255.0F}

        Private dbNet As InferenceSession

        Public Sub New(path As String, numThread As Integer)
            Try
                Dim op As New SessionOptions()
                op.GraphOptimizationLevel = GraphOptimizationLevel.ORT_ENABLE_EXTENDED
                op.InterOpNumThreads = numThread
                op.IntraOpNumThreads = numThread
                dbNet = New InferenceSession(path, op)
            Catch ex As Exception
                Console.WriteLine(ex.Message + ex.StackTrace)
                Throw ex
            End Try
        End Sub

        Protected Overrides Sub Finalize()
            Try
                dbNet.Dispose()
            Finally
                MyBase.Finalize()
            End Try
        End Sub

        Public Function GetTextBoxes(src As Mat, scale As ScaleParam, boxScoreThresh As Single, boxThresh As Single, minArea As Single, unClipRatio As Single) As List(Of TextBox)
            Dim srcResize As New Mat()
            CvInvoke.Resize(src, srcResize, New Size(scale.DstWidth, scale.DstHeight))
            Dim inputTensors As Tensor(Of Single) = OcrUtils.SubstractMeanNormalize(srcResize, MeanValues, NormValues)
            Dim inputs As List(Of NamedOnnxValue) = New List(Of NamedOnnxValue) From {
                NamedOnnxValue.CreateFromTensor("input0", inputTensors)
            }

            Try
                Using results As IDisposableReadOnlyCollection(Of DisposableNamedOnnxValue) = dbNet.Run(inputs)
                    Dim resultsArray = results.ToArray()
                    Console.WriteLine(resultsArray)
                    Dim textBoxes = GetTextBoxes(resultsArray, srcResize.Rows, srcResize.Cols, scale, boxScoreThresh, boxThresh,
                        minArea, unClipRatio)
                    Return textBoxes
                End Using
            Catch ex As Exception
                Console.WriteLine(ex.Message + ex.StackTrace)
                Throw ex
            End Try
            Return Nothing
        End Function

        Private Shared Function GetTextBoxes(outputTensor As DisposableNamedOnnxValue(), rows As Integer, cols As Integer, s As ScaleParam, boxScoreThresh As Single, boxThresh As Single,
            minArea As Single, unClipRatio As Single) As List(Of TextBox)
            Dim rsBoxes As New List(Of TextBox)

            Dim outputData As Single() = outputTensor(0).AsEnumerable(Of Single).ToArray()
            Dim norf As New List(Of Byte)
            For Each data As Single In outputData
                Dim val As Integer = If(data > boxThresh, 255, 0)
                norf.Add(CByte(val))
            Next
            Dim fMapMat As New Mat(rows, cols, DepthType.Cv32F, 1)
            fMapMat.SetTo(outputData)
            Console.WriteLine(fMapMat)

            Dim norfMapMat As New Mat(rows, cols, DepthType.Cv8U, 1)
            norfMapMat.SetTo(norf.ToArray())
            Console.WriteLine(norfMapMat)

            Dim contours As New VectorOfVectorOfPoint()

            CvInvoke.FindContours(norfMapMat, contours, Nothing, RetrType.List, ChainApproxMethod.ChainApproxSimple)
            For i As Integer = 0 To contours.Size - 1
                Dim minEdgeSize As Single = 0
                Dim box As List(Of PointF) = GetMiniBox(contours(i), minEdgeSize)
                If minEdgeSize < minArea Then
                    Continue For
                End If
                Dim score As Double = GetScore(contours(i), fMapMat)
                If score < CDbl(boxScoreThresh) Then
                    Continue For
                End If
                Dim newBox As List(Of Point) = Unclip(box, unClipRatio)
                If newBox Is Nothing Then
                    Continue For
                End If

                Dim minBox As List(Of PointF) = GetMiniBox(newBox, minEdgeSize)
                If minEdgeSize < minArea + 2 Then
                    Continue For
                End If
                Dim finalPoints As New List(Of Point)
                For Each item As PointF In minBox
                    Dim x As Integer = CInt(item.X / s.ScaleWidth)
                    Dim ptx As Integer = Math.Min(Math.Max(x, 0), s.SrcWidth)

                    Dim y As Integer = CInt(item.Y / s.ScaleHeight)
                    Dim pty As Integer = Math.Min(Math.Max(y, 0), s.SrcHeight)
                    Dim dstPt As New Point(ptx, pty)
                    finalPoints.Add(dstPt)
                Next

                Dim textBox As New TextBox(finalPoints, CSng(score))
                rsBoxes.Add(textBox)
            Next
            rsBoxes.Reverse()
            Return rsBoxes
        End Function

        Private Shared Function GetMiniBox(contours As List(Of Point), ByRef minEdgeSize As Single) As List(Of PointF)
            Dim vop As New VectorOfPoint()
            vop.Push(contours.ToArray())
            Return GetMiniBox(vop, minEdgeSize)
        End Function

        Private Shared Function GetMiniBox(contours As VectorOfPoint, ByRef minEdgeSize As Single) As List(Of PointF)
            Dim box As New List(Of PointF)
            Dim rrect As RotatedRect = CvInvoke.MinAreaRect(contours)
            Dim points As PointF() = CvInvoke.BoxPoints(rrect)
            minEdgeSize = Math.Min(rrect.Size.Width, rrect.Size.Height)

            Dim thePoints As New List(Of PointF)(points)
            thePoints.Sort(AddressOf CompareByX)

            Dim index_1 As Integer = 0, index_2 As Integer = 1, index_3 As Integer = 2, index_4 As Integer = 3
            If thePoints(1).Y > thePoints(0).Y Then
                index_1 = 0
                index_4 = 1
            Else
                index_1 = 1
                index_4 = 0
            End If

            If thePoints(3).Y > thePoints(2).Y Then
                index_2 = 2
                index_3 = 3
            Else
                index_2 = 3
                index_3 = 2
            End If

            box.Add(thePoints(index_1))
            box.Add(thePoints(index_2))
            box.Add(thePoints(index_3))
            box.Add(thePoints(index_4))

            Return box
        End Function

        Public Shared Function CompareByX(left As PointF, right As PointF) As Integer
            If left = Nothing AndAlso right = Nothing Then
                Return 1
            End If

            If left = Nothing Then
                Return 0
            End If

            If right = Nothing Then
                Return 1
            End If

            If left.X > right.X Then
                Return 1
            End If

            If left.X = right.X Then
                Return 0
            End If

            Return -1
        End Function

        Private Shared Function GetScore(contours As VectorOfPoint, fMapMat As Mat) As Double
            Dim xmin As Short = 9999
            Dim xmax As Short = 0
            Dim ymin As Short = 9999
            Dim ymax As Short = 0

            Try
                For Each point As Point In contours.ToArray()
                    If point.X < xmin Then
                        'var xx = nd[point.X];
                        xmin = CShort(point.X)
                    End If

                    If point.X > xmax Then
                        xmax = CShort(point.X)
                    End If

                    If point.Y < ymin Then
                        ymin = CShort(point.Y)
                    End If

                    If point.Y > ymax Then
                        ymax = CShort(point.Y)
                    End If
                Next

                Dim roiWidth As Integer = xmax - xmin + 1
                Dim roiHeight As Integer = ymax - ymin + 1

                Dim bitmap As Image(Of Gray, Single) = fMapMat.ToImage(Of Gray, Single)
                Dim roiBitmap As New Image(Of Gray, Single)(roiWidth, roiHeight)
                Dim dataFloat As Single(,,) = bitmap.Data
                Dim data As Single(,,) = roiBitmap.Data

                For j As Integer = ymin To ymin + (roiHeight - 1)
                    For i As Integer = xmin To xmin + (roiWidth - 1)
                        Try
                            data(j - ymin, i - xmin, 0) = dataFloat(j, i, 0)
                        Catch ex2 As Exception
                            Console.WriteLine(ex2.Message)
                        End Try
                    Next
                Next

                Dim mask As Mat = Mat.Zeros(roiHeight, roiWidth, DepthType.Cv8U, 1)
                Dim pts As New List(Of Point)
                For Each point As Point In contours.ToArray()
                    pts.Add(New Point(point.X - xmin, point.Y - ymin))
                Next

                Using vp As New VectorOfPoint(pts.ToArray())
                    Using vvp As New VectorOfVectorOfPoint(vp)
                        CvInvoke.FillPoly(mask, vvp, New MCvScalar(1))
                    End Using
                End Using

                Return CvInvoke.Mean(roiBitmap, mask).V0
            Catch ex As Exception
                Console.WriteLine(ex.Message + ex.StackTrace)
            End Try

            Return 0
        End Function

        Private Shared Function Unclip(box As List(Of PointF), unclip_ratio As Single) As List(Of Point)
            Dim theCliperPts As New List(Of IntPoint)
            For Each pt As PointF In box
                Dim a1 As New IntPoint(CInt(pt.X), CInt(pt.Y))
                theCliperPts.Add(a1)
            Next

            Dim area As Single = Math.Abs(SignedPolygonArea(box.ToArray()))
            Dim length As Double = LengthOfPoints(box)
            Dim distance As Double = CDbl(area) * CDbl(unclip_ratio) / length

            Dim co As New ClipperOffset()
            co.AddPath(theCliperPts, JoinType.jtRound, EndType.etClosedPolygon)
            Dim solution As New List(Of List(Of IntPoint))
            co.Execute(solution, distance)
            If solution.Count = 0 Then
                Return Nothing
            End If

            Dim retPts As New List(Of Point)
            For Each ip As IntPoint In solution(0)
                retPts.Add(New Point(CInt(ip.X), CInt(ip.Y)))
            Next

            Return retPts
        End Function

        Private Shared Function SignedPolygonArea(Points As PointF()) As Single
            ' Add the first point to the end.
            Dim num_points As Integer = Points.Length
            Dim pts As PointF() = New PointF(num_points) {}
            Points.CopyTo(pts, 0)
            pts(num_points) = Points(0)

            ' Get the areas.
            Dim area As Single = 0
            For i As Integer = 0 To num_points - 1
                area += (pts(i + 1).X - pts(i).X) * (pts(i + 1).Y + pts(i).Y) / 2
            Next

            Return area
        End Function

        Private Shared Function LengthOfPoints(box As List(Of PointF)) As Double
            Dim length As Double = 0

            Dim pt As PointF = box(0)
            Dim x0 As Double = CDbl(pt.X)
            Dim y0 As Double = CDbl(pt.Y)
            Dim x1 As Double = 0, y1 As Double = 0, dx As Double = 0, dy As Double = 0
            box.Add(pt)

            Dim count As Integer = box.Count
            For idx As Integer = 1 To count - 1
                Dim pts As PointF = box(idx)
                x1 = CDbl(pts.X)
                y1 = CDbl(pts.Y)
                dx = x1 - x0
                dy = y1 - y0

                length += Math.Sqrt(dx * dx + dy * dy)

                x0 = x1
                y0 = y1
            Next

            box.RemoveAt(count - 1)
            Return length
        End Function
    End Class
End Namespace
