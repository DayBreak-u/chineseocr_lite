#include "ocr.h"

#define CRNN_LSTM 1

OCR::OCR()
{
    psenet.load_param("../../models/psenet_lite_mbv2.param");
    psenet.load_model("../../models/psenet_lite_mbv2.bin");

#if CRNN_LSTM
    crnn_net.load_param("../../models/crnn_lite_lstm_v2.param");
    crnn_net.load_model("../../models/crnn_lite_lstm_v2.bin");
    crnn_vertical_net.load_param("../../models/crnn_lite_lstm_vertical.param");
    crnn_vertical_net.load_model("../../models/crnn_lite_lstm_vertical.bin");
#else
    crnn_net.load_param("../../models/crnn_lite_dw_dense.param");
    crnn_net.load_model("../../models/crnn_lite_dw_dense.bin");
    crnn_vertical_net.load_param("../../models/crnn_lite_dw_dense_vertical.param");
    crnn_vertical_net.load_model("../../models/crnn_lite_dw_dense_vertical.bin");
#endif



    angle_net.load_param("../../models/shufflenetv2_05_angle.param");
    angle_net.load_model("../../models/shufflenetv2_05_angle.bin");
}


string utf8_substr2(const string &str,int start, int length=INT_MAX)
{
    int i,ix,j,realstart,reallength;
    if (length==0) return "";
    if (start<0 || length <0)
    {
        //find j=utf8_strlen(str);
        for(j=0,i=0,ix=str.length(); i<ix; i+=1, j++)
        {
            unsigned char c= str[i];
            if      (c>=0   && c<=127) i+=0;
            else if (c>=192 && c<=223) i+=1;
            else if (c>=224 && c<=239) i+=2;
            else if (c>=240 && c<=247) i+=3;
            else if (c>=248 && c<=255) return "";//invalid utf8
        }
        if (length !=INT_MAX && j+length-start<=0) return "";
        if (start  < 0 ) start+=j;
        if (length < 0 ) length=j+length-start;
    }

    j=0,realstart=0,reallength=0;
    for(i=0,ix=str.length(); i<ix; i+=1, j++)
    {
        if (j==start) { realstart=i; }
        if (j>=start && (length==INT_MAX || j<=start+length)) { reallength=i-realstart; }
        unsigned char c= str[i];
        if      (c>=0   && c<=127) i+=0;
        else if (c>=192 && c<=223) i+=1;
        else if (c>=224 && c<=239) i+=2;
        else if (c>=240 && c<=247) i+=3;
        else if (c>=248 && c<=255) return "";//invalid utf8
    }
    if (j==start) { realstart=i; }
    if (j>=start && (length==INT_MAX || j<=start+length)) { reallength=i-realstart; }

    return str.substr(realstart,reallength);
}


std::vector<std::string> crnn_deocde(const ncnn::Mat score , string alphabetChinese) {
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
        if (max_index >0 && (not (i>0 && max_index == last_index))  ){
//            std::cout <<  max_index - 1 << std::endl;
            std::string temp_str =  utf8_substr2(alphabetChinese,max_index - 1,1)  ;
            str_res.push_back(temp_str);
        }



        last_index = max_index;
    }
    return str_res;
}

void ncnn2cv(ncnn::Mat src, cv::Mat &score, cv::Mat &thre_img, const float thre_val = 0.7311) {
    float *srcdata = (float *) src.data;
    for (int i = 0; i < src.h; i++) {
        for (int j = 0; j < src.w; j++) {
            score.at<float>(i, j) = srcdata[i * src.w + j + src.w*src.h*5];
            
            if (srcdata[i * src.w + j + src.w*src.h*5 ] >= thre_val) {
                // std::cout << srcdata[i * src.w + j] << std::endl;
                thre_img.at<uchar>(i, j) = 255;
            } else {
                thre_img.at<uchar>(i, j) = 0;
            }
        }
    }
}


