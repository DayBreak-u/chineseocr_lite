using Emgu.CV;
using Emgu.CV.CvEnum;
using OcrLiteLib;
using System;
using System.IO;
using System.Windows.Forms;

namespace OcrLiteOnnxCs
{
    public partial class FormOcrLite : Form
    {
        private OcrLite ocrEngin;

        public FormOcrLite()
        {
            InitializeComponent();
        }

        private void FormOcrLite_Load(object sender, EventArgs e)
        {
            string appPath = AppDomain.CurrentDomain.BaseDirectory;
            string appDir = Directory.GetParent(appPath).FullName;
            string modelsDir = appPath + "models";
            modelsTextBox.Text = modelsDir;
            string detPath = modelsDir + "\\" + detNameTextBox.Text;
            string clsPath = modelsDir + "\\" + clsNameTextBox.Text;
            string recPath = modelsDir + "\\" + recNameTextBox.Text;
            string keysPath = modelsDir + "\\" + keysNameTextBox.Text;
            bool isDetExists = File.Exists(detPath);
            if (!isDetExists)
            {
                MessageBox.Show("模型文件不存在:" + detPath);
            }
            bool isClsExists = File.Exists(clsPath);
            if (!isClsExists)
            {
                MessageBox.Show("模型文件不存在:" + clsPath);
            }
            bool isRecExists = File.Exists(recPath);
            if (!isRecExists)
            {
                MessageBox.Show("模型文件不存在:" + recPath);
            }
            bool isKeysExists = File.Exists(keysPath);
            if (!isKeysExists)
            {
                MessageBox.Show("Keys文件不存在:" + keysPath);
            }
            if(isDetExists && isClsExists && isRecExists && isKeysExists)
            {
                ocrEngin = new OcrLite();
                ocrEngin.InitModels(detPath,clsPath,recPath,keysPath, (int)numThreadNumeric.Value);
            }
            else
            {
                MessageBox.Show("初始化失败，请确认模型文件夹和文件后，重新初始化！");
            }
        }

        private void initBtn_Click(object sender, EventArgs e)
        {
            string modelsDir = modelsTextBox.Text;
            string detPath = modelsDir + "\\" + detNameTextBox.Text;
            string clsPath = modelsDir + "\\" + clsNameTextBox.Text;
            string recPath = modelsDir + "\\" + recNameTextBox.Text;
            string keysPath = modelsDir + "\\" + keysNameTextBox.Text;
            bool isDetExists = File.Exists(detPath);
            if (!isDetExists)
            {
                MessageBox.Show("模型文件不存在:" + detPath);
            }
            bool isClsExists = File.Exists(clsPath);
            if (!isClsExists)
            {
                MessageBox.Show("模型文件不存在:" + clsPath);
            }
            bool isRecExists = File.Exists(recPath);
            if (!isRecExists)
            {
                MessageBox.Show("模型文件不存在:" + recPath);
            }
            bool isKeysExists = File.Exists(recPath);
            if (!isKeysExists)
            {
                MessageBox.Show("Keys文件不存在:" + keysPath);
            }
            if (isDetExists && isClsExists && isRecExists && isKeysExists)
            {
                ocrEngin = new OcrLite();
                ocrEngin.InitModels(detPath, clsPath, recPath, keysPath, (int)numThreadNumeric.Value);
            }
            else
            {
                MessageBox.Show("初始化失败，请确认模型文件夹和文件后，重新初始化！");
            }
        }

        private void openBtn_Click(object sender, EventArgs e)
        {
            using (var dlg = new OpenFileDialog())
            {
                dlg.Multiselect = false;
                dlg.Filter = "(*.JPG,*.PNG,*.JPEG,*.BMP,*.GIF)|*.JPG;*.PNG;*.JPEG;*.BMP;*.GIF|All files(*.*)|*.*";
                if (dlg.ShowDialog() == DialogResult.OK && !string.IsNullOrEmpty(dlg.FileName))
                {
                    pathTextBox.Text = dlg.FileName;
                    Mat src = CvInvoke.Imread(dlg.FileName, ImreadModes.Color);
                    pictureBox.Image = src.ToBitmap();
                }
            }
        }

        private void modelsBtn_Click(object sender, EventArgs e)
        {
            using (var dlg = new FolderBrowserDialog())
            {
                dlg.SelectedPath = Environment.CurrentDirectory + "\\models";
                if (dlg.ShowDialog() == DialogResult.OK && !string.IsNullOrEmpty(dlg.SelectedPath))
                {
                    modelsTextBox.Text = dlg.SelectedPath;
                }
            }
        }

        private void detectBtn_Click(object sender, EventArgs e)
        {
            if(ocrEngin == null)
            {
                MessageBox.Show("未初始化，无法执行!");
                return;
            }
            string targetImg = pathTextBox.Text;
            if (!File.Exists(targetImg))
            {
                MessageBox.Show("目标图片不存在，请用Open按钮打开");
                return;
            }
            int padding = (int)paddingNumeric.Value;
            int imgResize = (int)imgResizeNumeric.Value;
            float boxScoreThresh = (float)boxScoreThreshNumeric.Value;
            float boxThresh = (float)boxThreshNumeric.Value;
            float unClipRatio = (float)unClipRatioNumeric.Value;
            bool doAngle = doAngleCheckBox.Checked;
            bool mostAngle = mostAngleCheckBox.Checked;
            OcrResult ocrResult = ocrEngin.Detect(pathTextBox.Text, padding, imgResize, boxScoreThresh, boxThresh, unClipRatio, doAngle, mostAngle);
            ocrResultTextBox.Text = ocrResult.ToString();
            strRestTextBox.Text = ocrResult.StrRes;
            pictureBox.Image = ocrResult.BoxImg.ToBitmap();
        }

        private void partImgCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            ocrEngin.isPartImg = partImgCheckBox.Checked;
        }

        private void debugCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            ocrEngin.isDebugImg = debugCheckBox.Checked;
        }
    }
}
