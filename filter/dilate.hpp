#pragma once

#include <filter/filter.hpp>

namespace filter {
    class DilateFilter : public Filter {
        public:
            ~DilateFilter();
            cv::Mat apply(cv::Mat const& img) override;
    };
}
