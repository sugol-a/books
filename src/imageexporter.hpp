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
            void run(IWorkerPool* wp) override;
    };
}
