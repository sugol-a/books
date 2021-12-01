#include "opencv2/imgproc.hpp"
#include <filter/bgr2grey.hpp>

namespace filter {
    BGR2GreyFilter::~BGR2GreyFilter() {}

    cv::Mat BGR2GreyFilter::apply(const cv::Mat &img) {
        cv::Mat img_grey;
        cv::cvtColor(img, img_grey, cv::COLOR_BGR2GRAY);

        return img_grey;
    }
}