
#pragma once

#include <opencv2/opencv.hpp>

template <typename T>
double Distance(T& a, T& b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

class Polygon {
public:
    Polygon(cv::Mat& boxPts, cv::Size side, float scaleX=1, float scaleY=1) {
        for (int row=0; row < boxPts.rows; ++row) {
            auto x = boxPts.at<float>(row, 0) * scaleX;
            auto y = boxPts.at<float>(row, 1) * scaleY;
            if (x < 0) x = 0;
            if (y < 0) y = 0;
            if (x >= side.width) x = side.width - 1;
            if (y >= side.height) y = side.height - 1;
            cv::Point2f pt(x, y);
            if (row == 0) lb = pt;
            else if (row == 1) lt = pt;
            else if (row == 2) rt = pt;
            else if (row == 3) rb = pt;
            else throw std::range_error("check boxPts mast be (4, 2)");
        }
        vec2f = {lt, rt, rb, lb};
        SortVertex();
    }

    std::vector<cv::Point2f> ToVec2f() { return vec2f; }

    std::vector<cv::Point2i> ToVec2i() {
        std::vector<cv::Point2i> vec2i(vec2f.size());
        for (int i=0; i < vec2f.size(); ++i) {
            auto x = (int) round(vec2f[i].x);
            auto y = (int) round(vec2f[i].y);
            vec2i[i] = cv::Point2i(x, y);
        }
        return vec2i;
    }

    float Area() {
        float area = 0.0;
        auto num = (int) vec2f.size();
        for (int i=num-1, j=0; j < num; i=j++) {
            area += vec2f[i].x * vec2f[j].y;
            area -= vec2f[i].y * vec2f[j].x;
        }
        return area;
    }

    /**
    * @brief resort poly vertex like (lt, rt, rb, lb)
    */
    void SortVertex() {
        int minAxis = 0;
        float minSum = -1;

        for (int i=0; i < vec2f.size(); ++i) {
            float sum = vec2f[i].x + vec2f[i].y;
            if (minSum < 0 || minSum > sum) {
                minSum = sum;
                minAxis = i;
            }
        }

        std::vector<cv::Point2f> vertex({vec2f[(minAxis + 0) % 4], vec2f[(minAxis + 1) % 4],
                                         vec2f[(minAxis + 2) % 4], vec2f[(minAxis + 3) % 4]});

        if (fabs(vertex[0].x - vertex[1].x) > fabs(vertex[0].y - vertex[1].y)) {
            vertex.swap(vec2f);
        } else {
            vec2f[0] = vertex[0];
            vec2f[1] = vertex[3];
            vec2f[2] = vertex[2];
            vec2f[3] = vertex[1];
        }
    }

    std::vector<cv::Point2f> ToQuadROI() {
        std::vector<cv::Point2f> quad;
        auto height = (int) round(fmax(Distance(vec2f[0], vec2f[3]), Distance(vec2f[1], vec2f[2])));
        auto width = (int) round(fmax(Distance(vec2f[0], vec2f[1]), Distance(vec2f[2], vec2f[3])));
        quad.emplace_back(cv::Point2f(0, 0));
        quad.emplace_back(cv::Point2f(width, 0));
        quad.emplace_back(cv::Point2f(width, height));
        quad.emplace_back(cv::Point2f(0, height));
        return quad;
    }

protected:
    cv::Point2f lb;
    cv::Point2f lt;
    cv::Point2f rt;
    cv::Point2f rb;
    std::vector<cv::Point2f> vec2f;
};


