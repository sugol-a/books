#include "filter/canny.hpp"
#include "filter/medianblur.hpp"
#include <filter/filterchain.hpp>

#include <filter/filters_all.hpp>

namespace filter {
    FilterChain::FilterChain() {
        // Construct the default filter chain
        // TODO: Allow for custom filter chains

        m_filters.emplace_back(new BGR2GreyFilter);
        m_filters.emplace_back(new MedianBlurFilter(11));
        m_filters.emplace_back(new NormaliseFilter);
        m_filters.emplace_back(new DilateFilter(4));
        m_filters.emplace_back(new ThresholdFilter);
        m_filters.emplace_back(new CannyFilter);
    }

    FilterChain::~FilterChain() {
        for (auto filter : m_filters){
            delete filter;
        }
    }

    cv::Mat FilterChain::apply_filters(const cv::Mat& img) {
        cv::Mat filter_result = img;
        for (auto& filter : m_filters) {
            filter_result = filter->apply(filter_result);
        }

        return filter_result;
    }
}
