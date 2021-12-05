#include <cmath>
#include <opencv2/objdetect.hpp>
#include <featuredetector.hpp>

namespace worker {
    // TODO: Ensure that fitness_metrics and fitness_metric_weights have the
    // same size
    FeatureDetector::FeatureDetector(std::shared_ptr<InputQueue> input_queue,
                                     std::shared_ptr<OutputQueue> output_queue,
                                     const filter::FilterChain& filter_chain,
                                     const std::vector<ft::FitnessMetric>& fitness_metrics,
                                     const std::vector<float>& fitness_metric_weights)
        : m_filter_chain(filter_chain),
          m_fitness_metrics(fitness_metrics),
          m_fitness_weights(fitness_metric_weights) {
        set_input_queue(input_queue);
        set_output_queue(output_queue);
    }

    void FeatureDetector::run() {
        auto work_fn = [&] {
            std::shared_ptr<img::ImageData> image_data = nullptr;

            while ((image_data = m_input_queue->pop()) != nullptr) {
                find_features(*image_data);

                // We're done with the image data for now, we only need to
                // reload it for display/export purposes
                image_data->unload();

                m_output_queue->push(image_data);
            }

            if (m_input_queue->empty())
                m_output_queue->close();
        };

        m_thread = std::thread(work_fn);
    }

    void FeatureDetector::find_features(img::ImageData& image_data) {
        cv::Mat result;
        double image_scale = resize_mat(image_data.mat(), result);
        result = m_filter_chain.apply_filters(result);

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
        image_data.set_candidate(best_candidate_box(features));
    }

    double FeatureDetector::resize_mat(const cv::Mat& src, cv::Mat& dest) {
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

    img::ImageData::Feature& FeatureDetector::best_candidate_box(std::vector<img::ImageData::Feature>& features) {
        // Select the feature with the highest fitness value
        return *std::max_element(features.begin(),
                                 features.end(),
                                 [](auto& f1, auto& f2) {
                                     return f1.first < f2.first;
                                 });
    }
}
