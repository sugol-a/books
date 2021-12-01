#include "opencv2/core.hpp"
#include "opencv2/core/base.hpp"
#include <filter/normalise.hpp>

namespace filter {
    NormaliseFilter::~NormaliseFilter() {}

    cv::Mat NormaliseFilter::apply(const cv::Mat& img) {
        cv::Mat normalised;
        cv::normalize(img, normalised, 255, 0, cv::NORM_MINMAX);
        return normalised;
    }
}