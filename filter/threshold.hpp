#pragma once

#include <filter/filter.hpp>

namespace filter {
    class ThresholdFilter : public Filter {
        public:
            ~ThresholdFilter();
            cv::Mat apply(cv::Mat const& img) override;
    };
}
