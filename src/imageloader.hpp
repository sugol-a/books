#pragma once

#include <imagedata.hpp>
#include <worker.hpp>

namespace worker {
    class ImageLoader : public Worker<std::string, img::ImageData> {
        public:
            void run(IWorkerPool* wp) override;
    };
}
