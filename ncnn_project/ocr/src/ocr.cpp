#if (_MSC_VER >= 1915)
#define no_init_all deprecated
#endif

#include "precomp.h"


void OCR::InitGPU(ncnn::Net & net)
{
    //dbnet, crnn_net, angle_net;
   
    net.opt.use_vulkan_compute = 1;// line2
    //net.opt.num_threads = 1;
    //net.opt.use_fp16_packed = true;
    //net.opt.use_fp16_storage = true;
    //net.opt.use_fp16_arithmetic = true;
    //net.opt.use_int8_storage = false;
    //net.opt.use_int8_arithmetic = false;
}

OCR::OCR(bool bUseGPU) : m_bReady(false), m_bVerbose(false)
{
    m_bUseGPU = bUseGPU;
#ifdef USE_GPU
    if (m_bUseGPU)
    {
        ncnn::create_gpu_instance();
        InitGPU(dbnet);
        InitGPU(crnn_net);
        InitGPU(angle_net);
    }
#endif
    m_bReady = Init("../models/");
}

OCR::OCR(const char* szModelDir, bool bUseGPU) :m_bReady(false), m_bVerbose(false)
{
    m_bUseGPU = bUseGPU;

#ifdef USE_GPU
    if (m_bUseGPU)
    {
        ncnn::create_gpu_instance();
        InitGPU(dbnet);
        InitGPU(crnn_net);
        InitGPU(angle_net);
    }
#endif
    m_bReady = Init(szModelDir);
}

bool OCR::Init(const char* szModelDir)
{
    if (m_bReady)  // cannot be initilized twice.
        return m_bReady;

    
    string strDirPath;
    if (!szModelDir || !strlen(szModelDir))
    {
        strDirPath = "../models/";
    }
    else
        strDirPath = szModelDir;

    if (szModelDir[strlen(szModelDir) - 1] != '/' && szModelDir[strlen(szModelDir) - 1] != '\\')
    {
        strDirPath += "/";
    }

    dbnet.load_param((strDirPath + COL_DBNET_OP_PARAM).c_str());
    dbnet.load_model((strDirPath + COL_DBNET_OP_BIN).c_str());

    crnn_net.load_param((strDirPath + COL_CRNN_LITE_PARAM).c_str());
    crnn_net.load_model((strDirPath + COL_CRNN_LITE_BIN).c_str());

    angle_net.load_param((strDirPath + COL_ANGLE_OP_PARAM).c_str());
    angle_net.load_model((strDirPath + COL_ANGLE_OP_BIN).c_str());

    //load keys
    ifstream in(strDirPath + COL_KEYS_FILE);
    std::string filename;
    std::string line;

 
    if (in.is_open()) // 有该文件
    {
        while (getline(in, line)) // line中不包括每行的换行符
        {
            alphabetChinese.push_back(line);
        }
        alphabetChinese.push_back(" ");
        alphabetChinese.push_back("·");

        return true;
    }
    else // 没有该文件
    {
        if(m_bVerbose)
            std::cout << "cannot find file: "<< (string(szModelDir) + COL_KEYS_FILE)  << std::endl;
    }

    return false;
}


std::vector<std::string> crnn_deocde(const ncnn::Mat score , std::vector<std::string> alphabetChinese) {
    float *srcdata = (float* ) score.data;
    std::vector<std::string> str_res;
    int last_index = 0;  
    for (int i = 0; i < score.h;i++){
        int max_index = 0;
        
        float max_value = -1000;
        for (int j =0; j< score.w; j++){
            if (srcdata[ i * score.w + j ] > max_value){
                max_value = srcdata[i * score.w + j ];
                max_index = j;
            }
        }
        if (max_index >0 && (! (i>0 && max_index == last_index))  ){
//            std::cout <<  max_index - 1 << std::endl;
//            std::string temp_str =  utf8_substr2(alphabetChinese,max_index - 1,1)  ;
            str_res.push_back(alphabetChinese[max_index-1]);
        }



        last_index = max_index;
    }
    return str_res;
}



