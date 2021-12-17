#include <imageexporterpool.hpp>

namespace worker {
    ImageExporterPool::ImageExporterPool(size_t n_workers) {
        m_active_workers = n_workers;

        for (size_t i = 0; i < n_workers; i++) {
            m_workers.push_back(std::make_unique<ImageExporter>());
        }
    }

    void ImageExporterPool::run_workers() {
        for (auto& worker : m_workers) {
            worker->run(this);
        }
    }
}
