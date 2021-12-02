#include <filter/dilate.hpp>>

namespace filter {
    DilateFilter::~DilateFilter() {}

    cv::Mat DilateFilter::apply(const cv::Mat &img) {
        cv::Mat dilated;
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(60, 60));

        cv::dilate(img, dilated, kernel);

        return dilated;
    }
}
