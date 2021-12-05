#pragma once

#include <filter/filterchain.hpp>
#include <featuredetector.hpp>
#include <workerpool.hpp>

namespace worker {
    class FeatureDetectorPool : public WorkerPool<FeatureDetector> {
        public:
            FeatureDetectorPool() { }
            FeatureDetectorPool(size_t n_workers,
                                const filter::FilterChain& filter_chain,
                                std::vector<ft::FitnessMetric> fitness_metrics,
                                std::vector<float> fitness_weights);

            std::shared_ptr<InputQueue> input() override;
            std::shared_ptr<OutputQueue> output() override;
            void set_input(std::shared_ptr<InputQueue> in) override;
            void set_output(std::shared_ptr<OutputQueue> out) override;

            void run_workers() override;
            void join_all() override;

        private:
            std::shared_ptr<InputQueue> m_input_queue;
            std::shared_ptr<OutputQueue> m_output_queue;
            std::vector<std::unique_ptr<FeatureDetector>> m_workers;
    };
}
