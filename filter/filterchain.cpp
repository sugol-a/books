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

    cv::Mat FilterChain::apply_filters(cv::Mat& img) {
        // Clear the previous results
        m_cache.clear();

        cv::Mat filter_result = img;
        for (auto& filter : m_filters) {
            filter_result = filter->apply(filter_result);
            m_cache.push_back(filter_result);
        }

        return filter_result;
    }

    std::vector<cv::Mat> FilterChain::cached() {
        return m_cache;
    }

    size_t FilterChain::length() const {
        return m_filters.size();
    }
}
