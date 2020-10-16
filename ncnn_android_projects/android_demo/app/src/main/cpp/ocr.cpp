#include "ocr.h"

#include <android/log.h>
#define TAG "ocrCpp"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)
#include "RRLib.cpp"
void OCR::pse_deocde(ncnn::Mat & features,
                std::map<int, std::vector<cv::Point>>& contours_map,
                const float thresh,//0.73
                const float min_area,//10
                const float ratio//1
)
{

    /// get kernels
    float *srcdata = (float *) features.data;
    std::vector<cv::Mat> kernels;

    float _thresh = thresh;
    cv::Mat scores = cv::Mat::zeros(features.h, features.w, CV_32FC1);
    //channel
    for (int c = features.c - 1; c >= 0; --c){
        //每一个通道channel都设置阈值
        cv::Mat kernel(features.h, features.w, CV_8UC1);//1通道的特征图
        for (int i = 0; i < features.h; i++) {
            for (int j = 0; j < features.w; j++) {
                //最后一个通道获得score得分，浮点数
                if (c==features.c - 1) scores.at<float>(i, j) = srcdata[i * features.w + j + features.w*features.h*c ] ;

                if (srcdata[i * features.w + j + features.w*features.h*c ] >= _thresh) {
                    // std::cout << srcdata[i * src.w + j] << std::endl;
                    kernel.at<uint8_t>(i, j) = 1;
                } else {
                    kernel.at<uint8_t>(i, j) = 0;
                }

            }
        }
        kernels.push_back(kernel);//每一个通道，根据阈值 thresh 0 1 二值化
        _thresh = thresh * ratio;
    }


    /// make label
    cv::Mat label;
    std::map<int, int> areas;
    std::map<int, float> scores_sum;
    cv::Mat mask(features.h, features.w, CV_32S, cv::Scalar(0));
    cv::connectedComponents(kernels[features.c  - 1], label, 4);
    //最后一个通道做连通域分析 4邻域



    for (int y = 0; y < label.rows; ++y) {
        for (int x = 0; x < label.cols; ++x) {
            int value = label.at<int32_t>(y, x);//获得每一个像素的标签
            float score = scores.at<float>(y,x);//获得每一个像素的得分
            if (value == 0) continue;//0为背景
            areas[value] += 1;//统计属于同一label的像素点个数

            scores_sum[value] += score;//统计得分
        }
    }

    std::queue<cv::Point> queue, next_queue;

    for (int y = 0; y < label.rows; ++y) {

        for (int x = 0; x < label.cols; ++x) {
            int value = label.at<int>(y, x);

            if (value == 0) continue;
            if (areas[value] < min_area) {
                areas.erase(value);
                continue;
            }

            if (scores_sum[value]*1.0 /areas[value] < 0.93  )
            {
                areas.erase(value);
                scores_sum.erase(value);
                continue;
            }
            cv::Point point(x, y);
            queue.push(point);
            mask.at<int32_t>(y, x) = value;
        }
    }

    /// growing text line
    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};

    for (int idx = features.c  - 2; idx >= 0; --idx) {
        while (!queue.empty()) {
            cv::Point point = queue.front(); queue.pop();
            int x = point.x;
            int y = point.y;
            int value = mask.at<int32_t>(y, x);

            bool is_edge = true;
            for (int d = 0; d < 4; ++d) {
                int _x = x + dx[d];
                int _y = y + dy[d];

                if (_y < 0 || _y >= mask.rows) continue;
                if (_x < 0 || _x >= mask.cols) continue;
                if (kernels[idx].at<uint8_t>(_y, _x) == 0) continue;
                if (mask.at<int32_t>(_y, _x) > 0) continue;

                cv::Point point_dxy(_x, _y);
                queue.push(point_dxy);

                mask.at<int32_t>(_y, _x) = value;
                is_edge = false;
            }

            if (is_edge) next_queue.push(point);
        }
        std::swap(queue, next_queue);
    }

    /// make contoursMap
    for (int y=0; y < mask.rows; ++y)
        for (int x=0; x < mask.cols; ++x) {
            int idx = mask.at<int32_t>(y, x);
            if (idx == 0) continue;
            contours_map[idx].emplace_back(cv::Point(x, y));
            //把属于一块的像素添加到contorus中
        }
}

