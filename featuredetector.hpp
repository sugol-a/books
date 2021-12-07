#pragma once

#include <fitnessmetrics.hpp>
#include <imagedata.hpp>
#include <worker.hpp>

namespace worker {
    class FeatureDetector : public Worker<img::ImageData, img::ImageData> {
        public:
            const double CV_IMAGE_TARGET_DIMENSION = 400.0;

            FeatureDetector(std::shared_ptr<InputQueue> input_queue,
                            std::shared_ptr<OutputQueue> output_queue,
                            const std::vector<ft::FitnessMetric>& fitness_metrics,
                            const std::vector<float>& fitness_metric_weights);

            void run(IWorkerPool* wp) override;

        private:
            cv::UMat apply_filters(const cv::UMat& mat);
            void find_features(img::ImageData& image_data);
            double resize_mat(const cv::UMat& src, cv::UMat& dest);
            float calculate_fitness(const cv::Mat& mat, const util::Box& box);
            img::ImageData::Feature& best_candidate_box(std::vector<img::ImageData::Feature>& features);

        private:
            std::vector<ft::FitnessMetric> m_fitness_metrics;
            std::vector<float> m_fitness_weights;
    };
}
