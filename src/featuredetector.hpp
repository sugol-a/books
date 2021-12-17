/*
 * Copyright (C) 2021  Alister Sanders
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include <optional>

#include <featuredetectorparams.hpp>
#include <fitnessmetrics.hpp>
#include <imagedata.hpp>
#include <worker.hpp>

namespace worker {
    class FeatureDetector : public Worker<img::ImageData, img::ImageData> {
        public:
            const double CV_IMAGE_TARGET_DIMENSION = 400.0;

            FeatureDetector(const std::vector<ft::FitnessMetric>& fitness_metrics,
                            const std::vector<float>& fitness_metric_weights,
                            const FeatureDetectorParams& params);

            void run(IWorkerPool* wp) override;

        private:
            cv::UMat apply_filters(const cv::UMat& mat);
            void find_features(img::ImageData& image_data);
            double resize_mat(const cv::UMat& src, cv::UMat& dest);
            float calculate_fitness(const cv::Mat& mat, const util::Box& box);
            std::optional<img::ImageData::Feature> best_candidate_box(std::vector<img::ImageData::Feature>& features);

        private:
            std::vector<ft::FitnessMetric> m_fitness_metrics;
            std::vector<float> m_fitness_weights;
            const FeatureDetectorParams& m_params;
    };
}
