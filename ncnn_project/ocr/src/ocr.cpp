#include "ocr.h"
#include "ZUtil.h"
#include <queue>



OCR::OCR()
{
    dbnet.load_param("../../models/dbnet_op.param");
    dbnet.load_model("../../models/dbnet_op.bin");

    crnn_net.load_param("../../models/crnn_lite_op.param");
    crnn_net.load_model("../../models/crnn_lite_op.bin");

    angle_net.load_param("../../models/angle_op.param");
    angle_net.load_model("../../models/angle_op.bin");

    //load keys
    ifstream in("../../models/keys.txt");
	std::string filename;
	std::string line;

	if(in) // 有该文件
	{
		while (getline (in, line)) // line中不包括每行的换行符
		{
            alphabetChinese.push_back(line);
		}
		alphabetChinese.push_back(" ");
		alphabetChinese.push_back("·");
	}
	else // 没有该文件
	{
		std::cout <<"no txt file" << std::endl;
	}
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
        if (max_index >0 && (not (i>0 && max_index == last_index))  ){
//            std::cout <<  max_index - 1 << std::endl;
//            std::string temp_str =  utf8_substr2(alphabetChinese,max_index - 1,1)  ;
            str_res.push_back(alphabetChinese[max_index-1]);
        }



        last_index = max_index;
    }
    return str_res;
}



cv::Mat resize_img(cv::Mat src,const int short_size)
{
    int w = src.cols;
    int h = src.rows;
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




void  OCR::detect(cv::Mat im_bgr,int short_size)
{

        // 图像缩放
    auto im = resize_img(im_bgr, short_size);

    int wid = im.cols;
    int hi = im.rows;
    int srcwid = im_bgr.cols;
    int srchi = im_bgr.rows;

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

    std::cout << "dbnet decode 时间:" << (static_cast<double>( cv::getTickCount()) - time1) / cv::getTickFrequency() << "s" << std::endl;
    std::cout << "boxzie" << boxes.size() << std::endl;

//    auto result = draw_bbox(im_bgr, boxes);
//    cv::imwrite("./imgs/result.jpg", result);

    time1 = static_cast<double>( cv::getTickCount());
    //开始行文本角度检测和文字识别
    std::cout << "预测结果：\n";
    for (int i = boxes.size() - 1; i >=0 ; i-- ){
        std::vector<cv::Point>   temp_box = boxes[i];

        cv::Mat part_im ;
        part_im = GetRotateCropImage(im_bgr,temp_box);
        int part_im_w = part_im.cols;
        int part_im_h = part_im.rows;

         // 开始文本识别
        int crnn_w_target ;
        float scale  = crnn_h * 1.0/ part_im_h ;
        crnn_w_target = int(part_im.cols * scale ) ;

//        char *svavePath = new char[25];
//        sprintf( svavePath, "debug_im/%d.jpg", i);
//        cv::imwrite(svavePath,part_im);
        // part_im = cv::imread("test.jpg");

        cv::Mat img2 = part_im.clone();

        ncnn::Mat  crnn_in = ncnn::Mat::from_pixels_resize(img2.data,
                    ncnn::Mat::PIXEL_BGR2RGB, img2.cols, img2.rows , crnn_w_target, crnn_h );

        //角度检测
        int crnn_w = crnn_in.w;
        int crnn_h = crnn_in.h;

        //ncnn::Mat angle_in ;
        //if (crnn_w >= angle_target_w) copy_cut_border(crnn_in,angle_in,0,0,0,crnn_w-angle_target_w);
        //else copy_make_border(crnn_in,angle_in,0,0,0,angle_target_w - crnn_w,0,255.f);

        //angle_in.substract_mean_normalize(mean_vals_crnn_angle,norm_vals_crnn_angle );


//         ncnn::Extractor angle_ex = angle_net.create_extractor();
//         angle_ex.set_num_threads(num_thread);
//         angle_ex.input("input", angle_in);
//         ncnn::Mat angle_preds;

//         angle_ex.extract("out", angle_preds);

//         float *srcdata =(float*) angle_preds.data;

//         float angle_score = srcdata[0];
//         //判断方向
//         if (angle_score < 0.5) part_im = matRotateClockWise180(part_im);

        //crnn识别
        crnn_in.substract_mean_normalize(mean_vals_crnn_angle,norm_vals_crnn_angle );
       
        ncnn::Mat crnn_preds;


        ncnn::Extractor crnn_ex = crnn_net.create_extractor();
        crnn_ex.set_num_threads(num_thread);
        crnn_ex.input("input", crnn_in);


        ncnn::Mat blob162;
        crnn_ex.extract("1000", blob162);

        ncnn::Mat blob263(5531, blob162.h);
        //batch fc
        for (int i=0; i<blob162.h; i++)
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


        auto res_pre = crnn_deocde(crnn_preds,alphabetChinese);

        for (int i=0; i<res_pre.size();i++){
            std::cout << res_pre[i] ;
        }
        std::cout  <<std::endl;


    }
    std::cout << "文字识别总时间:" << (static_cast<double>( cv::getTickCount()) - time1) / cv::getTickFrequency() << "s" << std::endl;
  


}


