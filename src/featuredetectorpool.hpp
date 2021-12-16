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

            ~FeatureDetectorPool();

            std::shared_ptr<InputQueue> input() override;
            std::shared_ptr<OutputQueue> output() override;
            void set_input(std::shared_ptr<InputQueue> in) override;
            void set_output(std::shared_ptr<OutputQueue> out) override;

            void run_workers() override;
            void join_all() override;
            void signal_done() override;
            void stop() override;
            bool stopped() override;

        private:
            std::shared_ptr<InputQueue> m_input_queue;
            std::shared_ptr<OutputQueue> m_output_queue;
            mutable std::mutex m_mutex;
            size_t m_active_workers;
            std::vector<std::unique_ptr<FeatureDetector>> m_workers;
            FeatureDetectorParams m_params;
            bool m_stopped;
    };
}