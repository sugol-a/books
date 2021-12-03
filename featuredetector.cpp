#include <opencv2/objdetect.hpp>

#include <featuredetector.hpp>

namespace ft {
    FeatureDetector::FeatureDetector() {
        m_filterChain = std::make_shared<filter::FilterChain>();
    }

    FeatureDetector::FeatureDetector(std::shared_ptr<filter::FilterChain> filter_chain)
        : m_filterChain(filter_chain) {
    }

    std::vector<std::pair<double, util::Box>>
    FeatureDetector::find_candidate_features(const cv::Mat& image,
                                             const std::vector<FitnessMetric>& fitness_metrics,
                                             const std::vector<double>& metric_weights,
                                             double image_scale_factor)
    {
        // Scale the image down in order to decrease processing time
        cv::Mat filtered = image;
        cv::Size new_size = cv::Size(image.size().width * image_scale_factor, image.size().height * image_scale_factor);
        cv::resize(image, filtered, new_size);
        filtered = m_filterChain->apply_filters(filtered);

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(filtered, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

        std::vector<cv::Rect> bounding_boxes;
        for (const auto& c : contours) {
            // Since we scaled the image down, the bounding rects need to be
            // scaled up by the same amount
            util::Box bounding_box = cv::boundingRect(c);
            bounding_box.top_left().scale(1 / image_scale_factor);
            bounding_box.bottom_right().scale(1 / image_scale_factor);

            bounding_boxes.push_back(bounding_box);
        }

        // TODO Make eps parameter configurable
        cv::groupRectangles(bounding_boxes, 0, 20);

        std::vector<std::pair<double, util::Box>> features;

        // Store the min/max fitness values for normalisation
        double fitness_min = INFINITY;
        double fitness_max = -INFINITY;
        // Calculate the fitness for each feature
        for (const auto& box : bounding_boxes) {
            double fitness = calculate_fitness(image, box, fitness_metrics, metric_weights);
            if (fitness < fitness_min) {
                fitness_min = fitness;
            }

            if (fitness > fitness_max) {
                fitness_max = fitness;
            }

            features.push_back(std::pair<double, util::Box>(fitness, box));
        }

        // Normalise the fitness values
        double fitness_range = fitness_max - fitness_min;
        for (auto& feature : features) {
            feature.first -= fitness_min;
            feature.first /= fitness_range;
        }

        return features;
    }

    double FeatureDetector::calculate_fitness(const cv::Mat& image,
                                              const util::Box& box,
                                              const std::vector<FitnessMetric>& fitness_metrics,
                                              const std::vector<double>& metric_weights)
    {
        double weighted_sum = 0;
        for (size_t i = 0; i < fitness_metrics.size(); i++) {
            double weight = metric_weights[i];
            FitnessMetric metric = fitness_metrics[i];

            weighted_sum += weight * metric(image, box);
        }

        // A greater weighted sum indicates lower fitness, so give -weighted_sum
        return -weighted_sum;
    }

    util::Box FeatureDetector::best_candidate(std::vector<std::pair<double, util::Box>> const& features) {
        // Return the feature with the highest fitness
        return std::max_element(features.begin(),
                        features.end(),
                        [](const auto& f1, const auto& f2) {
                            return f1.first < f2.first;
                        })->second;
    }

    const std::shared_ptr<filter::FilterChain> FeatureDetector::filter_chain() {
        return m_filterChain;
    }
}
