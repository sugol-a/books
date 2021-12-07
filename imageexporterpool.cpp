#include <imageexporterpool.hpp>

namespace worker {
    ImageExporterPool::ImageExporterPool(size_t n_workers) {
        m_active_workers = n_workers;
        m_input_queue = std::make_shared<InputQueue>();
        m_output_queue = std::make_shared<OutputQueue>();

        for (size_t i = 0; i < n_workers; i++) {
            m_workers.push_back(std::make_unique<ImageExporter>(m_input_queue, m_output_queue));
        }
    }

    ImageExporterPool::~ImageExporterPool() {
        join_all();
    }

    std::shared_ptr<ImageExporterPool::InputQueue> ImageExporterPool::input() {
        return m_input_queue;
    }

    std::shared_ptr<ImageExporterPool::OutputQueue> ImageExporterPool::output() {
        return m_output_queue;
    }

    void ImageExporterPool::set_input(std::shared_ptr<InputQueue> in) {
        m_input_queue = in;
    }

    void ImageExporterPool::set_output(std::shared_ptr<OutputQueue> out) {
        m_output_queue = out;
    }

    void ImageExporterPool::run_workers() {
        for (auto& worker : m_workers) {
            worker->run(this);
        }
    }

    void ImageExporterPool::join_all() {
        for (auto& worker : m_workers) {
            worker->join();
        }
    }

    void ImageExporterPool::signal_done() {
        std::lock_guard lock(m_mutex);
        m_active_workers--;

        if (m_active_workers == 0) {
            m_output_queue->finish();
        }
    }
}
