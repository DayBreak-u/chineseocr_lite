using Emgu.CV;
using Microsoft.ML.OnnxRuntime;
using Microsoft.ML.OnnxRuntime.Tensors;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;

namespace OcrLiteLib
{
    class CrnnNet
    {
        private readonly float[] MeanValues = { 127.5F, 127.5F, 127.5F };
        private readonly float[] NormValues = { 1.0F / 127.5F, 1.0F / 127.5F, 1.0F / 127.5F };
        private const int crnnDstHeight = 32;
        private const int crnnCols = 5531;

        private InferenceSession crnnNet;
        private List<string> keys;

        public CrnnNet() { }

        ~CrnnNet()
        {
            crnnNet.Dispose();
        }

        public void InitModel(string path, string keysPath, int numThread)
        {
            try
            {
                SessionOptions op = new SessionOptions();
                op.GraphOptimizationLevel = GraphOptimizationLevel.ORT_ENABLE_EXTENDED;
                op.InterOpNumThreads = numThread;
                op.IntraOpNumThreads = numThread;
                crnnNet = new InferenceSession(path, op);
                keys = InitKeys(keysPath);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message + ex.StackTrace);
                throw ex;
            }
        }
        private List<string> InitKeys(string path)
        {
            StreamReader sr = new StreamReader(path, Encoding.UTF8);
            List<string> keys = new List<string>();
            String line;
            while ((line = sr.ReadLine()) != null)
            {
                //Console.WriteLine(line.ToString());
                keys.Add(line);
            }
            Console.WriteLine($"keys Size = {keys.Count}");
            return keys;
        }

        public List<TextLine> GetTextLines(List<Mat> partImgs)
        {
            List<TextLine> textLines = new List<TextLine>();
            for (int i = 0; i < partImgs.Count; i++)
            {
                var startTicks = DateTime.Now.Ticks;
                var textLine = GetTextLine(partImgs[i]);
                var endTicks = DateTime.Now.Ticks;
                var crnnTime = (endTicks - startTicks) / 10000F;
                textLine.Time = crnnTime;
                textLines.Add(textLine);
            }
            return textLines;
        }

        private TextLine GetTextLine(Mat src)
        {
            TextLine textLine = new TextLine();

            float scale = (float)crnnDstHeight / (float)src.Rows;
            int dstWidth = (int)((float)src.Cols * scale);

            Mat srcResize = new Mat();
            CvInvoke.Resize(src, srcResize, new Size(dstWidth, crnnDstHeight));
            Tensor<float> inputTensors = OcrUtils.SubstractMeanNormalize(srcResize, MeanValues, NormValues);
            var inputs = new List<NamedOnnxValue>
            {
                NamedOnnxValue.CreateFromTensor("input", inputTensors)
            };
            try
            {
                using (IDisposableReadOnlyCollection<DisposableNamedOnnxValue> results = crnnNet.Run(inputs))
                {
                    var resultsArray = results.ToArray();
                    Console.WriteLine(resultsArray);
                    float[] outputData = resultsArray[0].AsEnumerable<float>().ToArray();

                    int crnnRows = outputData.Count() / crnnCols;

                    return ScoreToTextLine(outputData, crnnRows, crnnCols);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message + ex.StackTrace);
                //throw ex;
            }

            return textLine;
        }

        private TextLine ScoreToTextLine(float[] srcData, int rows, int cols)
        {
            StringBuilder sb = new StringBuilder();
            TextLine textLine = new TextLine();

            int lastIndex = 0;
            List<float> scores = new List<float>();

            for (int i = 0; i < rows; i++)
            {
                int maxIndex = 0;
                float maxValue = -1000F;

                //do softmax
                List<float> expList = new List<float>();
                for (int j = 0; j < cols; j++)
                {
                    float expSingle = (float)Math.Exp(srcData[i * cols + j]);
                    expList.Add(expSingle);
                }
                float partition = expList.Sum();
                for (int j = 0; j < cols; j++)
                {
                    float softmax = expList[j] / partition;
                    if (softmax > maxValue)
                    {
                        maxValue = softmax;
                        maxIndex = j;
                    }
                }

                //no softmax
                /*for (int j = 0; j < cols; j++) {
                    if (srcData[i * cols + j] > maxValue) {
                        maxValue = srcData[i * cols + j];
                        maxIndex = j;
                    }
                }*/

                if (maxIndex > 0 && maxIndex < keys.Count && (!(i > 0 && maxIndex == lastIndex)))
                {
                    scores.Add(maxValue);
                    sb.Append(keys[maxIndex - 1]);
                }
                lastIndex = maxIndex;
            }
            textLine.Text = sb.ToString();
            textLine.CharScores = scores;
            return textLine;
        }

    }
}