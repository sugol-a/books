#include "opencv2/imgproc.hpp"
#include <filter/canny.hpp>

namespace filter {
    CannyFilter::~CannyFilter() {
    }

    cv::Mat CannyFilter::apply(const cv::Mat& img) {
        double low_thres, high_thres;
        get_thresholds(img, low_thres, high_thres);

        cv::Mat canny;
        cv::Canny(img, canny, low_thres, high_thres);

        return canny;
    }

    void CannyFilter::get_thresholds(const cv::Mat& img, double& low, double& high) {
        cv::Mat dummy;

        high = cv::threshold(img, dummy, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
        low = 0.5 * high;
    }
}