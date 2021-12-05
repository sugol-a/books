#pragma once

#include <opencv2/imgproc.hpp>

namespace filter {
    class Filter {
        public:
            virtual ~Filter() { };
            virtual cv::Mat apply(cv::Mat const& img) = 0;
    };
}
