#ifndef _BGR2GREY
#define _BGR2GREY

#include <filter/filter.hpp>

namespace filter {
    class BGR2GreyFilter : public Filter {
        public:
            ~BGR2GreyFilter();
            cv::Mat apply(cv::Mat const& img) override;
    };
}

#endif