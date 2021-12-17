#pragma once

#include <featuredetector.hpp>
#include <featuredetectorparams.hpp>
#include <workerpool.hpp>

namespace worker {
    class FeatureDetectorPool : public WorkerPool<FeatureDetector> {
        public:
            FeatureDetectorPool() { }
            FeatureDetectorPool(size_t n_workers,
                                std::vector<ft::FitnessMetric> fitness_metrics,
                                std::vector<float> fitness_weights,
                                FeatureDetectorParams params = FeatureDetectorParams());

            void run_workers() override;

        private:
            FeatureDetectorParams m_params;
    };
}