cv::Mat resize_img(cv::Mat src, int short_size)
{
    int w = src.cols;
    int h = src.rows;
    if (short_size == 0)
        short_size = w > h ? h : w;

    //int nMax = w > h ? w : h;

    //if (short_size > 1000)
    //{
    //    short_size = 1000;   // 我觉得缩放条件，最短边小于1000，就用最短边的值，大于1000，就用1000，不在乎速度就不缩放  by  benjaminwan
    //}
    //// 对于比例差距太大的，需要进一步计算

    //if( ( w/h > 50 || h/w >50)  && short_size > 500 ) //长宽比例差别太大
    //    short_size = 1000;

    // std::cout<<"原图尺寸 (" << w << ", "<<h<<")"<<std::endl;
    float scale = 1.f;
    if (w < h)
    {
        scale = (float)short_size / w;
        w = short_size;
        h = h * scale;
    }
    else
    {
        scale = (float)short_size / h;
        h = short_size;
        w = w * scale;
    }
    if (h % 32 != 0)
    {
        h = (h / 32 + 1) * 32;
    }
    if (w % 32 != 0)
    {
        w = (w / 32 + 1) * 32;
    }
    // std::cout<<"缩放尺寸 (" << w << ", "<<h<<")"<<std::endl;
    cv::Mat result;
    cv::resize(src, result, cv::Size(w, h));
    return result;
}

cv::Mat draw_bbox(cv::Mat &src, const std::vector<std::vector<cv::Point>> &bboxs) {
    cv::Mat dst;
    if (src.channels() == 1) {
        cv::cvtColor(src, dst, cv::COLOR_GRAY2BGR);
    } else {
        dst = src.clone();
    }
    auto color = cv::Scalar(0, 0, 255);
    for (auto bbox :bboxs) {

        cv::line(dst, bbox[0], bbox[1], color, 3);
        cv::line(dst, bbox[1], bbox[2], color, 3);
        cv::line(dst, bbox[2], bbox[3], color, 3);
        cv::line(dst, bbox[3], bbox[0], color, 3);
    }
    return dst;
}


cv::Mat matRotateClockWise180(cv::Mat src)//顺时针180
{
	//0: 沿X轴翻转； >0: 沿Y轴翻转； <0: 沿X轴和Y轴翻转
	flip(src, src, 0);// 翻转模式，flipCode == 0垂直翻转（沿X轴翻转），flipCode>0水平翻转（沿Y轴翻转），flipCode<0水平垂直翻转（先沿X轴翻转，再沿Y轴翻转，等价于旋转180°）
	flip(src, src, 1);
	return src;
	//transpose(src, src);// 矩阵转置
}

cv::Mat matRotateClockWise90(cv::Mat src)
{

	// 矩阵转置
	transpose(src, src);
	//0: 沿X轴翻转； >0: 沿Y轴翻转； <0: 沿X轴和Y轴翻转
	flip(src, src, 1);// 翻转模式，flipCode == 0垂直翻转（沿X轴翻转），flipCode>0水平翻转（沿Y轴翻转），flipCode<0水平垂直翻转（先沿X轴翻转，再沿Y轴翻转，等价于旋转180°）
	return src;
}





cv::Mat GetRotateCropImage(const cv::Mat &srcimage,
                                           std::vector<cv::Point>  box) {
  cv::Mat image;
  srcimage.copyTo(image);
  std::vector<cv::Point> points = box;

  int x_collect[4] = {box[0].x, box[1].x, box[2].x, box[3].x};
  int y_collect[4] = {box[0].y, box[1].y, box[2].y, box[3].y};
  int left = int(*std::min_element(x_collect, x_collect + 4));
  int right = int(*std::max_element(x_collect, x_collect + 4));
  int top = int(*std::min_element(y_collect, y_collect + 4));
  int bottom = int(*std::max_element(y_collect, y_collect + 4));

  cv::Mat img_crop;
  image(cv::Rect(left, top, right - left, bottom - top)).copyTo(img_crop);

  for (int i = 0; i < points.size(); i++) {
    points[i].x -= left;
    points[i].y -= top;
  }

  int img_crop_width = int(sqrt(pow(points[0].x - points[1].x, 2) +
                                pow(points[0].y - points[1].y, 2)));
  int img_crop_height = int(sqrt(pow(points[0].x - points[3].x, 2) +
                                 pow(points[0].y - points[3].y, 2)));

  cv::Point2f pts_std[4];
  pts_std[0] = cv::Point2f(0., 0.);
  pts_std[1] = cv::Point2f(img_crop_width, 0.);
  pts_std[2] = cv::Point2f(img_crop_width, img_crop_height);
  pts_std[3] = cv::Point2f(0.f, img_crop_height);

  cv::Point2f pointsf[4];
  pointsf[0] = cv::Point2f(points[0].x, points[0].y);
  pointsf[1] = cv::Point2f(points[1].x, points[1].y);
  pointsf[2] = cv::Point2f(points[2].x, points[2].y);
  pointsf[3] = cv::Point2f(points[3].x, points[3].y);

  cv::Mat M = cv::getPerspectiveTransform(pointsf, pts_std);

  cv::Mat dst_img;
  cv::warpPerspective(img_crop, dst_img, M,
                      cv::Size(img_crop_width, img_crop_height),
                      cv::BORDER_REPLICATE);

  if (float(dst_img.rows) >= float(dst_img.cols) * 1.5) {
    cv::Mat srcCopy = cv::Mat(dst_img.rows, dst_img.cols, dst_img.depth());
    cv::transpose(dst_img, srcCopy);
    cv::flip(srcCopy, srcCopy, 0);
    return srcCopy;
  } else {
    return dst_img;
  }
}


