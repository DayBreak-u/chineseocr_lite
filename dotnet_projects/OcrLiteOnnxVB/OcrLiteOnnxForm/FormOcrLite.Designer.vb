<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class FormOcrLite
    Inherits System.Windows.Forms.Form

    'Form 重写 Dispose，以清理组件列表。
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Windows 窗体设计器所必需的
    Private components As System.ComponentModel.IContainer

    '注意: 以下过程是 Windows 窗体设计器所必需的
    '可以使用 Windows 窗体设计器修改它。  
    '不要使用代码编辑器修改它。
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.pictureBox = New System.Windows.Forms.PictureBox()
        Me.strRestTextBox = New System.Windows.Forms.TextBox()
        Me.pathTextBox = New System.Windows.Forms.TextBox()
        Me.boxScoreThreshNumeric = New System.Windows.Forms.NumericUpDown()
        Me.label4 = New System.Windows.Forms.Label()
        Me.detectBtn = New System.Windows.Forms.Button()
        Me.imgResizeNumeric = New System.Windows.Forms.NumericUpDown()
        Me.label3 = New System.Windows.Forms.Label()
        Me.label2 = New System.Windows.Forms.Label()
        Me.ocrResultTextBox = New System.Windows.Forms.TextBox()
        Me.label8 = New System.Windows.Forms.Label()
        Me.label9 = New System.Windows.Forms.Label()
        Me.label5 = New System.Windows.Forms.Label()
        Me.boxThreshNumeric = New System.Windows.Forms.NumericUpDown()
        Me.label6 = New System.Windows.Forms.Label()
        Me.tableLayoutPanel2 = New System.Windows.Forms.TableLayoutPanel()
        Me.modelsTextBox = New System.Windows.Forms.TextBox()
        Me.modelsBtn = New System.Windows.Forms.Button()
        Me.numThreadNumeric = New System.Windows.Forms.NumericUpDown()
        Me.label1 = New System.Windows.Forms.Label()
        Me.openBtn = New System.Windows.Forms.Button()
        Me.initBtn = New System.Windows.Forms.Button()
        Me.paddingNumeric = New System.Windows.Forms.NumericUpDown()
        Me.tableLayoutPanel3 = New System.Windows.Forms.TableLayoutPanel()
        Me.tableLayoutPanel1 = New System.Windows.Forms.TableLayoutPanel()
        Me.minAreaNumeric = New System.Windows.Forms.NumericUpDown()
        Me.label7 = New System.Windows.Forms.Label()
        Me.unClipRatioNumeric = New System.Windows.Forms.NumericUpDown()
        Me.doAngleCheckBox = New System.Windows.Forms.CheckBox()
        Me.mostAngleCheckBox = New System.Windows.Forms.CheckBox()
        Me.partImgCheckBox = New System.Windows.Forms.CheckBox()
        Me.debugCheckBox = New System.Windows.Forms.CheckBox()
        CType(Me.pictureBox, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.boxScoreThreshNumeric, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.imgResizeNumeric, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.boxThreshNumeric, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.tableLayoutPanel2.SuspendLayout()
        CType(Me.numThreadNumeric, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.paddingNumeric, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.tableLayoutPanel3.SuspendLayout()
        Me.tableLayoutPanel1.SuspendLayout()
        CType(Me.minAreaNumeric, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.unClipRatioNumeric, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'pictureBox
        '
        Me.pictureBox.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.pictureBox.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.pictureBox.Location = New System.Drawing.Point(173, 3)
        Me.pictureBox.Name = "pictureBox"
        Me.pictureBox.Size = New System.Drawing.Size(289, 260)
        Me.pictureBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom
        Me.pictureBox.TabIndex = 3
        Me.pictureBox.TabStop = False
        '
        'strRestTextBox
        '
        Me.strRestTextBox.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.strRestTextBox.Location = New System.Drawing.Point(468, 3)
        Me.strRestTextBox.Multiline = True
        Me.strRestTextBox.Name = "strRestTextBox"
        Me.strRestTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical
        Me.strRestTextBox.Size = New System.Drawing.Size(289, 260)
        Me.strRestTextBox.TabIndex = 0
        '
        'pathTextBox
        '
        Me.pathTextBox.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.pathTextBox.Location = New System.Drawing.Point(103, 63)
        Me.pathTextBox.Name = "pathTextBox"
        Me.pathTextBox.Size = New System.Drawing.Size(494, 21)
        Me.pathTextBox.TabIndex = 6
        '
        'boxScoreThreshNumeric
        '
        Me.boxScoreThreshNumeric.DecimalPlaces = 3
        Me.boxScoreThreshNumeric.Increment = New Decimal(New Integer() {1, 0, 0, 65536})
        Me.boxScoreThreshNumeric.Location = New System.Drawing.Point(87, 69)
        Me.boxScoreThreshNumeric.Maximum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.boxScoreThreshNumeric.Minimum = New Decimal(New Integer() {1, 0, 0, 131072})
        Me.boxScoreThreshNumeric.Name = "boxScoreThreshNumeric"
        Me.boxScoreThreshNumeric.Size = New System.Drawing.Size(74, 21)
        Me.boxScoreThreshNumeric.TabIndex = 13
        Me.boxScoreThreshNumeric.Value = New Decimal(New Integer() {618, 0, 0, 196608})
        '
        'label4
        '
        Me.label4.AutoSize = True
        Me.label4.Location = New System.Drawing.Point(5, 66)
        Me.label4.Name = "label4"
        Me.label4.Size = New System.Drawing.Size(71, 24)
        Me.label4.TabIndex = 12
        Me.label4.Text = "boxScoreThresh"
        '
        'detectBtn
        '
        Me.detectBtn.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.detectBtn.Location = New System.Drawing.Point(698, 12)
        Me.detectBtn.Name = "detectBtn"
        Me.detectBtn.Size = New System.Drawing.Size(74, 90)
        Me.detectBtn.TabIndex = 12
        Me.detectBtn.Text = "Detect"
        Me.detectBtn.UseVisualStyleBackColor = True
        '
        'imgResizeNumeric
        '
        Me.imgResizeNumeric.Location = New System.Drawing.Point(87, 37)
        Me.imgResizeNumeric.Maximum = New Decimal(New Integer() {99999, 0, 0, 0})
        Me.imgResizeNumeric.Name = "imgResizeNumeric"
        Me.imgResizeNumeric.Size = New System.Drawing.Size(74, 21)
        Me.imgResizeNumeric.TabIndex = 10
        '
        'label3
        '
        Me.label3.AutoSize = True
        Me.label3.Location = New System.Drawing.Point(5, 34)
        Me.label3.Name = "label3"
        Me.label3.Size = New System.Drawing.Size(59, 12)
        Me.label3.TabIndex = 9
        Me.label3.Text = "imgResize"
        '
        'label2
        '
        Me.label2.AutoSize = True
        Me.label2.Location = New System.Drawing.Point(5, 2)
        Me.label2.Name = "label2"
        Me.label2.Size = New System.Drawing.Size(47, 12)
        Me.label2.TabIndex = 6
        Me.label2.Text = "padding"
        '
        'ocrResultTextBox
        '
        Me.ocrResultTextBox.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.ocrResultTextBox.Font = New System.Drawing.Font("宋体", 9.0!)
        Me.ocrResultTextBox.Location = New System.Drawing.Point(12, 382)
        Me.ocrResultTextBox.Multiline = True
        Me.ocrResultTextBox.Name = "ocrResultTextBox"
        Me.ocrResultTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical
        Me.ocrResultTextBox.Size = New System.Drawing.Size(757, 367)
        Me.ocrResultTextBox.TabIndex = 13
        '
        'label8
        '
        Me.label8.AutoSize = True
        Me.label8.Location = New System.Drawing.Point(3, 60)
        Me.label8.Name = "label8"
        Me.label8.Size = New System.Drawing.Size(47, 12)
        Me.label8.TabIndex = 8
        Me.label8.Text = "ImgFile"
        '
        'label9
        '
        Me.label9.AutoSize = True
        Me.label9.Location = New System.Drawing.Point(3, 0)
        Me.label9.Name = "label9"
        Me.label9.Size = New System.Drawing.Size(41, 12)
        Me.label9.TabIndex = 9
        Me.label9.Text = "Models"
        '
        'label5
        '
        Me.label5.AutoSize = True
        Me.label5.Location = New System.Drawing.Point(5, 98)
        Me.label5.Name = "label5"
        Me.label5.Size = New System.Drawing.Size(59, 12)
        Me.label5.TabIndex = 14
        Me.label5.Text = "boxThresh"
        '
        'boxThreshNumeric
        '
        Me.boxThreshNumeric.DecimalPlaces = 3
        Me.boxThreshNumeric.Increment = New Decimal(New Integer() {1, 0, 0, 65536})
        Me.boxThreshNumeric.Location = New System.Drawing.Point(87, 101)
        Me.boxThreshNumeric.Maximum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.boxThreshNumeric.Minimum = New Decimal(New Integer() {1, 0, 0, 131072})
        Me.boxThreshNumeric.Name = "boxThreshNumeric"
        Me.boxThreshNumeric.Size = New System.Drawing.Size(74, 21)
        Me.boxThreshNumeric.TabIndex = 15
        Me.boxThreshNumeric.Value = New Decimal(New Integer() {3, 0, 0, 65536})
        '
        'label6
        '
        Me.label6.AutoSize = True
        Me.label6.Location = New System.Drawing.Point(5, 130)
        Me.label6.Name = "label6"
        Me.label6.Size = New System.Drawing.Size(47, 12)
        Me.label6.TabIndex = 18
        Me.label6.Text = "minArea"
        '
        'tableLayoutPanel2
        '
        Me.tableLayoutPanel2.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.tableLayoutPanel2.ColumnCount = 3
        Me.tableLayoutPanel2.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 100.0!))
        Me.tableLayoutPanel2.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.tableLayoutPanel2.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 80.0!))
        Me.tableLayoutPanel2.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 20.0!))
        Me.tableLayoutPanel2.Controls.Add(Me.modelsTextBox, 1, 0)
        Me.tableLayoutPanel2.Controls.Add(Me.label8, 0, 2)
        Me.tableLayoutPanel2.Controls.Add(Me.label9, 0, 0)
        Me.tableLayoutPanel2.Controls.Add(Me.pathTextBox, 1, 2)
        Me.tableLayoutPanel2.Controls.Add(Me.modelsBtn, 2, 0)
        Me.tableLayoutPanel2.Controls.Add(Me.numThreadNumeric, 1, 1)
        Me.tableLayoutPanel2.Controls.Add(Me.label1, 0, 1)
        Me.tableLayoutPanel2.Controls.Add(Me.openBtn, 2, 2)
        Me.tableLayoutPanel2.Controls.Add(Me.initBtn, 2, 1)
        Me.tableLayoutPanel2.Location = New System.Drawing.Point(12, 12)
        Me.tableLayoutPanel2.Name = "tableLayoutPanel2"
        Me.tableLayoutPanel2.RowCount = 3
        Me.tableLayoutPanel2.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30.0!))
        Me.tableLayoutPanel2.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30.0!))
        Me.tableLayoutPanel2.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30.0!))
        Me.tableLayoutPanel2.Size = New System.Drawing.Size(680, 90)
        Me.tableLayoutPanel2.TabIndex = 14
        '
        'modelsTextBox
        '
        Me.modelsTextBox.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.modelsTextBox.Location = New System.Drawing.Point(103, 3)
        Me.modelsTextBox.Name = "modelsTextBox"
        Me.modelsTextBox.Size = New System.Drawing.Size(494, 21)
        Me.modelsTextBox.TabIndex = 1
        '
        'modelsBtn
        '
        Me.modelsBtn.Location = New System.Drawing.Point(603, 3)
        Me.modelsBtn.Name = "modelsBtn"
        Me.modelsBtn.Size = New System.Drawing.Size(74, 23)
        Me.modelsBtn.TabIndex = 2
        Me.modelsBtn.Text = "Models"
        Me.modelsBtn.UseVisualStyleBackColor = True
        '
        'numThreadNumeric
        '
        Me.numThreadNumeric.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.numThreadNumeric.Location = New System.Drawing.Point(103, 33)
        Me.numThreadNumeric.Maximum = New Decimal(New Integer() {128, 0, 0, 0})
        Me.numThreadNumeric.Minimum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.numThreadNumeric.Name = "numThreadNumeric"
        Me.numThreadNumeric.Size = New System.Drawing.Size(494, 21)
        Me.numThreadNumeric.TabIndex = 4
        Me.numThreadNumeric.Value = New Decimal(New Integer() {4, 0, 0, 0})
        '
        'label1
        '
        Me.label1.AutoSize = True
        Me.label1.Location = New System.Drawing.Point(3, 30)
        Me.label1.Name = "label1"
        Me.label1.Size = New System.Drawing.Size(59, 12)
        Me.label1.TabIndex = 3
        Me.label1.Text = "numThread"
        '
        'openBtn
        '
        Me.openBtn.Location = New System.Drawing.Point(603, 63)
        Me.openBtn.Name = "openBtn"
        Me.openBtn.Size = New System.Drawing.Size(74, 23)
        Me.openBtn.TabIndex = 0
        Me.openBtn.Text = "Open"
        Me.openBtn.UseVisualStyleBackColor = True
        '
        'initBtn
        '
        Me.initBtn.Location = New System.Drawing.Point(603, 33)
        Me.initBtn.Name = "initBtn"
        Me.initBtn.Size = New System.Drawing.Size(74, 23)
        Me.initBtn.TabIndex = 10
        Me.initBtn.Text = "重新初始化"
        Me.initBtn.UseVisualStyleBackColor = True
        '
        'paddingNumeric
        '
        Me.paddingNumeric.Location = New System.Drawing.Point(87, 5)
        Me.paddingNumeric.Maximum = New Decimal(New Integer() {200, 0, 0, 0})
        Me.paddingNumeric.Name = "paddingNumeric"
        Me.paddingNumeric.Size = New System.Drawing.Size(74, 21)
        Me.paddingNumeric.TabIndex = 7
        Me.paddingNumeric.Value = New Decimal(New Integer() {50, 0, 0, 0})
        '
        'tableLayoutPanel3
        '
        Me.tableLayoutPanel3.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.tableLayoutPanel3.ColumnCount = 3
        Me.tableLayoutPanel3.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 170.0!))
        Me.tableLayoutPanel3.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50.0!))
        Me.tableLayoutPanel3.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50.0!))
        Me.tableLayoutPanel3.Controls.Add(Me.pictureBox, 1, 0)
        Me.tableLayoutPanel3.Controls.Add(Me.strRestTextBox, 2, 0)
        Me.tableLayoutPanel3.Controls.Add(Me.tableLayoutPanel1, 0, 0)
        Me.tableLayoutPanel3.Location = New System.Drawing.Point(12, 108)
        Me.tableLayoutPanel3.Name = "tableLayoutPanel3"
        Me.tableLayoutPanel3.RowCount = 1
        Me.tableLayoutPanel3.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100.0!))
        Me.tableLayoutPanel3.Size = New System.Drawing.Size(760, 268)
        Me.tableLayoutPanel3.TabIndex = 15
        '
        'tableLayoutPanel1
        '
        Me.tableLayoutPanel1.CellBorderStyle = System.Windows.Forms.TableLayoutPanelCellBorderStyle.Outset
        Me.tableLayoutPanel1.ColumnCount = 2
        Me.tableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 80.0!))
        Me.tableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 88.0!))
        Me.tableLayoutPanel1.Controls.Add(Me.boxScoreThreshNumeric, 1, 2)
        Me.tableLayoutPanel1.Controls.Add(Me.label4, 0, 2)
        Me.tableLayoutPanel1.Controls.Add(Me.imgResizeNumeric, 1, 1)
        Me.tableLayoutPanel1.Controls.Add(Me.label3, 0, 1)
        Me.tableLayoutPanel1.Controls.Add(Me.paddingNumeric, 1, 0)
        Me.tableLayoutPanel1.Controls.Add(Me.label2, 0, 0)
        Me.tableLayoutPanel1.Controls.Add(Me.label5, 0, 3)
        Me.tableLayoutPanel1.Controls.Add(Me.boxThreshNumeric, 1, 3)
        Me.tableLayoutPanel1.Controls.Add(Me.label6, 0, 4)
        Me.tableLayoutPanel1.Controls.Add(Me.minAreaNumeric, 1, 4)
        Me.tableLayoutPanel1.Controls.Add(Me.label7, 0, 5)
        Me.tableLayoutPanel1.Controls.Add(Me.unClipRatioNumeric, 1, 5)
        Me.tableLayoutPanel1.Controls.Add(Me.doAngleCheckBox, 0, 6)
        Me.tableLayoutPanel1.Controls.Add(Me.mostAngleCheckBox, 1, 6)
        Me.tableLayoutPanel1.Controls.Add(Me.partImgCheckBox, 0, 7)
        Me.tableLayoutPanel1.Controls.Add(Me.debugCheckBox, 1, 7)
        Me.tableLayoutPanel1.Location = New System.Drawing.Point(3, 3)
        Me.tableLayoutPanel1.Name = "tableLayoutPanel1"
        Me.tableLayoutPanel1.RowCount = 8
        Me.tableLayoutPanel1.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30.0!))
        Me.tableLayoutPanel1.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30.0!))
        Me.tableLayoutPanel1.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30.0!))
        Me.tableLayoutPanel1.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30.0!))
        Me.tableLayoutPanel1.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30.0!))
        Me.tableLayoutPanel1.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30.0!))
        Me.tableLayoutPanel1.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30.0!))
        Me.tableLayoutPanel1.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30.0!))
        Me.tableLayoutPanel1.Size = New System.Drawing.Size(164, 260)
        Me.tableLayoutPanel1.TabIndex = 2
        '
        'minAreaNumeric
        '
        Me.minAreaNumeric.DecimalPlaces = 1
        Me.minAreaNumeric.Increment = New Decimal(New Integer() {1, 0, 0, 65536})
        Me.minAreaNumeric.Location = New System.Drawing.Point(87, 133)
        Me.minAreaNumeric.Maximum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.minAreaNumeric.Minimum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.minAreaNumeric.Name = "minAreaNumeric"
        Me.minAreaNumeric.Size = New System.Drawing.Size(74, 21)
        Me.minAreaNumeric.TabIndex = 19
        Me.minAreaNumeric.Value = New Decimal(New Integer() {3, 0, 0, 0})
        '
        'label7
        '
        Me.label7.AutoSize = True
        Me.label7.Location = New System.Drawing.Point(5, 162)
        Me.label7.Name = "label7"
        Me.label7.Size = New System.Drawing.Size(71, 12)
        Me.label7.TabIndex = 21
        Me.label7.Text = "unClipRatio"
        '
        'unClipRatioNumeric
        '
        Me.unClipRatioNumeric.DecimalPlaces = 1
        Me.unClipRatioNumeric.Increment = New Decimal(New Integer() {1, 0, 0, 65536})
        Me.unClipRatioNumeric.Location = New System.Drawing.Point(87, 165)
        Me.unClipRatioNumeric.Maximum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.unClipRatioNumeric.Minimum = New Decimal(New Integer() {1, 0, 0, 65536})
        Me.unClipRatioNumeric.Name = "unClipRatioNumeric"
        Me.unClipRatioNumeric.Size = New System.Drawing.Size(74, 21)
        Me.unClipRatioNumeric.TabIndex = 22
        Me.unClipRatioNumeric.Value = New Decimal(New Integer() {18, 0, 0, 65536})
        '
        'doAngleCheckBox
        '
        Me.doAngleCheckBox.AutoSize = True
        Me.doAngleCheckBox.Checked = True
        Me.doAngleCheckBox.CheckState = System.Windows.Forms.CheckState.Checked
        Me.doAngleCheckBox.Location = New System.Drawing.Point(5, 197)
        Me.doAngleCheckBox.Name = "doAngleCheckBox"
        Me.doAngleCheckBox.Size = New System.Drawing.Size(66, 16)
        Me.doAngleCheckBox.TabIndex = 26
        Me.doAngleCheckBox.Text = "doAngle"
        Me.doAngleCheckBox.UseVisualStyleBackColor = True
        '
        'mostAngleCheckBox
        '
        Me.mostAngleCheckBox.AutoSize = True
        Me.mostAngleCheckBox.Checked = True
        Me.mostAngleCheckBox.CheckState = System.Windows.Forms.CheckState.Checked
        Me.mostAngleCheckBox.Location = New System.Drawing.Point(87, 197)
        Me.mostAngleCheckBox.Name = "mostAngleCheckBox"
        Me.mostAngleCheckBox.Size = New System.Drawing.Size(78, 16)
        Me.mostAngleCheckBox.TabIndex = 27
        Me.mostAngleCheckBox.Text = "mostAngle"
        Me.mostAngleCheckBox.UseVisualStyleBackColor = True
        '
        'partImgCheckBox
        '
        Me.partImgCheckBox.AutoSize = True
        Me.partImgCheckBox.Location = New System.Drawing.Point(5, 229)
        Me.partImgCheckBox.Name = "partImgCheckBox"
        Me.partImgCheckBox.Size = New System.Drawing.Size(66, 16)
        Me.partImgCheckBox.TabIndex = 28
        Me.partImgCheckBox.Text = "PartImg"
        Me.partImgCheckBox.UseVisualStyleBackColor = True
        '
        'debugCheckBox
        '
        Me.debugCheckBox.AutoSize = True
        Me.debugCheckBox.Location = New System.Drawing.Point(87, 229)
        Me.debugCheckBox.Name = "debugCheckBox"
        Me.debugCheckBox.Size = New System.Drawing.Size(72, 16)
        Me.debugCheckBox.TabIndex = 29
        Me.debugCheckBox.Text = "DebugImg"
        Me.debugCheckBox.UseVisualStyleBackColor = True
        '
        'FormOcrLite
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(784, 761)
        Me.Controls.Add(Me.detectBtn)
        Me.Controls.Add(Me.ocrResultTextBox)
        Me.Controls.Add(Me.tableLayoutPanel2)
        Me.Controls.Add(Me.tableLayoutPanel3)
        Me.Name = "FormOcrLite"
        Me.Text = "FormOcrLite"
        CType(Me.pictureBox, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.boxScoreThreshNumeric, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.imgResizeNumeric, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.boxThreshNumeric, System.ComponentModel.ISupportInitialize).EndInit()
        Me.tableLayoutPanel2.ResumeLayout(False)
        Me.tableLayoutPanel2.PerformLayout()
        CType(Me.numThreadNumeric, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.paddingNumeric, System.ComponentModel.ISupportInitialize).EndInit()
        Me.tableLayoutPanel3.ResumeLayout(False)
        Me.tableLayoutPanel3.PerformLayout()
        Me.tableLayoutPanel1.ResumeLayout(False)
        Me.tableLayoutPanel1.PerformLayout()
        CType(Me.minAreaNumeric, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.unClipRatioNumeric, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub

    Private WithEvents pictureBox As PictureBox
    Private WithEvents strRestTextBox As TextBox
    Private WithEvents pathTextBox As TextBox
    Private WithEvents boxScoreThreshNumeric As NumericUpDown
    Private WithEvents label4 As Label
    Private WithEvents detectBtn As Button
    Private WithEvents imgResizeNumeric As NumericUpDown
    Private WithEvents label3 As Label
    Private WithEvents label2 As Label
    Private WithEvents ocrResultTextBox As TextBox
    Private WithEvents label8 As Label
    Private WithEvents label9 As Label
    Private WithEvents label5 As Label
    Private WithEvents boxThreshNumeric As NumericUpDown
    Private WithEvents label6 As Label
    Private WithEvents tableLayoutPanel2 As TableLayoutPanel
    Private WithEvents modelsTextBox As TextBox
    Private WithEvents modelsBtn As Button
    Private WithEvents numThreadNumeric As NumericUpDown
    Private WithEvents label1 As Label
    Private WithEvents openBtn As Button
    Private WithEvents initBtn As Button
    Private WithEvents paddingNumeric As NumericUpDown
    Private WithEvents tableLayoutPanel3 As TableLayoutPanel
    Private WithEvents tableLayoutPanel1 As TableLayoutPanel
    Private WithEvents minAreaNumeric As NumericUpDown
    Private WithEvents label7 As Label
    Private WithEvents unClipRatioNumeric As NumericUpDown
    Private WithEvents doAngleCheckBox As CheckBox
    Private WithEvents mostAngleCheckBox As CheckBox
    Private WithEvents partImgCheckBox As CheckBox
    Private WithEvents debugCheckBox As CheckBox
End Class
