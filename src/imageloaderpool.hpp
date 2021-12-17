#pragma once

#include <workerpool.hpp>
#include <imageloader.hpp>

namespace worker {
    class ImageLoaderPool : public WorkerPool<ImageLoader> {
        public:
            ImageLoaderPool() { }
            ImageLoaderPool(size_t n_workers);
            void run_workers() override;
    };
}
