#pragma once

#include <filter/filter.hpp>

namespace filter {
    class BGR2GreyFilter : public Filter {
        public:
            ~BGR2GreyFilter();
            cv::Mat apply(cv::Mat const& img) override;
    };
}
