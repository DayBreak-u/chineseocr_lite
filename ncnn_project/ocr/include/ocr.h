#ifndef __OCR_H__
#define __OCR_H__
using namespace std;


#define  COL_DBNET_OP_PARAM         "dbnet_op.param"
#define  COL_DBNET_OP_BIN           "dbnet_op.bin"

#define  COL_CRNN_LITE_PARAM        "crnn_lite_op.param"
#define  COL_CRNN_LITE_BIN          "crnn_lite_op.bin"

#define  COL_ANGLE_OP_PARAM         "angle_op.param"
#define  COL_ANGLE_OP_BIN           "angle_op.bin"


#define  COL_KEYS_FILE               "keys.txt"

struct Bbox
{
    int x1;
    int y1;
    int x2;
    int y2;
    int x3;
    int y3;
    int x4;
    int y4;
};

class OCR
{
    public:
        OCR();
        OCR(const char* szModelDir);
        bool Init(const char* szModelDir);
        void detect(cv::Mat im_bgr,int short_size);
        void dbnet_decode(cv::Mat im_bgr,int long_size);

        void set_verbose(bool bVerbose)
        {
            m_bVerbose = bVerbose;
        }

        std::vector<string>& GetResult() { return m_Result; }
    private:
       
        std::vector<string> m_Result;
        bool m_bReady;

        bool m_bVerbose;

        ncnn::Net  dbnet,crnn_net,angle_net;
        ncnn::Mat  img;
        int num_thread = 4;
        int angle_target_w  = 192;
        int angle_target_h  = 32;
        int crnn_h = 32;

        const float mean_vals_dbnet[3] = { 0.485 * 255, 0.456 * 255, 0.406 * 255 };
        const float norm_vals_dbnet[3] = { 1.0 / 0.229 / 255.0, 1.0 / 0.224 / 255.0, 1.0 / 0.225 / 255.0 };
        const float unclip_ratio = 2.0;
        const float box_thresh = 0.5;
        const float thresh = 0.3;
        const int min_size = 3;

        const float mean_vals_crnn_angle[3] = { 127.5, 127.5, 127.5};
        const float norm_vals_crnn_angle[3] = { 1.0 /127.5,1.0 /127.5,1.0 /127.5};

        std::vector<std::string>   alphabetChinese;


};




#endif