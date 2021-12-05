#pragma once

#include <workerpool.hpp>
#include <imageloader.hpp>

namespace worker {
    class ImageLoaderPool : public WorkerPool<ImageLoader> {
        public:
            ImageLoaderPool(size_t n_workers);

            std::shared_ptr<InputQueue> input() override;
            std::shared_ptr<OutputQueue> output() override;

            void run_workers() override;
            void join_all() override;

        private:
            std::shared_ptr<InputQueue> m_input_queue;
            std::shared_ptr<OutputQueue> m_output_queue;
            std::vector<std::unique_ptr<ImageLoader>> m_workers;
    };
}
