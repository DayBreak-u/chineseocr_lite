using Emgu.CV;
using Emgu.CV.CvEnum;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Text;

namespace OcrLiteLib
{
    public class OcrLite
    {
        public bool isPartImg { get; set; }
        public bool isDebugImg { get; set; }
        private string dbNetPath, angleNetPath, crnnNetPath, keysPath;
        private DbNet dbNet;
        private AngleNet angleNet;
        private CrnnNet crnnNet;

        public OcrLite()
        {
            dbNet = new DbNet();
            angleNet = new AngleNet();
            crnnNet = new CrnnNet();
        }

        public void InitModels(string detPath,string clsPath,string recPath,string keysPath,int numThread)
        {
            try
            {
                dbNet.InitModel(detPath,numThread);
                angleNet.InitModel(clsPath, numThread);
                crnnNet.InitModel(recPath, keysPath, numThread);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message + ex.StackTrace);
                throw ex;
            }
        }

        public OcrResult Detect(string img, int padding, int imgResize, float boxScoreThresh, float boxThresh, 
                              float unClipRatio, bool doAngle, bool mostAngle)
        {
            Mat brgSrc = CvInvoke.Imread(img, ImreadModes.Color);//default : BGR
            Mat originSrc = new Mat();
            CvInvoke.CvtColor(brgSrc, originSrc, ColorConversion.Bgr2Rgb);// convert to RGB
            Rectangle originRect = new Rectangle(padding, padding, originSrc.Cols, originSrc.Rows);
            Mat paddingSrc = OcrUtils.MakePadding(originSrc, padding);

            int resize;
            if (imgResize <= 0)
            {
                resize = Math.Max(paddingSrc.Cols, paddingSrc.Rows);
            }
            else
            {
                resize = imgResize;
            }
            ScaleParam scale = ScaleParam.GetScaleParam(paddingSrc, resize);

            return DetectOnce(paddingSrc, originRect, scale, boxScoreThresh, boxThresh, unClipRatio, doAngle, mostAngle);
        }

        private OcrResult DetectOnce(Mat src, Rectangle originRect, ScaleParam scale, float boxScoreThresh, float boxThresh,
                              float unClipRatio, bool doAngle, bool mostAngle)
        {
            Mat textBoxPaddingImg = src.Clone();
            int thickness = OcrUtils.GetThickness(src);
            Console.WriteLine("=====Start detect=====");
            var startTicks = DateTime.Now.Ticks;

            Console.WriteLine("---------- step: dbNet getTextBoxes ----------");
            var textBoxes = dbNet.GetTextBoxes(src, scale, boxScoreThresh, boxThresh, unClipRatio);
            var dbNetTime = (DateTime.Now.Ticks - startTicks) / 10000F;

            Console.WriteLine($"TextBoxesSize({textBoxes.Count})");
            textBoxes.ForEach(x => Console.WriteLine(x));
            //Console.WriteLine($"dbNetTime({dbNetTime}ms)");

            Console.WriteLine("---------- step: drawTextBoxes ----------");
            OcrUtils.DrawTextBoxes(textBoxPaddingImg, textBoxes, thickness);
            //CvInvoke.Imshow("ResultPadding", textBoxPaddingImg);

            //---------- getPartImages ----------
            List<Mat> partImages = OcrUtils.GetPartImages(src, textBoxes);
            if (isPartImg)
            {
                for (int i = 0; i < partImages.Count; i++)
                {
                    CvInvoke.Imshow($"PartImg({i})", partImages[i]);
                }
            }

            Console.WriteLine("---------- step: angleNet getAngles ----------");
            List<Angle> angles = angleNet.GetAngles(partImages, doAngle, mostAngle);
            //angles.ForEach(x => Console.WriteLine(x));

            //Rotate partImgs
            for (int i = 0; i < partImages.Count; ++i)
            {
                if (angles[i].Index == 0)
                {
                    partImages[i] = OcrUtils.MatRotateClockWise180(partImages[i]);
                }
                if (isDebugImg)
                {
                    CvInvoke.Imshow($"DebugImg({i})", partImages[i]);
                }
            }

            Console.WriteLine("---------- step: crnnNet getTextLines ----------");
            List<TextLine> textLines = crnnNet.GetTextLines(partImages);
            //textLines.ForEach(x => Console.WriteLine(x));

            List<TextBlock> textBlocks = new List<TextBlock>();
            for (int i = 0; i < textLines.Count; ++i)
            {
                TextBlock textBlock = new TextBlock();
                textBlock.BoxPoints = textBoxes[i].Points;
                textBlock.BoxScore = textBoxes[i].Score;
                textBlock.AngleIndex = angles[i].Index;
                textBlock.AngleScore = angles[i].Score;
                textBlock.AngleTime = angles[i].Time;
                textBlock.Text = textLines[i].Text;
                textBlock.CharScores = textLines[i].CharScores;
                textBlock.CrnnTime = textLines[i].Time;
                textBlock.BlockTime = angles[i].Time + textLines[i].Time;
                textBlocks.Add(textBlock);
            }
            //textBlocks.ForEach(x => Console.WriteLine(x));

            var endTicks = DateTime.Now.Ticks;
            var fullDetectTime = (endTicks - startTicks) / 10000F;
            //Console.WriteLine($"fullDetectTime({fullDetectTime}ms)");

            //cropped to original size
            Mat rgbBoxImg = new Mat(textBoxPaddingImg, originRect);
            Mat boxImg = new Mat();
            CvInvoke.CvtColor(rgbBoxImg, boxImg, ColorConversion.Rgb2Bgr);//convert to BGR for Output Result Img
            //CvInvoke.Imshow("Result", boxImg);

            StringBuilder strRes = new StringBuilder();
            textBlocks.ForEach(x => strRes.AppendLine(x.Text));

            OcrResult ocrResult = new OcrResult();
            ocrResult.TextBlocks = textBlocks;
            ocrResult.DbNetTime = dbNetTime;
            ocrResult.BoxImg = boxImg;
            ocrResult.DetectTime = fullDetectTime;
            ocrResult.StrRes = strRes.ToString();

            return ocrResult;
        }

    }
}
