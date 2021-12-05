#include <imageloaderpool.hpp>

namespace worker {
    ImageLoaderPool::ImageLoaderPool(size_t n_workers) {
        m_input_queue = std::make_shared<InputQueue>();
        m_output_queue = std::make_shared<OutputQueue>();

        // Create the workers
        for (size_t i = 0; i < n_workers; i++) {
            m_workers.push_back(std::make_unique<ImageLoader>(m_input_queue, m_output_queue));
        }
    }

    std::shared_ptr<ImageLoaderPool::InputQueue> ImageLoaderPool::input() {
        return m_input_queue;
    }

    std::shared_ptr<ImageLoaderPool::OutputQueue> ImageLoaderPool::output() {
        return m_output_queue;
    }

    void ImageLoaderPool::run_workers() {
        for (auto& worker : m_workers) {
            worker->run();
        }
    }

    void ImageLoaderPool::join_all() {
        for (auto& worker : m_workers) {
            worker->join();
        }
    }
}
