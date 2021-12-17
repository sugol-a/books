#include <imageloaderpool.hpp>

namespace worker {
    ImageLoaderPool::ImageLoaderPool(size_t n_workers) {
        m_active_workers = n_workers;
        m_stopped = false;
        m_input_queue = std::make_shared<InputQueue>();
        m_output_queue = std::make_shared<OutputQueue>();

        // Create the workers
        for (size_t i = 0; i < n_workers; i++) {
            m_workers.push_back(std::make_unique<ImageLoader>());
        }
    }

    ImageLoaderPool::~ImageLoaderPool() {
        join_all();
    }

    std::shared_ptr<ImageLoaderPool::InputQueue> ImageLoaderPool::input() {
        return m_input_queue;
    }

    std::shared_ptr<ImageLoaderPool::OutputQueue> ImageLoaderPool::output() {
        return m_output_queue;
    }

    void ImageLoaderPool::set_input(std::shared_ptr<InputQueue> in) {
        m_input_queue = in;
    }

    void ImageLoaderPool::set_output(std::shared_ptr<OutputQueue> out) {
        m_output_queue = out;
    }

    void ImageLoaderPool::run_workers() {
        for (auto& worker : m_workers) {
            worker->run(this);
        }
    }

    void ImageLoaderPool::join_all() {
        for (auto& worker : m_workers) {
            worker->join();
        }
    }

    void ImageLoaderPool::signal_done() {
        std::lock_guard lock(m_mutex);
        m_active_workers--;

        // We're done producing, so signal the consumers to stop consuming once
        // the output queue runs out
        if (m_active_workers == 0) {
            m_output_queue->finish();
        }
    }

    void ImageLoaderPool::stop() {
        m_input_queue->finish();
        m_input_queue->clear();
        m_output_queue->finish();
        m_output_queue->clear();

        m_stopped = true;
    }

    bool ImageLoaderPool::stopped() {
        return m_stopped;
    }
}