OCR::OCR(AAssetManager *mgr,const std::vector<ncnn::Mat> param_files, const std::vector<ncnn::Mat> bin_files){

//    int ret1 = psenet.load_param((const unsigned char *)param_files[0]);
//    int ret2 =  psenet.load_model((const unsigned char *)bin_files[0]);
//    int ret3 = crnn_net.load_param((const unsigned char *)param_files[1]);
//    int ret4= crnn_net.load_model((const unsigned char *)bin_files[1]);
    int ret1 = psenet.load_param(mgr,"psenet_lite_mbv2.param");
    int ret2 = psenet.load_model(mgr,"psenet_lite_mbv2.bin");
    int ret3 = crnn_net.load_param(mgr,"crnn_lite_lstm_v2.param");
    int ret4 = crnn_net.load_model(mgr,"crnn_lite_lstm_v2.bin");

    LOGD("############### %d  %d  %d  %d", ret1,ret2,ret3,ret4);
    //load keys
    ifstream in("keys.txt");
    std::string filename;
    std::string line;

    if(in) // 有该文件
    {
        LOGD(" txt file  found");
        while (getline (in, line)) // line中不包括每行的换行符
        {
            alphabetChinese.push_back(line);
        }
    }
    else // 没有该文件
    {
        //std::cout <<"no txt file" << std::endl;
        LOGD(" txt file not found");
    }


}


std::vector<std::vector<int>> OCR::Detect(cv::Mat im_bgr,int length) {
    std::vector<std::vector<int>> ocrRes;
    cv::Mat im = OCR::resize_img(im_bgr, length);

    float h_scale = im_bgr.rows * 1.0 / im.rows;
    float w_scale = im_bgr.cols * 1.0 / im.cols;
    //ncnn 读入图片
    ncnn::Mat in = ncnn::Mat::from_pixels(im.data, ncnn::Mat::PIXEL_BGR2RGB, im.cols, im.rows);
    in.substract_mean_normalize(mean_vals_pse_angle,norm_vals_pse_angle);
    LOGD("input size : %d, %d, %d", in.w, in.h, in.c);

    ncnn::Extractor ex = psenet.create_extractor();
    ex.set_num_threads(num_thread);
    ex.input("input", in);
    ncnn::Mat out;
    ex.extract("out", out);
    LOGD("output size : %d, %d, %d", out.w, out.h, out.c);

    double time1 = static_cast<double>( cv::getTickCount());
    //std::cout << "psenet前向时间:" << (static_cast<double>( cv::getTickCount()) - time1) / cv::getTickFrequency() << "s" << std::endl;
    LOGD("psenet前向时间: : %f s", (static_cast<double>( cv::getTickCount()) - time1) / cv::getTickFrequency());

    time1 = static_cast<double>( cv::getTickCount());
    std::map<int, std::vector<cv::Point>> contoursMap;

    pse_deocde(out, contoursMap, 0.6311, 10, 1);

    std::vector<std::vector<cv::Point>> bboxs;
    std::vector<cv::RotatedRect> rects ;
    for (auto &cnt: contoursMap) {
        cv::Mat bbox;
        cv::RotatedRect rect = cv::minAreaRect(cnt.second);//获得最小外接矩形
        rect.size.width = rect.size.width * w_scale;
        rect.size.height = rect.size.height * h_scale;
        rect.center.x = rect.center.x * w_scale;
        rect.center.y = rect.center.y * h_scale;//尺寸恢复
        rects.push_back(rect);
        cv::boxPoints(rect, bbox);//查找旋转的矩形的四个顶点。绘制旋转的矩形很有用。
        std::vector<cv::Point> points;
        for (int i = 0; i < bbox.rows; ++i) {
            points.emplace_back(cv::Point(int(bbox.at<float>(i, 0) ), int(bbox.at<float>(i, 1) )));
        }
        bboxs.emplace_back(points);

    }
    LOGD("psenet decode 时间 : %f s", (static_cast<double>( cv::getTickCount()) - time1) / cv::getTickFrequency());
    //std::cout << "psenet decode 时间:" << (static_cast<double>( cv::getTickCount()) - time1) / cv::getTickFrequency() << "s" << std::endl;
    //std::cout << "boxzie" << bboxs.size() << std::endl;
    //LOGD("boxzie %d ", bboxs.size());

    //auto result = draw_bbox(im_bgr, bboxs);
    time1 = static_cast<double>( cv::getTickCount());
    //处理每一个旋转矩形 识别
    RRLib *rlib = new RRLib();
    LOGD("start crnn");
    for (int i = 0; i < rects.size() ; i++ ){
        cv::RotatedRect  temprect = rects[i];
        cv::Mat part_im ;
        int  min_size   = temprect.size.width>temprect.size.height?temprect.size.height:temprect.size.width;
        temprect.size.width  =   int(temprect.size.width + min_size * 0.15);
        temprect.size.height =   int(temprect.size.height + min_size * 0.15);

        rlib->getRotRectImg(temprect, im_bgr, part_im);

        // 开始文本识别
        int crnn_w_target ;
        float scale  = crnn_h * 1.0/ part_im.rows ;
        crnn_w_target = int(part_im.cols * scale ) ;
        cv::Mat img2 = part_im.clone();

        ncnn::Mat  crnn_in = ncnn::Mat::from_pixels_resize(img2.data,
                                                           ncnn::Mat::PIXEL_BGR2GRAY, img2.cols, img2.rows , crnn_w_target, crnn_h );
        crnn_in.substract_mean_normalize(mean_vals_crnn,norm_vals_crnn );

        ncnn::Mat crnn_preds;

        // lstm
        ncnn::Extractor crnn_ex = crnn_net.create_extractor();
        crnn_ex.set_num_threads(num_thread);
        crnn_ex.input("input", crnn_in);
        ncnn::Mat blob162;
        crnn_ex.extract("234", blob162);

        // batch fc
        ncnn::Mat blob182(256, blob162.h);
        for (int i=0; i<blob162.h; i++)
        {
            ncnn::Extractor crnn_ex_1 = crnn_net.create_extractor();
            crnn_ex_1.set_num_threads(num_thread);
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
            crnn_ex_2.set_num_threads(num_thread);
            ncnn::Mat blob243_i = blob243.row_range(i, 1);
            crnn_ex_2.input("406", blob243_i);

            ncnn::Mat blob263_i;
            crnn_ex_2.extract("407", blob263_i);

            memcpy(blob263.row(i), blob263_i, 5530 * sizeof(float));
        }

        crnn_preds = blob263;

        //auto res_pre = crnn_deocde(crnn_preds,alphabetChinese);
        auto res_pre = crnn_deocde(crnn_preds,alphabetChinese);
        ocrRes.emplace_back(res_pre);
        // std::cout << "前向时间:" << (static_cast<double>( cv::getTickCount()) - time1) / cv::getTickFrequency() << "s" << std::endl;
        // std::cout << "网络输出尺寸 (" << crnn_preds.w << ", " << crnn_preds.h << ", " << crnn_preds.c << ")" << std::endl;
        LOGD("前向时间: %f s",(static_cast<double>( cv::getTickCount()) - time1) / cv::getTickFrequency());
        LOGD("网络输出尺寸 : %d  %d    %d   ",crnn_preds.w,crnn_preds.h,crnn_preds.c);


    }
    //LOGD("ocrRes :  %d   ",ocrRes.size());
    return ocrRes;
}

