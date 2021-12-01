#pragma once

#include <filter/filter.hpp>

namespace filter {
    class NormaliseFilter : public Filter {
        public:
            ~NormaliseFilter();
            cv::Mat apply(cv::Mat const& img) override;
    };
}
