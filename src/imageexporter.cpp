#include <iostream>
#include <filesystem>
#include <imageexporterpool.hpp>
#include <imageexporter.hpp>

namespace fs { using path = std::filesystem::path; }

namespace worker {
    ImageExporter::ImageExporter(std::shared_ptr<InputQueue> input_queue,
                                 std::shared_ptr<OutputQueue> output_queue) {
        set_input_queue(input_queue);
        set_output_queue(output_queue);
    }

    void ImageExporter::run(IWorkerPool* wp) {
        auto work_fn = [=] {
            auto worker_pool = static_cast<ImageExporterPool*>(wp);
            std::shared_ptr<std::pair<std::shared_ptr<img::ImageData>, ExportParameters>> image = nullptr;

            while ((image = m_input_queue->pop()) != nullptr) {
                auto image_data = image->first;
                auto params = image->second;
                util::Box crop = params.crop;

                // Clip the crop rectangle to the image size
                image_data->load(image_data->filename());

                cv::Mat output_image;
                if (params.do_crop && crop.area() > 0) {
                    output_image = image_data->mat()(crop);
                } else {
                    output_image = image_data->mat();
                }

                fs::path output_path = fs::path(params.export_directory) / fs::path(params.output_file);
                cv::cvtColor(output_image, output_image, cv::COLOR_RGB2BGR);
                cv::imwrite(output_path.string(), output_image);
                image_data->unload();

                m_output_queue->push(nullptr);
            }

            worker_pool->signal_done();
        };

        m_thread = std::thread(work_fn);
    }
}
