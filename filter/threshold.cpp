#include <filter/threshold.hpp>

namespace filter {
    ThresholdFilter::~ThresholdFilter() {}

    cv::Mat ThresholdFilter::apply(const cv::Mat &img) {
        cv::Mat threshold_img;
        cv::threshold(img, threshold_img, 60, 255, cv::THRESH_BINARY);
        return threshold_img;
    }
}
