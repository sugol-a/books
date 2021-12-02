#pragma once

#include <memory>
#include <utility>

#include <filter/filterchain.hpp>
#include <util/box.hpp>
#include <fitnessmetrics.hpp>

namespace ft {
    class FeatureDetector {
        public:
            FeatureDetector();
            FeatureDetector(std::shared_ptr<filter::FilterChain> filter_chain);

            // Detect features an the image, and their associated fitness
            // according to the provided fitness metrics
            std::vector<std::pair<double, util::Box>> find_candidate_features(const cv::Mat& image,
                                                                              const std::vector<FitnessMetric>& fitness_metrics,
                                                                              const std::vector<double>& metric_weights,
                                                                              double image_scale_factor = 0.5);
            util::Box best_candidate(std::vector<std::pair<double, util::Box>> const& features);

            const std::shared_ptr<filter::FilterChain> filter_chain();

        private:
            double calculate_fitness(const cv::Mat& image,
                                     const util::Box& box,
                                     const std::vector<FitnessMetric>& fitness_metrics,
                                     const std::vector<double>& metric_weights);

        private:
            std::shared_ptr<filter::FilterChain> m_filterChain;
    };
}
