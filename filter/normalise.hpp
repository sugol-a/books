#ifndef _NORMALISE_H
#define _NORMALISE_H

#include <filter/filter.hpp>

namespace filter {
    class NormaliseFilter : public Filter {
        public:
            ~NormaliseFilter();
            cv::Mat apply(cv::Mat const& img) override;
    };
}

#endif