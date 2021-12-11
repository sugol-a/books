#pragma once

#include <imagedata.hpp>
#include <worker.hpp>

namespace worker {
    struct ExportParameters {
            std::string export_directory;
            std::string output_file;
            cv::Rect crop;
            bool do_crop;
    };

    class ImageExporter : public Worker<std::pair<std::shared_ptr<img::ImageData>, ExportParameters>, void*> {
        public:
            ImageExporter(std::shared_ptr<InputQueue> input_queue,
                          std::shared_ptr<OutputQueue> output_queue);

            void run(IWorkerPool* wp) override;
    };
}
