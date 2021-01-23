namespace OcrLiteOnnxCs
{
    partial class FormOcrLite
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要修改
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.openBtn = new System.Windows.Forms.Button();
            this.modelsTextBox = new System.Windows.Forms.TextBox();
            this.modelsBtn = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.numThreadNumeric = new System.Windows.Forms.NumericUpDown();
            this.label2 = new System.Windows.Forms.Label();
            this.paddingNumeric = new System.Windows.Forms.NumericUpDown();
            this.label3 = new System.Windows.Forms.Label();
            this.imgResizeNumeric = new System.Windows.Forms.NumericUpDown();
            this.label4 = new System.Windows.Forms.Label();
            this.boxScoreThreshNumeric = new System.Windows.Forms.NumericUpDown();
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.label5 = new System.Windows.Forms.Label();
            this.boxThreshNumeric = new System.Windows.Forms.NumericUpDown();
            this.label7 = new System.Windows.Forms.Label();
            this.unClipRatioNumeric = new System.Windows.Forms.NumericUpDown();
            this.doAngleCheckBox = new System.Windows.Forms.CheckBox();
            this.mostAngleCheckBox = new System.Windows.Forms.CheckBox();
            this.partImgCheckBox = new System.Windows.Forms.CheckBox();
            this.debugCheckBox = new System.Windows.Forms.CheckBox();
            this.pictureBox = new System.Windows.Forms.PictureBox();
            this.detectBtn = new System.Windows.Forms.Button();
            this.strRestTextBox = new System.Windows.Forms.TextBox();
            this.pathTextBox = new System.Windows.Forms.TextBox();
            this.ocrResultTextBox = new System.Windows.Forms.TextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.tableLayoutPanel2 = new System.Windows.Forms.TableLayoutPanel();
            this.initBtn = new System.Windows.Forms.Button();
            this.tableLayoutPanel3 = new System.Windows.Forms.TableLayoutPanel();
            this.label6 = new System.Windows.Forms.Label();
            this.detNameTextBox = new System.Windows.Forms.TextBox();
            this.label10 = new System.Windows.Forms.Label();
            this.clsNameTextBox = new System.Windows.Forms.TextBox();
            this.label11 = new System.Windows.Forms.Label();
            this.recNameTextBox = new System.Windows.Forms.TextBox();
            this.label12 = new System.Windows.Forms.Label();
            this.keysNameTextBox = new System.Windows.Forms.TextBox();
            ((System.ComponentModel.ISupportInitialize)(this.numThreadNumeric)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.paddingNumeric)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.imgResizeNumeric)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.boxScoreThreshNumeric)).BeginInit();
            this.tableLayoutPanel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.boxThreshNumeric)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.unClipRatioNumeric)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox)).BeginInit();
            this.tableLayoutPanel2.SuspendLayout();
            this.tableLayoutPanel3.SuspendLayout();
            this.SuspendLayout();
            // 
            // openBtn
            // 
            this.openBtn.Location = new System.Drawing.Point(598, 183);
            this.openBtn.Name = "openBtn";
            this.openBtn.Size = new System.Drawing.Size(74, 23);
            this.openBtn.TabIndex = 0;
            this.openBtn.Text = "Open";
            this.openBtn.UseVisualStyleBackColor = true;
            this.openBtn.Click += new System.EventHandler(this.openBtn_Click);
            // 
            // modelsTextBox
            // 
            this.modelsTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.modelsTextBox.Location = new System.Drawing.Point(103, 3);
            this.modelsTextBox.Name = "modelsTextBox";
            this.modelsTextBox.Size = new System.Drawing.Size(489, 21);
            this.modelsTextBox.TabIndex = 1;
            // 
            // modelsBtn
            // 
            this.modelsBtn.Location = new System.Drawing.Point(598, 3);
            this.modelsBtn.Name = "modelsBtn";
            this.modelsBtn.Size = new System.Drawing.Size(74, 23);
            this.modelsBtn.TabIndex = 2;
            this.modelsBtn.Text = "Models";
            this.modelsBtn.UseVisualStyleBackColor = true;
            this.modelsBtn.Click += new System.EventHandler(this.modelsBtn_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(3, 150);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(59, 12);
            this.label1.TabIndex = 3;
            this.label1.Text = "numThread";
            // 
            // numThreadNumeric
            // 
            this.numThreadNumeric.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.numThreadNumeric.Location = new System.Drawing.Point(103, 153);
            this.numThreadNumeric.Maximum = new decimal(new int[] {
            128,
            0,
            0,
            0});
            this.numThreadNumeric.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numThreadNumeric.Name = "numThreadNumeric";
            this.numThreadNumeric.Size = new System.Drawing.Size(489, 21);
            this.numThreadNumeric.TabIndex = 4;
            this.numThreadNumeric.Value = new decimal(new int[] {
            4,
            0,
            0,
            0});
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(5, 2);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(47, 12);
            this.label2.TabIndex = 6;
            this.label2.Text = "padding";
            // 
            // paddingNumeric
            // 
            this.paddingNumeric.Location = new System.Drawing.Point(87, 5);
            this.paddingNumeric.Maximum = new decimal(new int[] {
            200,
            0,
            0,
            0});
            this.paddingNumeric.Name = "paddingNumeric";
            this.paddingNumeric.Size = new System.Drawing.Size(74, 21);
            this.paddingNumeric.TabIndex = 7;
            this.paddingNumeric.Value = new decimal(new int[] {
            50,
            0,
            0,
            0});
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(5, 34);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(65, 12);
            this.label3.TabIndex = 9;
            this.label3.Text = "maxSideLen";
            // 
            // imgResizeNumeric
            // 
            this.imgResizeNumeric.Location = new System.Drawing.Point(87, 37);
            this.imgResizeNumeric.Maximum = new decimal(new int[] {
            99999,
            0,
            0,
            0});
            this.imgResizeNumeric.Name = "imgResizeNumeric";
            this.imgResizeNumeric.Size = new System.Drawing.Size(74, 21);
            this.imgResizeNumeric.TabIndex = 10;
            this.imgResizeNumeric.Value = new decimal(new int[] {
            1024,
            0,
            0,
            0});
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(5, 66);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(71, 24);
            this.label4.TabIndex = 12;
            this.label4.Text = "boxScoreThresh";
            // 
            // boxScoreThreshNumeric
            // 
            this.boxScoreThreshNumeric.DecimalPlaces = 3;
            this.boxScoreThreshNumeric.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.boxScoreThreshNumeric.Location = new System.Drawing.Point(87, 69);
            this.boxScoreThreshNumeric.Maximum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.boxScoreThreshNumeric.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            131072});
            this.boxScoreThreshNumeric.Name = "boxScoreThreshNumeric";
            this.boxScoreThreshNumeric.Size = new System.Drawing.Size(74, 21);
            this.boxScoreThreshNumeric.TabIndex = 13;
            this.boxScoreThreshNumeric.Value = new decimal(new int[] {
            618,
            0,
            0,
            196608});
            // 
            // tableLayoutPanel1
            // 
            this.tableLayoutPanel1.CellBorderStyle = System.Windows.Forms.TableLayoutPanelCellBorderStyle.Outset;
            this.tableLayoutPanel1.ColumnCount = 2;
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 80F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 86F));
            this.tableLayoutPanel1.Controls.Add(this.boxScoreThreshNumeric, 1, 2);
            this.tableLayoutPanel1.Controls.Add(this.label4, 0, 2);
            this.tableLayoutPanel1.Controls.Add(this.imgResizeNumeric, 1, 1);
            this.tableLayoutPanel1.Controls.Add(this.label3, 0, 1);
            this.tableLayoutPanel1.Controls.Add(this.paddingNumeric, 1, 0);
            this.tableLayoutPanel1.Controls.Add(this.label2, 0, 0);
            this.tableLayoutPanel1.Controls.Add(this.label5, 0, 3);
            this.tableLayoutPanel1.Controls.Add(this.boxThreshNumeric, 1, 3);
            this.tableLayoutPanel1.Controls.Add(this.label7, 0, 4);
            this.tableLayoutPanel1.Controls.Add(this.unClipRatioNumeric, 1, 4);
            this.tableLayoutPanel1.Controls.Add(this.doAngleCheckBox, 0, 5);
            this.tableLayoutPanel1.Controls.Add(this.mostAngleCheckBox, 1, 5);
            this.tableLayoutPanel1.Controls.Add(this.partImgCheckBox, 0, 6);
            this.tableLayoutPanel1.Controls.Add(this.debugCheckBox, 1, 6);
            this.tableLayoutPanel1.Location = new System.Drawing.Point(3, 3);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.RowCount = 8;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30F));
            this.tableLayoutPanel1.Size = new System.Drawing.Size(164, 260);
            this.tableLayoutPanel1.TabIndex = 2;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(5, 98);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(59, 12);
            this.label5.TabIndex = 14;
            this.label5.Text = "boxThresh";
            // 
            // boxThreshNumeric
            // 
            this.boxThreshNumeric.DecimalPlaces = 3;
            this.boxThreshNumeric.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.boxThreshNumeric.Location = new System.Drawing.Point(87, 101);
            this.boxThreshNumeric.Maximum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.boxThreshNumeric.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            131072});
            this.boxThreshNumeric.Name = "boxThreshNumeric";
            this.boxThreshNumeric.Size = new System.Drawing.Size(74, 21);
            this.boxThreshNumeric.TabIndex = 15;
            this.boxThreshNumeric.Value = new decimal(new int[] {
            3,
            0,
            0,
            65536});
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(5, 130);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(71, 12);
            this.label7.TabIndex = 21;
            this.label7.Text = "unClipRatio";
            // 
            // unClipRatioNumeric
            // 
            this.unClipRatioNumeric.DecimalPlaces = 1;
            this.unClipRatioNumeric.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.unClipRatioNumeric.Location = new System.Drawing.Point(87, 133);
            this.unClipRatioNumeric.Maximum = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.unClipRatioNumeric.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.unClipRatioNumeric.Name = "unClipRatioNumeric";
            this.unClipRatioNumeric.Size = new System.Drawing.Size(74, 21);
            this.unClipRatioNumeric.TabIndex = 22;
            this.unClipRatioNumeric.Value = new decimal(new int[] {
            20,
            0,
            0,
            65536});
            // 
            // doAngleCheckBox
            // 
            this.doAngleCheckBox.AutoSize = true;
            this.doAngleCheckBox.Checked = true;
            this.doAngleCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.doAngleCheckBox.Location = new System.Drawing.Point(5, 165);
            this.doAngleCheckBox.Name = "doAngleCheckBox";
            this.doAngleCheckBox.Size = new System.Drawing.Size(66, 16);
            this.doAngleCheckBox.TabIndex = 26;
            this.doAngleCheckBox.Text = "doAngle";
            this.doAngleCheckBox.UseVisualStyleBackColor = true;
            // 
            // mostAngleCheckBox
            // 
            this.mostAngleCheckBox.AutoSize = true;
            this.mostAngleCheckBox.Checked = true;
            this.mostAngleCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.mostAngleCheckBox.Location = new System.Drawing.Point(87, 165);
            this.mostAngleCheckBox.Name = "mostAngleCheckBox";
            this.mostAngleCheckBox.Size = new System.Drawing.Size(78, 16);
            this.mostAngleCheckBox.TabIndex = 27;
            this.mostAngleCheckBox.Text = "mostAngle";
            this.mostAngleCheckBox.UseVisualStyleBackColor = true;
            // 
            // partImgCheckBox
            // 
            this.partImgCheckBox.AutoSize = true;
            this.partImgCheckBox.Location = new System.Drawing.Point(5, 197);
            this.partImgCheckBox.Name = "partImgCheckBox";
            this.partImgCheckBox.Size = new System.Drawing.Size(66, 16);
            this.partImgCheckBox.TabIndex = 28;
            this.partImgCheckBox.Text = "PartImg";
            this.partImgCheckBox.UseVisualStyleBackColor = true;
            this.partImgCheckBox.CheckedChanged += new System.EventHandler(this.partImgCheckBox_CheckedChanged);
            // 
            // debugCheckBox
            // 
            this.debugCheckBox.AutoSize = true;
            this.debugCheckBox.Location = new System.Drawing.Point(87, 197);
            this.debugCheckBox.Name = "debugCheckBox";
            this.debugCheckBox.Size = new System.Drawing.Size(72, 16);
            this.debugCheckBox.TabIndex = 29;
            this.debugCheckBox.Text = "DebugImg";
            this.debugCheckBox.UseVisualStyleBackColor = true;
            this.debugCheckBox.CheckedChanged += new System.EventHandler(this.debugCheckBox_CheckedChanged);
            // 
            // pictureBox
            // 
            this.pictureBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.pictureBox.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pictureBox.Location = new System.Drawing.Point(173, 3);
            this.pictureBox.Name = "pictureBox";
            this.pictureBox.Size = new System.Drawing.Size(287, 260);
            this.pictureBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictureBox.TabIndex = 3;
            this.pictureBox.TabStop = false;
            // 
            // detectBtn
            // 
            this.detectBtn.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.detectBtn.Location = new System.Drawing.Point(695, 12);
            this.detectBtn.Name = "detectBtn";
            this.detectBtn.Size = new System.Drawing.Size(74, 114);
            this.detectBtn.TabIndex = 4;
            this.detectBtn.Text = "Detect";
            this.detectBtn.UseVisualStyleBackColor = true;
            this.detectBtn.Click += new System.EventHandler(this.detectBtn_Click);
            // 
            // strRestTextBox
            // 
            this.strRestTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.strRestTextBox.Location = new System.Drawing.Point(466, 3);
            this.strRestTextBox.Multiline = true;
            this.strRestTextBox.Name = "strRestTextBox";
            this.strRestTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.strRestTextBox.Size = new System.Drawing.Size(288, 260);
            this.strRestTextBox.TabIndex = 0;
            // 
            // pathTextBox
            // 
            this.pathTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.pathTextBox.Location = new System.Drawing.Point(103, 183);
            this.pathTextBox.Name = "pathTextBox";
            this.pathTextBox.Size = new System.Drawing.Size(489, 21);
            this.pathTextBox.TabIndex = 6;
            // 
            // ocrResultTextBox
            // 
            this.ocrResultTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.ocrResultTextBox.Font = new System.Drawing.Font("宋体", 9F);
            this.ocrResultTextBox.Location = new System.Drawing.Point(12, 503);
            this.ocrResultTextBox.Multiline = true;
            this.ocrResultTextBox.Name = "ocrResultTextBox";
            this.ocrResultTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.ocrResultTextBox.Size = new System.Drawing.Size(757, 246);
            this.ocrResultTextBox.TabIndex = 7;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(3, 180);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(47, 12);
            this.label8.TabIndex = 8;
            this.label8.Text = "ImgFile";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(3, 0);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(41, 12);
            this.label9.TabIndex = 9;
            this.label9.Text = "Models";
            // 
            // tableLayoutPanel2
            // 
            this.tableLayoutPanel2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tableLayoutPanel2.ColumnCount = 3;
            this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 100F));
            this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 80F));
            this.tableLayoutPanel2.Controls.Add(this.modelsTextBox, 1, 0);
            this.tableLayoutPanel2.Controls.Add(this.label9, 0, 0);
            this.tableLayoutPanel2.Controls.Add(this.modelsBtn, 2, 0);
            this.tableLayoutPanel2.Controls.Add(this.label8, 0, 6);
            this.tableLayoutPanel2.Controls.Add(this.pathTextBox, 1, 6);
            this.tableLayoutPanel2.Controls.Add(this.openBtn, 2, 6);
            this.tableLayoutPanel2.Controls.Add(this.label1, 0, 5);
            this.tableLayoutPanel2.Controls.Add(this.numThreadNumeric, 1, 5);
            this.tableLayoutPanel2.Controls.Add(this.label6, 0, 1);
            this.tableLayoutPanel2.Controls.Add(this.detNameTextBox, 1, 1);
            this.tableLayoutPanel2.Controls.Add(this.label10, 0, 2);
            this.tableLayoutPanel2.Controls.Add(this.clsNameTextBox, 1, 2);
            this.tableLayoutPanel2.Controls.Add(this.label11, 0, 3);
            this.tableLayoutPanel2.Controls.Add(this.recNameTextBox, 1, 3);
            this.tableLayoutPanel2.Controls.Add(this.label12, 0, 4);
            this.tableLayoutPanel2.Controls.Add(this.keysNameTextBox, 1, 4);
            this.tableLayoutPanel2.Location = new System.Drawing.Point(12, 12);
            this.tableLayoutPanel2.Name = "tableLayoutPanel2";
            this.tableLayoutPanel2.RowCount = 7;
            this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30F));
            this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30F));
            this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30F));
            this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30F));
            this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30F));
            this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30F));
            this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 30F));
            this.tableLayoutPanel2.Size = new System.Drawing.Size(675, 211);
            this.tableLayoutPanel2.TabIndex = 10;
            // 
            // initBtn
            // 
            this.initBtn.Location = new System.Drawing.Point(695, 132);
            this.initBtn.Name = "initBtn";
            this.initBtn.Size = new System.Drawing.Size(74, 91);
            this.initBtn.TabIndex = 10;
            this.initBtn.Text = "重新初始化";
            this.initBtn.UseVisualStyleBackColor = true;
            this.initBtn.Click += new System.EventHandler(this.initBtn_Click);
            // 
            // tableLayoutPanel3
            // 
            this.tableLayoutPanel3.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tableLayoutPanel3.ColumnCount = 3;
            this.tableLayoutPanel3.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 170F));
            this.tableLayoutPanel3.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel3.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel3.Controls.Add(this.pictureBox, 1, 0);
            this.tableLayoutPanel3.Controls.Add(this.strRestTextBox, 2, 0);
            this.tableLayoutPanel3.Controls.Add(this.tableLayoutPanel1, 0, 0);
            this.tableLayoutPanel3.Location = new System.Drawing.Point(12, 229);
            this.tableLayoutPanel3.Name = "tableLayoutPanel3";
            this.tableLayoutPanel3.RowCount = 1;
            this.tableLayoutPanel3.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel3.Size = new System.Drawing.Size(757, 268);
            this.tableLayoutPanel3.TabIndex = 11;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(3, 30);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(35, 12);
            this.label6.TabIndex = 11;
            this.label6.Text = "dbNet";
            // 
            // detNameTextBox
            // 
            this.detNameTextBox.Location = new System.Drawing.Point(103, 33);
            this.detNameTextBox.Name = "detNameTextBox";
            this.detNameTextBox.Size = new System.Drawing.Size(489, 21);
            this.detNameTextBox.TabIndex = 12;
            this.detNameTextBox.Text = "dbnet.onnx";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(3, 60);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(53, 12);
            this.label10.TabIndex = 13;
            this.label10.Text = "angleNet";
            // 
            // clsNameTextBox
            // 
            this.clsNameTextBox.Location = new System.Drawing.Point(103, 63);
            this.clsNameTextBox.Name = "clsNameTextBox";
            this.clsNameTextBox.Size = new System.Drawing.Size(489, 21);
            this.clsNameTextBox.TabIndex = 14;
            this.clsNameTextBox.Text = "angle_net.onnx";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(3, 90);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(47, 12);
            this.label11.TabIndex = 15;
            this.label11.Text = "crnnNet";
            // 
            // recNameTextBox
            // 
            this.recNameTextBox.Location = new System.Drawing.Point(103, 93);
            this.recNameTextBox.Name = "recNameTextBox";
            this.recNameTextBox.Size = new System.Drawing.Size(489, 21);
            this.recNameTextBox.TabIndex = 16;
            this.recNameTextBox.Text = "crnn_lite_lstm.onnx";
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(3, 120);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(29, 12);
            this.label12.TabIndex = 17;
            this.label12.Text = "keys";
            // 
            // keysNameTextBox
            // 
            this.keysNameTextBox.Location = new System.Drawing.Point(103, 123);
            this.keysNameTextBox.Name = "keysNameTextBox";
            this.keysNameTextBox.Size = new System.Drawing.Size(489, 21);
            this.keysNameTextBox.TabIndex = 18;
            this.keysNameTextBox.Text = "keys.txt";
            // 
            // FormOcrLite
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(784, 761);
            this.Controls.Add(this.tableLayoutPanel3);
            this.Controls.Add(this.tableLayoutPanel2);
            this.Controls.Add(this.ocrResultTextBox);
            this.Controls.Add(this.detectBtn);
            this.Controls.Add(this.initBtn);
            this.Name = "FormOcrLite";
            this.Text = "OcrLiteOnnxCs v1.0.3";
            this.Load += new System.EventHandler(this.FormOcrLite_Load);
            ((System.ComponentModel.ISupportInitialize)(this.numThreadNumeric)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.paddingNumeric)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.imgResizeNumeric)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.boxScoreThreshNumeric)).EndInit();
            this.tableLayoutPanel1.ResumeLayout(false);
            this.tableLayoutPanel1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.boxThreshNumeric)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.unClipRatioNumeric)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox)).EndInit();
            this.tableLayoutPanel2.ResumeLayout(false);
            this.tableLayoutPanel2.PerformLayout();
            this.tableLayoutPanel3.ResumeLayout(false);
            this.tableLayoutPanel3.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button openBtn;
        private System.Windows.Forms.Button modelsBtn;
        private System.Windows.Forms.TextBox modelsTextBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.NumericUpDown numThreadNumeric;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.NumericUpDown paddingNumeric;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.NumericUpDown imgResizeNumeric;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.NumericUpDown boxScoreThreshNumeric;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.NumericUpDown boxThreshNumeric;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.NumericUpDown unClipRatioNumeric;
        private System.Windows.Forms.CheckBox doAngleCheckBox;
        private System.Windows.Forms.CheckBox mostAngleCheckBox;
        private System.Windows.Forms.PictureBox pictureBox;
        private System.Windows.Forms.Button detectBtn;
        private System.Windows.Forms.TextBox pathTextBox;
        private System.Windows.Forms.CheckBox partImgCheckBox;
        private System.Windows.Forms.CheckBox debugCheckBox;
        private System.Windows.Forms.TextBox ocrResultTextBox;
        private System.Windows.Forms.TextBox strRestTextBox;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel2;
        private System.Windows.Forms.Button initBtn;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel3;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox detNameTextBox;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.TextBox clsNameTextBox;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.TextBox recNameTextBox;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.TextBox keysNameTextBox;
    }
}