//std::vector<std::string> OCR::crnn_deocde(const ncnn::Mat score , std::vector<std::string> alphabetChinese) {
//    float *srcdata = (float* ) score.data;
//    std::vector<std::string> str_res;
//    int last_index = 0;
//    for (int i = 0; i < score.h;i++){
//        int max_index = 0;
//
//        float max_value = -1000;
//        for (int j =0; j< score.w; j++){
//            if (srcdata[ i * score.w + j ] > max_value){
//                max_value = srcdata[i * score.w + j ];
//                max_index = j;
//            }
//        }
//        if (max_index >0 && (not (i>0 && max_index == last_index))  ){
////            std::cout <<  max_index - 1 << std::endl;
////            std::string temp_str =  utf8_substr2(alphabetChinese,max_index - 1,1)  ;
//            str_res.push_back(alphabetChinese[max_index-1]);
//        }
//
//
//
//        last_index = max_index;
//    }
//    return str_res;
//}

std::vector<int> OCR::crnn_deocde(const ncnn::Mat score , std::vector<std::string> alphabetChinese) {
    float *srcdata = (float* ) score.data;
    std::vector<int> str_res;
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
//            std::string temp_str =  utf8_substr2(alphabetChinese,max_index - 1,1)  ;
            str_res.push_back(max_index-1);
            LOGD("crnn decode %d   ", max_index-1);
        }
        last_index = max_index;
    }
    return str_res;
}

cv::Mat OCR::draw_bbox(cv::Mat &src, const std::vector<std::vector<cv::Point>> &bboxs) {
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

cv::Mat OCR::resize_img(cv::Mat src,const int long_size)
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




