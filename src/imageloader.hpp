#pragma once

#include <imagedata.hpp>
#include <worker.hpp>

namespace worker {
    class ImageLoader : public Worker<std::string, img::ImageData> {
        public:
            ImageLoader(std::shared_ptr<InputQueue> input_queue,
                        std::shared_ptr<OutputQueue> output_queue);

            void run(IWorkerPool* wp) override;
    };
}
