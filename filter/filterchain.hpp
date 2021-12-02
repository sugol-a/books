#pragma once

#include <opencv2/imgproc.hpp>
#include <vector>
#include <map>

#include <filter/filter.hpp>

namespace filter {
    class FilterChain {
        public:
            FilterChain();
            cv::Mat apply_filters(cv::Mat& img);
            std::vector<cv::Mat> cached();
            size_t length() const;

        private:
            std::vector<std::unique_ptr<Filter>> m_filters;
            std::vector<cv::Mat> m_cache;
    };
}
