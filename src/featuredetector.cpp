#include <cmath>
#include <iostream>
#include <opencv2/objdetect.hpp>

#include <featuredetectorpool.hpp>
#include <workerpool.hpp>
#include <featuredetector.hpp>

namespace worker {
    // TODO: Ensure that fitness_metrics and fitness_metric_weights have the
    // same size
    FeatureDetector::FeatureDetector(const std::vector<ft::FitnessMetric>& fitness_metrics,
                                     const std::vector<float>& fitness_metric_weights,
                                     const FeatureDetectorParams& params)
        : m_fitness_metrics(fitness_metrics),
          m_fitness_weights(fitness_metric_weights),
          m_params(params) {
    }

    void FeatureDetector::run(IWorkerPool* wp) {
        auto work_fn = [=] {
            FeatureDetectorPool* worker_pool = static_cast<FeatureDetectorPool*>(wp);
            std::shared_ptr<InputQueue> input_queue = worker_pool->input();
            std::shared_ptr<OutputQueue> output_queue = worker_pool->output();

            std::shared_ptr<img::ImageData> image_data = nullptr;

            while ((image_data = input_queue->pop()) != nullptr) {
                find_features(*image_data);

                // We're done with the image data for now, we only need to
                // reload it for display/export purposes
                image_data->unload();
                output_queue->push(std::move(image_data));
            }

            // Tell the pool this worker is finished
            worker_pool->signal_done();
        };

        m_thread = std::thread(work_fn);
    }

    cv::UMat FeatureDetector::apply_filters(const cv::UMat& mat) {
        // TODO: Remove magic numbers
        cv::Mat dilate_kernel = cv::getStructuringElement(cv::MORPH_RECT,
                                                          cv::Size(m_params.dilate_kernel_size,
                                                                   m_params.dilate_kernel_size));
        cv::UMat result;

        cv::cvtColor(mat, result, cv::COLOR_RGB2GRAY);
        cv::medianBlur(result, result, m_params.blur_kernel_size);
        cv::normalize(result, result, 255, 0, cv::NORM_MINMAX);
        cv::dilate(result, result, dilate_kernel);
        cv::threshold(result, result, m_params.threshold, 255, cv::THRESH_BINARY);
        cv::Canny(result, result, 0.0, 1.0);

        return result;
    }

    void FeatureDetector::find_features(img::ImageData& image_data) {
        cv::UMat image_umat = image_data.mat().getUMat(cv::ACCESS_READ);
        cv::UMat result;
        double image_scale = resize_mat(image_umat, result);
        result = apply_filters(result);

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(result, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

        std::vector<cv::Rect> bounding_boxes;
        for (const auto& c : contours) {
            util::Box bounding_box = cv::boundingRect(c);

            // Scale the bounding box up to fit the original image
            bounding_box.top_left().scale(1 / image_scale);
            bounding_box.bottom_right().scale(1 / image_scale);

            bounding_boxes.push_back(bounding_box);
        }

        cv::groupRectangles(bounding_boxes, 0);
        std::vector<img::ImageData::Feature> features;

        // Calculate the fitness score for each bounding box
        for (auto& bounding_box : bounding_boxes) {
            float fitness = calculate_fitness(image_data.mat(), bounding_box);
            features.push_back(img::ImageData::Feature(fitness, bounding_box));
        }

        image_data.set_features(features);

        std::optional<img::ImageData::Feature> candidate = best_candidate_box(features);
        if (candidate)
            image_data.set_candidate(candidate.value());
    }

    double FeatureDetector::resize_mat(const cv::UMat& src, cv::UMat& dest) {
        // Resize the image mat such that its largest dimension is
        // CV_IMAGE_TARGET_DIMENSION -- this improves throughput
        double max_dimension = std::max(src.size().width, src.size().height);
        double image_scale = CV_IMAGE_TARGET_DIMENSION / max_dimension;

        cv::Size new_size(image_scale * double(src.size().width),
                          image_scale * double(src.size().height));

        cv::resize(src, dest, new_size);
        return image_scale;
    }

    float FeatureDetector::calculate_fitness(const cv::Mat& mat, const util::Box& box) {
        float weighted_sum = 0;

        for (size_t i = 0; i < m_fitness_metrics.size(); i++) {
            float weight = m_fitness_weights[i];
            const ft::FitnessMetric& metric = m_fitness_metrics[i];

            weighted_sum += weight * metric(mat, box);
        }

        // Use the sigmoid function to map the weighted sum to (0, 1)
        return 1 / (1 + expf(weighted_sum));
    }

    std::optional<img::ImageData::Feature> FeatureDetector::best_candidate_box(std::vector<img::ImageData::Feature>& features) {
        if (features.size() == 0) {
            return {};
        }

        // Select the feature with the highest fitness value
        return *std::max_element(features.begin(),
                                 features.end(),
                                 [](auto& f1, auto& f2) {
                                     return f1.first < f2.first;
                                 });
    }
}
