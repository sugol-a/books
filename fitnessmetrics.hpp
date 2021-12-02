#pragma once

#include <functional>

#include <opencv2/imgproc.hpp>

#include <util/box.hpp>
#include <util/box.hpp>

namespace ft {
    using FitnessMetric = std::function<double(const cv::Mat&, const util::Box&)>;

    double sigmoid(double x);

    FitnessMetric aspect_ratio(double aspect);
    FitnessMetric distance_to(util::Point<double> target);
    FitnessMetric relative_area(double ideal_area);
}
