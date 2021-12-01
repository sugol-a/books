#pragma once
#include <filter/filter.hpp>

namespace filter {
    class CannyFilter : public Filter {
        public:
            ~CannyFilter();
            cv::Mat apply(cv::Mat const& img) override;

        private:
            void get_thresholds(cv::Mat const& img, double& low, double& high);
    };
}
