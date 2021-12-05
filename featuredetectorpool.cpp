#include <featuredetectorpool.hpp>

namespace worker {
    FeatureDetectorPool::FeatureDetectorPool(size_t n_workers,
                                             const filter::FilterChain& filter_chain,
                                             std::vector<ft::FitnessMetric> fitness_metrics,
                                             std::vector<float> fitness_weights) {
        m_input_queue = std::make_shared<InputQueue>();
        m_output_queue = std::make_shared<OutputQueue>();

        for (size_t i = 0; i < n_workers; i++) {
            m_workers.push_back(std::make_unique<FeatureDetector>(m_input_queue,
                                                                  m_output_queue,
                                                                  filter_chain,
                                                                  fitness_metrics,
                                                                  fitness_weights));
        }
    }

    std::shared_ptr<FeatureDetectorPool::InputQueue> FeatureDetectorPool::input() {
        return m_input_queue;
    }

    std::shared_ptr<FeatureDetectorPool::OutputQueue> FeatureDetectorPool::output() {
        return m_output_queue;
    }

    void FeatureDetectorPool::set_input(std::shared_ptr<InputQueue> in) {
        m_input_queue = in;

        for (auto& worker : m_workers) {
            worker->set_input_queue(m_input_queue);
        }
    }

    void FeatureDetectorPool::set_output(std::shared_ptr<OutputQueue> out) {
        m_output_queue = out;
    }

    void FeatureDetectorPool::run_workers() {
        for (auto& worker : m_workers) {
            worker->run();
        }
    }

    void FeatureDetectorPool::join_all() {
        for (auto& worker : m_workers) {
            worker->join();
        }
    }
}