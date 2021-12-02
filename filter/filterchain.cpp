#include "filter/canny.hpp"
#include "filter/medianblur.hpp"
#include <filter/filterchain.hpp>

#include <filter/filters_all.hpp>

namespace filter {
    FilterChain::FilterChain() {
        // Construct the default filter chain
        // TODO: Allow for custom filter chains

        m_filters.emplace_back(new BGR2GreyFilter);
        m_filters.emplace_back(new MedianBlurFilter(17));
        m_filters.emplace_back(new NormaliseFilter);
        m_filters.emplace_back(new DilateFilter);
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

    const cv::Mat& FilterChain::get_cached(size_t index) {
        return m_cache[index];
    }

    size_t FilterChain::length() const {
        return m_filters.size();
    }
}