cv::RotatedRect getPartRect(std::vector<cv::Point>& box, float scaleWidth, float scaleHeight) {
    cv::RotatedRect rect = cv::minAreaRect(box);
    int minSize = rect.size.width > rect.size.height ? rect.size.height : rect.size.width;
    if (rect.size.width > rect.size.height) {
        rect.size.width = rect.size.width + (float)minSize * scaleWidth;
        rect.size.height = rect.size.height + (float)minSize * scaleHeight;
    }
    else {
        rect.size.width = rect.size.width + (float)minSize * scaleHeight;
        rect.size.height = rect.size.height + (float)minSize * scaleWidth;
    }
    return rect;
}

// detect the angle

// 相找最大值 点
Angle scoreToAngle(ncnn::Mat& score) {
    auto* srcData = (float*)score.data;
    int angleIndex = 0;
    float maxValue = -1000.0f;
    maxValue = srcData[0];
    for (int i = 0; i < score.w; i++) {
        if (srcData[i] > maxValue)
        {
            angleIndex = i;
            maxValue = srcData[i];
        }
    }
    return Angle(angleIndex, maxValue);
}


void  OCR::detect(cv::Mat im_bgr,int short_size,double & dTotalTime)
{

    //printf("do ocring...\r\n");
        // 图像缩放
    auto im = resize_img(im_bgr, short_size);

    int wid = im.cols;
    int hi = im.rows;
//    int srcwid = im_bgr.cols;
//    int srchi = im_bgr.rows;

    float h_scale = im_bgr.rows * 1.0 / im.rows;
    float w_scale = im_bgr.cols * 1.0 / im.cols;

    ncnn::Mat in = ncnn::Mat::from_pixels(im.data, ncnn::Mat::PIXEL_BGR2RGB, im.cols, im.rows);
    in.substract_mean_normalize(mean_vals_dbnet,norm_vals_dbnet);

    // std::cout << "输入尺寸 (" << in.w << ", " << in.h << ")" << std::endl;

    ncnn::Extractor ex = dbnet.create_extractor();
    ex.set_num_threads(num_thread);
    ex.input("input0", in);
    ncnn::Mat dbnet_out;

    double time1 = static_cast<double>( cv::getTickCount());
    ex.extract("out1", dbnet_out);

    if(m_bVerbose)
        std::cout << "dbnet前向时间:" << (static_cast<double>( cv::getTickCount()) - time1) / cv::getTickFrequency() << "s" << std::endl;
    // std::cout << "网络输出尺寸 (" << preds.w << ", " << preds.h << ", " << preds.c << ")" << std::endl;

    time1 = static_cast<double>( cv::getTickCount());


    cv::Mat fmapmat(hi, wid, CV_32FC1);
	memcpy(fmapmat.data, (float*)dbnet_out.data, wid * hi * sizeof(float));

	cv::Mat norfmapmat;

	norfmapmat = fmapmat > thresh;


 	std::vector<std::vector<cv::Point>> contours;
 	std::vector<std::vector<cv::Point>> boxes;
 	cv::findContours(norfmapmat, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
 	for (int i = 0; i < contours.size(); ++i)
 	{
 		std::vector<cv::Point> minbox;
 		float minedgesize, alledgesize;
 		get_mini_boxes(contours[i], minbox, minedgesize, alledgesize);

 		if (minedgesize < min_size)
 			continue;
 		float score = box_score_fast(fmapmat, contours[i]);

		if (score < box_thresh)
			continue;


 		std::vector<cv::Point> newbox;
 		unclip(minbox, alledgesize, newbox, unclip_ratio);

 		get_mini_boxes(newbox, minbox, minedgesize, alledgesize);

 		if (minedgesize < min_size + 2)
 			continue;

 		for (int i = 0; i < minbox.size(); ++i)
 	        {
 	          minbox[i].x = minbox[i].x * w_scale;
 	          minbox[i].y = minbox[i].y * h_scale;
 	        }

        boxes.push_back(minbox);

 	}
    if (m_bVerbose)
    {
        std::cout << "dbnet decode 时间:" << (static_cast<double>(cv::getTickCount()) - time1) / cv::getTickFrequency() << "s" << std::endl;
        std::cout << "boxzie: " << boxes.size() << std::endl;
    }



//    auto result = draw_bbox(im_bgr, boxes);
//    cv::imwrite("./imgs/result.jpg", result);

    time1 = static_cast<double>( cv::getTickCount());
    //开始行文本角度检测和文字识别

    bool bAllRevert = true;
    bool bRevert = false;
    if (m_bVerbose)
        std::cout << "Result: \n";
    for (int i = boxes.size() - 1; i >= 0; i--)
    {
        cv::Mat text_img;
        text_img = GetRotateCropImage(im_bgr, boxes[i]);
        int part_im_w = text_img.cols;
        int part_im_h = text_img.rows;

        // 开始文本识别
        int crnn_w_target;
        float scale = crnn_h * 1.0 / part_im_h;
        crnn_w_target = int(text_img.cols * scale);


        // 角度检测

       

        ncnn::Mat  text_in = ncnn::Mat::from_pixels_resize(text_img.data, ncnn::Mat::PIXEL_BGR2RGB, text_img.cols, text_img.rows, crnn_w_target, crnn_h);

        int crnn_w = text_in.w;
        int crnn_h = text_in.h;
        ncnn::Mat angle_in;
        if (crnn_w >= angle_target_w)
            copy_cut_border(text_in, angle_in, 0, 0, 0, crnn_w - angle_target_w);
        else
            copy_make_border(text_in, angle_in, 0, 0, 0, angle_target_w - crnn_w, 0, 255.f);

        angle_in.substract_mean_normalize(mean_vals_crnn_angle, norm_vals_crnn_angle);
        ncnn::Extractor angle_ex = angle_net.create_extractor();
        angle_ex.set_num_threads(num_thread);
        angle_ex.input("input", angle_in);

        ncnn::Mat angle_preds;
        angle_ex.extract("out", angle_preds);

        Angle angle_val=scoreToAngle(angle_preds);
     

      
        //判断方向
        bRevert = false;
        if (angle_val.index == 0 || angle_val.index == 2)
        {
            text_img = matRotateClockWise180(text_img);
            bRevert=true;
            
        }


      


        // 结束 角度检测
   

        //crnn识别
        ncnn::Mat  crnn_in = ncnn::Mat::from_pixels_resize(text_img.data, ncnn::Mat::PIXEL_BGR2RGB, text_img.cols, text_img.rows, crnn_w_target, crnn_h);
        crnn_in.substract_mean_normalize(mean_vals_crnn_angle, norm_vals_crnn_angle);

        ncnn::Mat crnn_preds;


        ncnn::Extractor crnn_ex = crnn_net.create_extractor();
        crnn_ex.set_num_threads(num_thread);
        crnn_ex.input("input", crnn_in);


        ncnn::Mat blob162;
        crnn_ex.extract("1000", blob162);

        ncnn::Mat blob263(5531, blob162.h);
        //batch fc
        for (int i = 0; i < blob162.h; i++)
        {
            ncnn::Extractor crnn_ex_2 = crnn_net.create_extractor();
            crnn_ex_2.set_num_threads(num_thread);
            ncnn::Mat blob243_i = blob162.row_range(i, 1);
            crnn_ex_2.input("1014", blob243_i);

            ncnn::Mat blob263_i;
            crnn_ex_2.extract("1015", blob263_i);

            memcpy(blob263.row(i), blob263_i, 5531 * sizeof(float));
        }

        crnn_preds = blob263;


        auto res_pre = crnn_deocde(crnn_preds, alphabetChinese);

        if (m_bVerbose)
        {
            for (size_t i = 0; i < res_pre.size(); i++) {
                std::cout << res_pre[i];
            }
            std::cout << std::endl;

        }
        else
        {
            string strLine;
            for (size_t s = 0; s < res_pre.size(); s++)
            {
                strLine+=res_pre[s];
            }

            if (!bRevert && !strLine.empty())
            {
                //printf("unreverting...: %s \n", strLine.c_str());
                bAllRevert = false;
            }
            strLine += "\n";
            m_Result.push_back(strLine);
        }
    }

    if (bAllRevert)
    {
        std::reverse(m_Result.begin(), m_Result.end());
    }
        dTotalTime = (static_cast<double>(cv::getTickCount()) - time1) / cv::getTickFrequency();

        if (m_bVerbose)
            std::cout << "Total time:" << dTotalTime << "s" << std::endl;

   

}


OCR::~OCR()
{

   
    dbnet.clear();
    crnn_net.clear();
    angle_net.clear();
#ifdef USE_GPU
    if(m_bUseGPU)
     ncnn::destroy_gpu_instance();
#endif
    if (m_bVerbose)
        cout << "ocr nets cleared" << endl;
}