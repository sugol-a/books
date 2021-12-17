#include <imageloaderpool.hpp>

namespace worker {
    ImageLoaderPool::ImageLoaderPool(size_t n_workers) {
        m_active_workers = n_workers;

        // Create the workers
        for (size_t i = 0; i < n_workers; i++) {
            m_workers.push_back(std::make_unique<ImageLoader>());
        }
    }

    void ImageLoaderPool::run_workers() {
        for (auto& worker : m_workers) {
            worker->run(this);
        }
    }
}
