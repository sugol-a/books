#include <featuredetectorpool.hpp>

namespace worker {
    FeatureDetectorPool::FeatureDetectorPool(size_t n_workers,
                                             std::vector<ft::FitnessMetric> fitness_metrics,
                                             std::vector<float> fitness_weights,
                                             FeatureDetectorParams params)
        : m_params(params) {
        m_active_workers = n_workers;

        for (size_t i = 0; i < n_workers; i++) {
            m_workers.push_back(std::make_unique<FeatureDetector>(fitness_metrics,
                                                                  fitness_weights,
                                                                  m_params));
        }
    }

    void FeatureDetectorPool::run_workers() {
        for (auto& worker : m_workers) {
            worker->run(this);
        }
    }
}
