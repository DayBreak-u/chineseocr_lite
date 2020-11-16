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
            ocrEngin = new OcrLite(modelsDir,(int) numThreadNumeric.Value);
        }

        private void initBtn_Click(object sender, EventArgs e)
        {
            ocrEngin = new OcrLite(modelsTextBox.Text, (int) numThreadNumeric.Value);
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
            int padding = (int)paddingNumeric.Value;
            int imgResize = (int)imgResizeNumeric.Value;
            float boxScoreThresh = (float)boxScoreThreshNumeric.Value;
            float boxThresh = (float)boxThreshNumeric.Value;
            float minArea = (float)minAreaNumeric.Value;
            float unClipRatio = (float)unClipRatioNumeric.Value;
            bool doAngle = doAngleCheckBox.Checked;
            bool mostAngle = mostAngleCheckBox.Checked;
            OcrResult ocrResult = ocrEngin.Detect(pathTextBox.Text, padding, imgResize, boxScoreThresh, boxThresh, minArea, unClipRatio, doAngle, mostAngle);
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
