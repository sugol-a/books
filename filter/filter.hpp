#ifndef _FILTER_H
#define _FILTER_H

#include <opencv2/imgproc.hpp>

namespace filter {
    class Filter {
        public:
            virtual ~Filter();
            virtual cv::Mat apply(cv::Mat const& img) = 0;
    };
}

#endif