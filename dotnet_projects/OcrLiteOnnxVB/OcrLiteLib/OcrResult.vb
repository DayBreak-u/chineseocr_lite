Imports System.Drawing
Imports System.Text
Imports Emgu.CV

Namespace OcrLiteLib
    Public Class TextBox
        Public Property Points As List(Of Point)
            Get
                Return m_Points
            End Get
            Set
                m_Points = value
            End Set
        End Property
        Private m_Points As List(Of Point)
        Public Property Score As Single
            Get
                Return m_Score
            End Get
            Set
                m_Score = Value
            End Set
        End Property
        Private m_Score As Single
        Public Sub New(ByVal points As List(Of Point), ByVal score As Single)
            m_Points = points
            m_Score = score
        End Sub
        Public Overrides Function ToString() As String
            Return $"TextBox[score({Score}),[x: {Points(0).X}, y: {Points(0).Y}], [x: {Points(1).X}, y: {Points(1).Y}], [x: {Points(2).X}, y : {Points(2).Y}], [x: {Points(3).X}, y: {Points(3).Y}]]"
        End Function

    End Class
    Public Class Angle
        Public Property Index As Integer
            Get
                Return m_Index
            End Get
            Set
                m_Index = Value
            End Set
        End Property
        Private m_Index As Integer
        Public Property Score As Single
            Get
                Return m_Score
            End Get
            Set
                m_Score = Value
            End Set
        End Property
        Private m_Score As Single
        Public Property Time As Single
            Get
                Return m_Time
            End Get
            Set
                m_Time = Value
            End Set
        End Property
        Private m_Time As Single
        Public Sub New(ByVal index As Integer, ByVal score As Single, ByVal time As Single)
            m_Index = index
            m_Score = score
            m_Time = time
        End Sub
        Public Sub New(ByVal index As Integer, ByVal score As Single)
            m_Index = index
            m_Score = score
            m_Time = 0
        End Sub
        Public Sub New()
            m_Index = -1
            m_Score = 0
            m_Time = 0
        End Sub
        Public Overrides Function ToString() As String
            Dim header As String = If(Index >= 0, "Angle", "AngleDisabled")
            Return $"{header}[Index({Index}), Score({Score}), Time({Time}ms)]"
        End Function
    End Class

    Public Class TextLine
        Public Property Text As String
            Get
                Return m_Text
            End Get
            Set
                m_Text = Value
            End Set
        End Property
        Private m_Text As String
        Public Property CharScores As List(Of Single)
            Get
                Return m_CharScores
            End Get
            Set
                m_CharScores = Value
            End Set
        End Property
        Private m_CharScores As List(Of Single)
        Public Property Time As Single
            Get
                Return m_Time
            End Get
            Set
                m_Time = Value
            End Set
        End Property
        Private m_Time As Single
        Public Sub New(ByVal text As String, ByVal charScores As List(Of Single), ByVal time As Single)
            m_Text = text
            m_CharScores = charScores
            m_Time = time
        End Sub

        Public Sub New()
        End Sub

        Public Overrides Function ToString() As String
            Dim sb As New StringBuilder()
            CharScores.ForEach(Sub(x) sb.Append($"{x},"))
            Return $"TextLine[Text({Text}),CharScores({sb.ToString()}),Time({Time}ms)]"
        End Function
    End Class

    Public Class TextBlock
        Public Property BoxPoints As List(Of Point)
            Get
                Return m_BoxPoints
            End Get
            Set
                m_BoxPoints = Value
            End Set
        End Property
        Private m_BoxPoints As List(Of Point)
        Public Property BoxScore As Single
            Get
                Return m_BoxScore
            End Get
            Set
                m_BoxScore = Value
            End Set
        End Property
        Private m_BoxScore As Single
        Public Property AngleIndex As Integer
            Get
                Return m_AngleIndex
            End Get
            Set
                m_AngleIndex = Value
            End Set
        End Property
        Private m_AngleIndex As Integer
        Public Property AngleScore As Single
            Get
                Return m_AngleScore
            End Get
            Set
                m_AngleScore = Value
            End Set
        End Property
        Private m_AngleScore As Single
        Public Property AngleTime As Single
            Get
                Return m_AngleTime
            End Get
            Set
                m_AngleTime = Value
            End Set
        End Property
        Private m_AngleTime As Single
        Public Property Text As String
            Get
                Return m_Text
            End Get
            Set
                m_Text = Value
            End Set
        End Property
        Private m_Text As String
        Public Property CharScores As List(Of Single)
            Get
                Return m_CharScores
            End Get
            Set
                m_CharScores = Value
            End Set
        End Property
        Private m_CharScores As List(Of Single)
        Public Property CrnnTime As Single
            Get
                Return m_CrnnTime
            End Get
            Set
                m_CrnnTime = Value
            End Set
        End Property
        Private m_CrnnTime As Single
        Public Property BlockTime As Single
            Get
                Return m_BlockTime
            End Get
            Set
                m_BlockTime = Value
            End Set
        End Property
        Private m_BlockTime As Single
        Public Sub New(ByVal boxPoints As List(Of Point), ByVal boxScore As Single, ByVal angleIndex As Integer, ByVal angleScore As Single, ByVal angleTime As Single, ByVal text As String, ByVal charScores As List(Of Single), ByVal crnnTime As Single, ByVal blockTime As Single)
            m_BoxPoints = boxPoints
            m_BoxScore = boxScore
            m_AngleIndex = angleIndex
            m_AngleScore = angleScore
            m_AngleTime = angleTime
            m_Text = text
            m_CharScores = charScores
            m_CrnnTime = crnnTime
            m_BlockTime = blockTime
        End Sub

        Public Sub New()
        End Sub

        Public Overrides Function ToString() As String
            Dim sb As New StringBuilder()
            sb.AppendLine("├─TextBlock")
            Dim textBox As String = $"│   ├──TextBox[score({BoxScore}),[x: {BoxPoints(0).X}, y: {BoxPoints(0).Y}], [x: {BoxPoints(1).X}, y: {BoxPoints(1).Y}], [x: {BoxPoints(2).X}, y: {BoxPoints(2).Y}], [x: {BoxPoints(3).X}, y: {BoxPoints(3).Y}]]"
            sb.AppendLine(textBox)
            Dim header As String = If(AngleIndex >= 0, "Angle", "AngleDisabled")
            Dim angle As String = $"│   ├──{header}[Index({AngleIndex}), Score({AngleScore}), Time({AngleTime}ms)]"
            sb.AppendLine(angle)
            Dim sbScores As New StringBuilder()
            CharScores.ForEach(Function(x) sbScores.Append($"{x},"))
            Dim textLine As String = $"│   ├──TextLine[Text({Text}),CharScores({sbScores.ToString()}),Time({CrnnTime}ms)]"
            sb.AppendLine(textLine)
            sb.AppendLine($"│   └──BlockTime({BlockTime}ms)")
            Return sb.ToString()
        End Function
    End Class

    Public NotInheritable Class OcrResult
        Public Property TextBlocks As List(Of TextBlock)
            Get
                Return m_TextBlocks
            End Get
            Set
                m_TextBlocks = Value
            End Set
        End Property
        Private m_TextBlocks As List(Of TextBlock)
        Public Property DbNetTime As Single
            Get
                Return m_DbNetTime
            End Get
            Set
                m_DbNetTime = Value
            End Set
        End Property
        Private m_DbNetTime As Single
        Public Property BoxImg As Mat
            Get
                Return m_BoxImg
            End Get
            Set
                m_BoxImg = Value
            End Set
        End Property
        Private m_BoxImg As Mat
        Public Property DetectTime As Single
            Get
                Return m_DetectTime
            End Get
            Set
                m_DetectTime = Value
            End Set
        End Property
        Private m_DetectTime As Single
        Public Property StrRes As String
            Get
                Return m_StrRes
            End Get
            Set
                m_StrRes = Value
            End Set
        End Property
        Private m_StrRes As String
        Public Sub New(ByVal dbNetTime As Single, ByVal boxImg As Mat, ByVal detectTime As Single, ByVal strRes As String)
            m_DbNetTime = dbNetTime
            m_BoxImg = boxImg
            m_DetectTime = detectTime
            m_StrRes = strRes
        End Sub

        Public Sub New()
        End Sub

        Public Overrides Function ToString() As String
            Dim sb As New StringBuilder()
            sb.AppendLine("OcrResult")
            TextBlocks.ForEach(Function(x) sb.Append(x))
            sb.AppendLine($"├─DbNetTime({DbNetTime}ms)")
            sb.AppendLine($"├─DetectTime({DetectTime}ms)")
            sb.AppendLine($"└─StrRes({StrRes})")
            Return sb.ToString()
        End Function
    End Class
End Namespace

