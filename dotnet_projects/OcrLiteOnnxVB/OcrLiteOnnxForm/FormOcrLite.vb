Imports System.IO
Imports Emgu.CV
Imports Emgu.CV.CvEnum
Imports OcrLiteLib.OcrLiteLib

Public Class FormOcrLite
    Private ocrEngin As OcrLite
    Private Sub FormOcrLite_Load(sender As Object, e As EventArgs) Handles MyBase.Load
        Dim appPath As String = AppDomain.CurrentDomain.BaseDirectory
        Dim appDir As String = Directory.GetParent(appPath).FullName
        Dim modelsDir As String = appPath & Convert.ToString("models")
        modelsTextBox.Text = modelsDir
        ocrEngin = New OcrLite(modelsDir, CInt(numThreadNumeric.Value))
    End Sub

    Private Sub initBtn_Click(sender As Object, e As EventArgs) Handles initBtn.Click
        ocrEngin = New OcrLite(modelsTextBox.Text, CInt(numThreadNumeric.Value))
    End Sub

    Private Sub openBtn_Click(sender As Object, e As EventArgs) Handles openBtn.Click
        Using dlg = New OpenFileDialog()
            dlg.Multiselect = False
            dlg.Filter = "(*.JPG,*.PNG,*.JPEG,*.BMP,*.GIF)|*.JPG;*.PNG;*.JPEG;*.BMP;*.GIF|All files(*.*)|*.*"
            If dlg.ShowDialog() = DialogResult.OK AndAlso Not String.IsNullOrEmpty(dlg.FileName) Then
                pathTextBox.Text = dlg.FileName
                Dim src As Mat = CvInvoke.Imread(dlg.FileName, ImreadModes.Color)
                pictureBox.Image = src.ToBitmap()
            End If
        End Using
    End Sub

    Private Sub modelsBtn_Click(sender As Object, e As EventArgs) Handles modelsBtn.Click
        Using dlg = New FolderBrowserDialog()
            dlg.SelectedPath = Environment.CurrentDirectory + "\models"
            If dlg.ShowDialog() = DialogResult.OK AndAlso Not String.IsNullOrEmpty(dlg.SelectedPath) Then
                modelsTextBox.Text = dlg.SelectedPath
            End If
        End Using
    End Sub

    Private Sub detectBtn_Click(sender As Object, e As EventArgs) Handles detectBtn.Click
        Dim padding As Integer = CInt(paddingNumeric.Value)
        Dim imgResize As Integer = CInt(imgResizeNumeric.Value)
        Dim boxScoreThresh As Single = CSng(boxScoreThreshNumeric.Value)
        Dim boxThresh As Single = CSng(boxThreshNumeric.Value)
        Dim minArea As Single = CSng(minAreaNumeric.Value)
        Dim unClipRatio As Single = CSng(unClipRatioNumeric.Value)
        Dim doAngle As Boolean = doAngleCheckBox.Checked
        Dim mostAngle As Boolean = mostAngleCheckBox.Checked
        Dim ocrResult As OcrResult = ocrEngin.Detect(pathTextBox.Text, padding, imgResize, boxScoreThresh, boxThresh, minArea,
                unClipRatio, doAngle, mostAngle)
        ocrResultTextBox.Text = ocrResult.ToString()
        strRestTextBox.Text = ocrResult.StrRes
        pictureBox.Image = ocrResult.BoxImg.ToBitmap()
    End Sub

    Private Sub partImgCheckBox_CheckedChanged(sender As Object, e As EventArgs) Handles partImgCheckBox.CheckedChanged
        ocrEngin.isPartImg = partImgCheckBox.Checked
    End Sub

    Private Sub debugCheckBox_CheckedChanged(sender As Object, e As EventArgs) Handles debugCheckBox.CheckedChanged
        ocrEngin.isDebugImg = debugCheckBox.Checked
    End Sub
End Class
