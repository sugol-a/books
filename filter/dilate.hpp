#pragma once

#include <filter/filter.hpp>

namespace filter {
    class DilateFilter : public Filter {
        public:
            DilateFilter(int kernel_size = 8);
            ~DilateFilter();
            cv::Mat apply(cv::Mat const& img) override;

        private:
            int m_kernelSize;
    };
}
