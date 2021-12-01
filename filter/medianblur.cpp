#include "opencv2/imgproc.hpp"
#include <filter/medianblur.hpp>

namespace filter {
    MedianBlurFilter::MedianBlurFilter(int kernel_size)
    : m_kernel_size(kernel_size) { }

    MedianBlurFilter::~MedianBlurFilter() {}

    cv::Mat MedianBlurFilter::apply(const cv::Mat& img) {
        cv::Mat blurred;
        cv::medianBlur(img, blurred, m_kernel_size);

        return blurred;
    }
}