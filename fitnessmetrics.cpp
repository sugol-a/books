#include <cmath>

#include <fitnessmetrics.hpp>

#include <iostream>

namespace ft {
    static double map(double from_min, double from_max, double to_min, double to_max, double value) {
        double from_range = from_max - from_min;
        double to_range = to_max - to_min;

        return ((value / from_range) * to_range) + to_min;
    }

    double sigmoid(double x) {
        return 1.0 / (1.0 + exp(-x));
    }

    FitnessMetric aspect_ratio(double aspect) {
        // Returns the absolute difference between the ideal aspect ratio and
        // the actual aspect ratio of the bouding box
        return [=](const cv::Mat& img, const util::Box& box) {
            double box_aspect = box.width() / box.height();
            return abs(box_aspect - aspect);
        };
    }

    FitnessMetric distance_to(util::Point<double> target) {
        // Returns the distance from the midpoint of the bounding box and the
        // given target point
        return [=](const cv::Mat& img, const util::Box& box) {
            // Use normalised coordinates so that this metric isn't affected by
            // the actual size of the image

            util::Point<double> midpoint = {
                map(0, img.size().width, -1.0, 1.0, box.midpoint().x),
                map(0, img.size().height, -1.0, 1.0, box.midpoint().y)
            };

            return midpoint.distance_to(target);
        };
    }

    FitnessMetric relative_area(double ideal_area) {
        // Returns the area of the box relative to the area of the image
        return [=](const cv::Mat& img, const util::Box& box) {
            double relative_area = double(box.area()) / double(img.size().area());
            return abs(relative_area - ideal_area);
       };
    }
}
