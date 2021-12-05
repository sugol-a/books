#pragma once

#include <opencv2/imgproc.hpp>
#include <vector>
#include <map>

#include <filter/filter.hpp>

namespace filter {
    class FilterChain {
        public:
            FilterChain();
            ~FilterChain();
            cv::Mat apply_filters(const cv::Mat& img);

        private:
            std::vector<Filter*> m_filters;
    };
}
