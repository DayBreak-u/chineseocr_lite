#include "ocr.h"

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s  /path/to/image/file  longsize \n", argv[0]);
        std::cout << "eg: " << argv[0] << " e:\\temp\\test.jpg  560" << std::endl;
        return -1;
    }

    const char *imagepath = argv[1];
    cv::Mat im_bgr = cv::imread(imagepath, 1);
    if (im_bgr.empty()) {
        fprintf(stderr, "cv::imread %s failed\n", imagepath);
        return -1;
    }

    const int long_size = atoi(argv[2]);
    OCR *ocrengine = new OCR();
    
    ocrengine->detect(im_bgr,long_size);

    delete ocrengine;
    return 0;
}
