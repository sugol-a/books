#pragma once

#include <imageexporter.hpp>
#include <workerpool.hpp>

namespace worker {
    class ImageExporterPool : public WorkerPool<ImageExporter> {
        public:
            ImageExporterPool() { };

            ImageExporterPool(size_t n_workers);

            ~ImageExporterPool();

            std::shared_ptr<InputQueue> input() override;
            std::shared_ptr<OutputQueue> output() override;
            void set_input(std::shared_ptr<InputQueue> in) override;
            void set_output(std::shared_ptr<OutputQueue> out) override;

            void run_workers() override;
            void join_all() override;
            void signal_done() override;

        private:
            std::shared_ptr<InputQueue> m_input_queue;
            std::shared_ptr<OutputQueue> m_output_queue;
            mutable std::mutex m_mutex;
            size_t m_active_workers;
            std::vector<std::unique_ptr<ImageExporter>> m_workers;
    };
}