cv::Mat resize_img(cv::Mat src,const int long_size)
{
    int w = src.cols;
    int h = src.rows;
    // std::cout<<"原图尺寸 (" << w << ", "<<h<<")"<<std::endl;
    float scale = 1.f;
    if (w > h)
    {
        scale = (float)long_size / w;
        w = long_size;
        h = h * scale;
    }
    else
    {
        scale = (float)long_size / h;
        h = long_size;
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


std::vector<std::vector<cv::Point>> pse_deocde(const cv::Mat &score, const cv::Mat &thre,std::vector<cv::RotatedRect> &rects , const int scale, const float h_scale, const float w_scale) {
    int img_rows = score.rows;
    int img_cols = score.cols;
    // auto min_w_h = std::min(img_cols,img_rows);
    // min_w_h *= min_w_h / 20;
    auto min_w_h = 800;

    cv::Mat stats, centroids, label_img(thre.size(), CV_32S);
    // 二值化
    // cv::threshold(cv_img * 255, thre, 0, 255, cv::THRESH_OTSU);
    // 计算连通域ss
    int nLabels = connectedComponentsWithStats(thre, label_img, stats, centroids,4);
    // int nLabels = connectedComponents(thre, label_img  , 4);


    // std::cout << "nLabels:" << nLabels << std::endl;

    std::vector<float> angles;
    std::vector<std::vector<cv::Point>> bboxs;

    for (int label = 1; label < nLabels; label++) {
        float area = stats.at<int>(label, cv::CC_STAT_AREA);
        if (area < min_w_h / (scale * scale)) {
            // std::cout << "area:" << area << std::endl;
            // std::cout << "min_w_h:" << min_w_h / (scale * scale) << std::endl;
            continue;
        }
        // 计算该label的平均分数
        std::vector<float> scores;
        std::vector<cv::Point> points;
        for (int y = 0; y < img_rows; ++y) {
            for (int x = 0; x < img_cols; ++x) {
                if (label_img.at<int>(y, x) == label) {
                    scores.emplace_back(score.at<float>(y, x));
                    points.emplace_back(cv::Point(x, y));
                }
            }
        }

        //均值
        double sum = std::accumulate(std::begin(scores), std::end(scores), 0.0);
        // std::cout << "sum" << sum << std::endl;
        if (sum == 0) {
            continue;
        }
        double mean = sum / scores.size();
        // std::cout << "mean" << mean << std::endl;

        if (mean < 0.8) {
            continue;
        }
        cv::RotatedRect rect = cv::minAreaRect(points);
        
        float w = rect.size.width;
        float h = rect.size.height;
        float angle = rect.angle;

        // if (w < h) {
        //     std::swap(w, h);
        //     angle -= 90;
        // }
        // if (45 < std::abs(angle) && std::abs(angle) < 135) {
        //     std::swap(img_rows, img_cols);
        // }

        points.clear();
        // 对卡号进行限制，长宽比，卡号的宽度不能超过图片宽高的95%
        // if (w > h * 8 && w < img_cols * 0.95) {
        cv::Mat bbox;
        cv::boxPoints(rect, bbox);
        for (int i = 0; i < bbox.rows; ++i) {
            points.emplace_back(cv::Point(int(bbox.at<float>(i, 0) * w_scale), int(bbox.at<float>(i, 1) * h_scale)));
        }
        rect.size.width = rect.size.width * w_scale;
        rect.size.height = rect.size.height * h_scale;
        rect.center.x = rect.center.x * w_scale;
        rect.center.y = rect.center.y * h_scale;

        bboxs.emplace_back(points);
        angles.emplace_back(angle);
        rects.push_back(rect);
     
    }
    return bboxs;
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


void  OCR::detect(cv::Mat im_bgr,int long_size)
{

        // 图像缩放
    auto im = resize_img(im_bgr, long_size);


    float h_scale = im_bgr.rows * 1.0 / im.rows;
    float w_scale = im_bgr.cols * 1.0 / im.cols;

    ncnn::Mat in = ncnn::Mat::from_pixels(im.data, ncnn::Mat::PIXEL_BGR2RGB, im.cols, im.rows);
    in.substract_mean_normalize(mean_vals_pse_angle,norm_vals_pse_angle);

    // std::cout << "输入尺寸 (" << in.w << ", " << in.h << ")" << std::endl;

    ncnn::Extractor ex = psenet.create_extractor();
//    ex.set_num_threads(4);
    ex.input("input", in);
    ncnn::Mat preds;
    double time1 = static_cast<double>( cv::getTickCount());
    ex.extract("out", preds);
    std::cout << "psenet前向时间:" << (static_cast<double>( cv::getTickCount()) - time1) / cv::getTickFrequency() << "s" << std::endl;
    // std::cout << "网络输出尺寸 (" << preds.w << ", " << preds.h << ", " << preds.c << ")" << std::endl;

    time1 = static_cast<double>( cv::getTickCount());
    cv::Mat score = cv::Mat::zeros(preds.h, preds.w, CV_32FC1);
    cv::Mat thre = cv::Mat::zeros(preds.h, preds.w, CV_8UC1);
    std::vector<cv::RotatedRect> rects ; 
    ncnn2cv(preds, score, thre);
    auto bboxs = pse_deocde(score, thre, rects , 1, h_scale, w_scale);
    std::cout << "psenet decode 时间:" << (static_cast<double>( cv::getTickCount()) - time1) / cv::getTickFrequency() << "s" << std::endl;
    std::cout << "boxzie" << bboxs.size() << std::endl;

    auto result = draw_bbox(im_bgr, bboxs);
    cv::imwrite("./imgs/result.jpg", result);
    cv::imwrite("./imgs/net_result.jpg", score * 255);
    cv::imwrite("./imgs/net_thre.jpg", thre);
    time1 = static_cast<double>( cv::getTickCount());
    //开始行文本角度检测和文字识别
    for (int i = 0; i < rects.size() ; i++ ){
        cv::RotatedRect  temprect = rects[i];
        // std::cout<<  temprect.size.width << "," << temprect.size.height << "," <<  temprect.center.x <<
        // "," <<  temprect.center.y  << "," <<  temprect.angle << std::endl;
        // cv::Mat part_im = crop_text_area(temprect,im_bgr);
        cv::Mat part_im ;

        int  min_size   = temprect.size.width>temprect.size.height?temprect.size.height:temprect.size.width;
        temprect.size.width  =   int(temprect.size.width + min_size * 0.1);
        temprect.size.height =   int(temprect.size.height + min_size * 0.1);


        RRLib::getRotRectImg(temprect, im_bgr, part_im);

        int part_im_w = part_im.cols;
        int part_im_h = part_im.rows;
        // std::cout << "网络输出尺寸 (" << part_im_w<< ", " << part_im_h <<  ")" << std::endl;
        if (part_im_h > 1.5 *  part_im_w) part_im = matRotateClockWise90(part_im);

        cv::Mat angle_input = part_im.clone();
        // part_im_w = angle_input.cols;
        // part_im_h = angle_input.rows;
        // std::cout << "网络输出尺寸 2(" << part_im_w<< ", " << part_im_h <<  ")" << std::endl;
        // cv::imwrite("test.jpg",part_im);
        //分类
        ncnn::Mat  shufflenet_input = ncnn::Mat::from_pixels_resize(angle_input.data, 
                ncnn::Mat::PIXEL_BGR2RGB, angle_input.cols, part_im.rows ,shufflenetv2_target_w ,shufflenetv2_target_h );

        shufflenet_input.substract_mean_normalize(mean_vals_pse_angle,norm_vals_pse_angle );
        ncnn::Extractor shufflenetv2_ex = angle_net.create_extractor();
        //    shufflenetv2_ex.set_num_threads(4);
        shufflenetv2_ex.input("input", shufflenet_input);
        ncnn::Mat angle_preds;
        double time2 = static_cast<double>( cv::getTickCount());
        shufflenetv2_ex.extract("out", angle_preds);

        // std::cout << "anglenet前向时间:" << (static_cast<double>( cv::getTickCount()) - time2) / cv::getTickFrequency() << "s" << std::endl;
        // std::cout << "网络输出尺寸 (" << preds.w << ", " << preds.h << ", " << preds.c << ")" << std::endl;

        float *srcdata =(float*) angle_preds.data;

        int angle_index = 0;
        int max_value ;
        for (int i=0; i<angle_preds.w;i++){
            // std::cout << srcdata[i] << std::endl;
            if (i==0)max_value = srcdata[i];
            else if (srcdata[i] > angle_index) {
                angle_index = i ;
                max_value = srcdata[i];
            }
        }
        
        if (angle_index == 0 || angle_index ==2) part_im = matRotateClockWise180(part_im);

        // 开始文本识别
        int crnn_w_target ;
        float scale  = crnn_h * 1.0/ part_im.rows ;
        crnn_w_target = int(part_im.cols * scale ) ;

        char *svavePath = new char[25];
        sprintf( svavePath, "debug_im/%d.jpg", i);
        cv::imwrite(svavePath,part_im);
        // part_im = cv::imread("test.jpg");

        cv::Mat img2 = part_im.clone();

        ncnn::Mat  crnn_in = ncnn::Mat::from_pixels_resize(img2.data, 
                    ncnn::Mat::PIXEL_BGR2GRAY, img2.cols, img2.rows , crnn_w_target, crnn_h );

        // ncnn::Mat  crnn_in = ncnn::Mat::from_pixels_resize(part_im.data, 
        //             ncnn::Mat::PIXEL_BGR2GRAY, part_im.cols, part_im.rows , crnn_w_target, crnn_h );
        
        crnn_in.substract_mean_normalize(mean_vals_crnn,norm_vals_crnn );
       
        ncnn::Mat crnn_preds;

        //判断用横排还是竖排模型 { 0 : "hengdao",  1:"hengzhen",  2:"shudao",  3:"shuzhen"} #hengdao: 文本行横向倒立 其他类似
       
        // time1 = static_cast<double>( cv::getTickCount());
        // std::cout << angle_index << std::endl;
        if (angle_index ==0 || angle_index ==1 ){

            ncnn::Extractor crnn_ex = crnn_net.create_extractor();
            crnn_ex.input("input", crnn_in);
#if CRNN_LSTM
            // lstm

            ncnn::Mat blob162;
            crnn_ex.extract("234", blob162);

            // batch fc
            ncnn::Mat blob182(256, blob162.h);
            for (int i=0; i<blob162.h; i++)
            {
                ncnn::Extractor crnn_ex_1 = crnn_net.create_extractor();

                ncnn::Mat blob162_i = blob162.row_range(i, 1);
                crnn_ex_1.input("253", blob162_i);

                ncnn::Mat blob182_i;
                crnn_ex_1.extract("254", blob182_i);

                memcpy(blob182.row(i), blob182_i, 256 * sizeof(float));
            }

            // lstm
            ncnn::Mat blob243;
            crnn_ex.input("260", blob182);
            crnn_ex.extract("387", blob243);

            // batch fc
            ncnn::Mat blob263(5530, blob243.h);
            for (int i=0; i<blob243.h; i++)
            {
                ncnn::Extractor crnn_ex_2 = crnn_net.create_extractor();

                ncnn::Mat blob243_i = blob243.row_range(i, 1);
                crnn_ex_2.input("406", blob243_i);

                ncnn::Mat blob263_i;
                crnn_ex_2.extract("407", blob263_i);

                memcpy(blob263.row(i), blob263_i, 5530 * sizeof(float));
            }

            crnn_preds = blob263;
#else // CRNN_LSTM
            crnn_ex.extract("out", crnn_preds);
#endif // CRNN_LSTM
        }
        else{


            ncnn::Extractor crnn_ex = crnn_vertical_net.create_extractor();
            crnn_ex.input("input", crnn_in);
#if CRNN_LSTM
            // lstm

            ncnn::Mat blob162;
            crnn_ex.extract("234", blob162);

            // batch fc
            ncnn::Mat blob182(256, blob162.h);
            for (int i=0; i<blob162.h; i++)
            {
                ncnn::Extractor crnn_ex_1 = crnn_vertical_net.create_extractor();

                ncnn::Mat blob162_i = blob162.row_range(i, 1);
                crnn_ex_1.input("253", blob162_i);

                ncnn::Mat blob182_i;
                crnn_ex_1.extract("254", blob182_i);

                memcpy(blob182.row(i), blob182_i, 256 * sizeof(float));
            }

            // lstm
            ncnn::Mat blob243;
            crnn_ex.input("260", blob182);
            crnn_ex.extract("387", blob243);

            // batch fc
            ncnn::Mat blob263(5530, blob243.h);
            for (int i=0; i<blob243.h; i++)
            {
                ncnn::Extractor crnn_ex_2 = crnn_vertical_net.create_extractor();

                ncnn::Mat blob243_i = blob243.row_range(i, 1);
                crnn_ex_2.input("406", blob243_i);

                ncnn::Mat blob263_i;
                crnn_ex_2.extract("407", blob263_i);

                memcpy(blob263.row(i), blob263_i, 5530 * sizeof(float));
            }

            crnn_preds = blob263;
#else // CRNN_LSTM
            crnn_ex.extract("out", crnn_preds);
#endif // CRNN_LSTM
        }
         
       
    //    crnn_ex.set_num_threads(4);ss
     

        // std::cout << "前向时间:" << (static_cast<double>( cv::getTickCount()) - time1) / cv::getTickFrequency() << "s" << std::endl;
        // std::cout << "网络输出尺寸 (" << crnn_preds.w << ", " << crnn_preds.h << ", " << crnn_preds.c << ")" << std::endl;


        auto res_pre = crnn_deocde(crnn_preds,alphabetChinese);
        std::cout << "预测结果：";
        for (int i=0; i<res_pre.size();i++){
            std::cout << res_pre[i] ;
        }
        std::cout  <<std::endl;


    }
    std::cout << "角度检测和文字识别总时间:" << (static_cast<double>( cv::getTickCount()) - time1) / cv::getTickFrequency() << "s" << std::endl;
  


}


