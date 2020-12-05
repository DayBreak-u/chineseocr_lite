Imports System.Drawing
Imports System.Text
Imports Emgu.CV
Imports Emgu.CV.CvEnum

Namespace OcrLiteLib
    Public Class OcrLite
        Public Property isPartImg() As Boolean
            Get
                Return m_isPartImg
            End Get
            Set
                m_isPartImg = Value
            End Set
        End Property
        Private m_isPartImg As Boolean
        Public Property isDebugImg() As Boolean
            Get
                Return m_isDebugImg
            End Get
            Set
                m_isDebugImg = Value
            End Set
        End Property
        Private m_isDebugImg As Boolean
        Private dbNetPath As String, angleNetPath As String, crnnNetPath As String, keysPath As String
        Private dbNet As DbNet
        Private angleNet As AngleNet
        Private crnnNet As CrnnNet

        Public Sub New(models As String, numThread As Integer)
            dbNetPath = models & Convert.ToString("\dbnet.onnx")
            angleNetPath = models & Convert.ToString("\angle_net.onnx")
            crnnNetPath = models & Convert.ToString("\crnn_lite_lstm.onnx")
            keysPath = models & Convert.ToString("\keys.txt")
            Try
                dbNet = New DbNet(dbNetPath, numThread)
                angleNet = New AngleNet(angleNetPath, numThread)
                crnnNet = New CrnnNet(crnnNetPath, keysPath, numThread)
            Catch ex As Exception
                Console.WriteLine(ex.Message + ex.StackTrace)
                Throw ex
            End Try
        End Sub

        Public Function Detect(img As String, padding As Integer, imgResize As Integer, boxScoreThresh As Single, boxThresh As Single, minArea As Single,
            unClipRatio As Single, doAngle As Boolean, mostAngle As Boolean) As OcrResult
            Dim brgSrc As Mat = CvInvoke.Imread(img, ImreadModes.Color)
            'default : BGR
            Dim originSrc As New Mat()
            CvInvoke.CvtColor(brgSrc, originSrc, ColorConversion.Bgr2Rgb)
            ' convert to RGB
            Dim originRect As New Rectangle(padding, padding, originSrc.Cols, originSrc.Rows)
            Dim paddingSrc As Mat = OcrUtils.MakePadding(originSrc, padding)

            Dim resize As Integer
            If imgResize <= 0 Then
                resize = Math.Max(paddingSrc.Cols, paddingSrc.Rows)
            Else
                resize = imgResize
            End If
            Dim scale As ScaleParam = ScaleParam.GetScaleParam(paddingSrc, resize)

            Return DetectOnce(paddingSrc, originRect, scale, boxScoreThresh, boxThresh, minArea,
                unClipRatio, doAngle, mostAngle)
        End Function

        Private Function DetectOnce(src As Mat, originRect As Rectangle, scale As ScaleParam, boxScoreThresh As Single, boxThresh As Single, minArea As Single,
            unClipRatio As Single, doAngle As Boolean, mostAngle As Boolean) As OcrResult
            Dim textBoxPaddingImg As Mat = src.Clone()
            Dim thickness As Integer = OcrUtils.GetThickness(src)
            Console.WriteLine("=====Start detect=====")
            Dim startTicks = DateTime.Now.Ticks

            Console.WriteLine("---------- step: dbNet getTextBoxes ----------")
            Dim textBoxes = dbNet.GetTextBoxes(src, scale, boxScoreThresh, boxThresh, minArea, unClipRatio)
            Dim dbNetTime = (DateTime.Now.Ticks - startTicks) / 10000.0F

            Console.WriteLine("TextBoxesSize({textBoxes.Count})")
            textBoxes.ForEach(Sub(x) Console.WriteLine(x.ToString()))
            'Console.WriteLine($"dbNetTime({dbNetTime}ms)");

            Console.WriteLine("---------- step: drawTextBoxes ----------")
            OcrUtils.DrawTextBoxes(textBoxPaddingImg, textBoxes, thickness)
            'CvInvoke.Imshow("ResultPadding", textBoxPaddingImg);

            '---------- getPartImages ----------
            Dim partImages As List(Of Mat) = OcrUtils.GetPartImages(src, textBoxes)
            If isPartImg Then
                For i As Integer = 0 To partImages.Count - 1
                    CvInvoke.Imshow($"PartImg({i})", partImages(i))
                Next
            End If

            Console.WriteLine("---------- step: angleNet getAngles ----------")
            Dim angles As List(Of Angle) = angleNet.GetAngles(partImages, doAngle, mostAngle)
            'angles.ForEach(x => Console.WriteLine(x));

            'Rotate partImgs
            For i As Integer = 0 To partImages.Count - 1
                If angles(i).Index = 0 Then
                    partImages(i) = OcrUtils.MatRotateClockWise180(partImages(i))
                End If
                If isDebugImg Then
                    CvInvoke.Imshow($"DebugImg({i})", partImages(i))
                End If
            Next

            Console.WriteLine("---------- step: crnnNet getTextLines ----------")
            Dim textLines As List(Of TextLine) = crnnNet.GetTextLines(partImages)
            'textLines.ForEach(x => Console.WriteLine(x));

            Dim textBlocks As New List(Of TextBlock)()
            For i As Integer = 0 To textLines.Count - 1
                Dim textBlock As New TextBlock()
                textBlock.BoxPoints = textBoxes(i).Points
                textBlock.BoxScore = textBoxes(i).Score
                textBlock.AngleIndex = angles(i).Index
                textBlock.AngleScore = angles(i).Score
                textBlock.AngleTime = angles(i).Time
                textBlock.Text = textLines(i).Text
                textBlock.CharScores = textLines(i).CharScores
                textBlock.CrnnTime = textLines(i).Time
                textBlock.BlockTime = angles(i).Time + textLines(i).Time
                textBlocks.Add(textBlock)
            Next
            'textBlocks.ForEach(x => Console.WriteLine(x));

            Dim endTicks = DateTime.Now.Ticks
            Dim fullDetectTime = (endTicks - startTicks) / 10000.0F
            'Console.WriteLine($"fullDetectTime({fullDetectTime}ms)");

            'cropped to original size
            Dim rgbBoxImg As New Mat(textBoxPaddingImg, originRect)
            Dim boxImg As New Mat()
            CvInvoke.CvtColor(rgbBoxImg, boxImg, ColorConversion.Rgb2Bgr)
            'convert to BGR for Output Result Img
            'CvInvoke.Imshow("Result", boxImg);
            Dim strRes As New StringBuilder()
            textBlocks.ForEach(Function(x) strRes.AppendLine(x.Text))

            Dim ocrResult As New OcrResult()
            ocrResult.TextBlocks = textBlocks
            ocrResult.DbNetTime = dbNetTime
            ocrResult.BoxImg = boxImg
            ocrResult.DetectTime = fullDetectTime
            ocrResult.StrRes = strRes.ToString()

            Return ocrResult
        End Function
    End Class
End Namespace
