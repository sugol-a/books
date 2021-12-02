#include <filter/dilate.hpp>

namespace filter {
    DilateFilter::DilateFilter(int kernel_size)
     : m_kernelSize(kernel_size) { }

    DilateFilter::~DilateFilter() {}

    cv::Mat DilateFilter::apply(const cv::Mat &img) {
        cv::Mat dilated;
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(m_kernelSize, m_kernelSize));

        cv::dilate(img, dilated, kernel);

        return dilated;
    }
}
