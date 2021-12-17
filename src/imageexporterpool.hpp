#pragma once

#include <imageexporter.hpp>
#include <workerpool.hpp>

namespace worker {
    class ImageExporterPool : public WorkerPool<ImageExporter> {
        public:
            ImageExporterPool() { };
            ImageExporterPool(size_t n_workers);

            void run_workers() override;
    };
}
