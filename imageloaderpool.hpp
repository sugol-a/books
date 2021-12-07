#pragma once

#include <workerpool.hpp>
#include <imageloader.hpp>

namespace worker {
    class ImageLoaderPool : public WorkerPool<ImageLoader> {
        public:
            ImageLoaderPool() { }
            ImageLoaderPool(size_t n_workers);
            ~ImageLoaderPool();

            std::shared_ptr<InputQueue> input() override;
            std::shared_ptr<OutputQueue> output() override;
            void set_input(std::shared_ptr<InputQueue> in) override;
            void set_output(std::shared_ptr<OutputQueue> out) override;

            void run_workers() override;
            void join_all() override;
            void signal_done() override;

        private:
            std::size_t m_active_workers;
            mutable std::mutex m_mutex;
            std::shared_ptr<InputQueue> m_input_queue;
            std::shared_ptr<OutputQueue> m_output_queue;
            std::vector<std::unique_ptr<ImageLoader>> m_workers;
    };
}
