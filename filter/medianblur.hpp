#ifndef _MEDIANBLUR
#define _MEDIANBLUR

#include <filter/filter.hpp>

namespace filter {
    class MedianBlurFilter : public Filter {
        public:
            MedianBlurFilter(int kernel_size = 11);
            ~MedianBlurFilter();
            cv::Mat apply(cv::Mat const& img) override;

        private:
            int m_kernel_size;
    };
}

#endif