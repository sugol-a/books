// #pragma once

// #include <memory>
// #include <utility>

// #include <filter/filterchain.hpp>
// #include <util/box.hpp>
// #include <fitnessmetrics.hpp>

// namespace ft {
//     class FeatureDetector {
//         public:
//             FeatureDetector();
//             FeatureDetector(std::shared_ptr<filter::FilterChain> filter_chain);

//             // Detect features an the image, and their associated fitness
//             // according to the provided fitness metrics
//             std::vector<std::pair<double, util::Box>> find_candidate_features(const cv::Mat& image,
//                                                                               const std::vector<FitnessMetric>& fitness_metrics,
//                                                                               const std::vector<double>& metric_weights,
//                                                                               double image_scale_factor = 0.5);
//             util::Box best_candidate(std::vector<std::pair<double, util::Box>> const& features);

//             const std::shared_ptr<filter::FilterChain> filter_chain();

//         private:
//             double calculate_fitness(const cv::Mat& image,
//                                      const util::Box& box,
//                                      const std::vector<FitnessMetric>& fitness_metrics,
//                                      const std::vector<double>& metric_weights);

//         private:
//             std::shared_ptr<filter::FilterChain> m_filterChain;
//     };
// }

#pragma once

#include <filter/filterchain.hpp>
#include <fitnessmetrics.hpp>
#include <imagedata.hpp>
#include <worker.hpp>

namespace worker {
    class FeatureDetector : public Worker<img::ImageData, img::ImageData> {
        public:
            const double CV_IMAGE_TARGET_DIMENSION = 400.0;

            FeatureDetector(std::shared_ptr<InputQueue> input_queue,
                            std::shared_ptr<OutputQueue> output_queue,
                            const filter::FilterChain& filter_chain,
                            const std::vector<ft::FitnessMetric>& fitness_metrics,
                            const std::vector<float>& fitness_metric_weights);

            void run() override;

        private:
            void find_features(img::ImageData& image_data);
            double resize_mat(const cv::Mat& src, cv::Mat& dest);
            float calculate_fitness(const cv::Mat& mat, const util::Box& box);
            img::ImageData::Feature& best_candidate_box(std::vector<img::ImageData::Feature>& features);

        private:
            filter::FilterChain m_filter_chain;
            std::vector<ft::FitnessMetric> m_fitness_metrics;
            std::vector<float> m_fitness_weights;
    };
}
