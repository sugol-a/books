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
            // TODO: ImageExporterPool
            auto worker_pool = static_cast<ImageExporterPool*>(wp);
            std::shared_ptr<std::pair<std::shared_ptr<img::ImageData>, ExportParameters>> image = nullptr;

            while ((image = m_input_queue->pop()) != nullptr) {
                auto image_data = image->first;
                auto params = image->second;
                util::Box crop = image_data->candidate().second;
                util::Box bounds;

                image_data->load(image_data->filename());
                bounds.top_left() = { 0, 0 };
                bounds.bottom_right() = { image_data->pixbuf()->get_width(), image_data->pixbuf()->get_height() };

                crop.expand(params.margin, bounds);

                cv::Mat output_image;
                if (params.do_crop && crop.area() > 0) {
                    try {
                        output_image = image_data->mat()(crop);
                    } catch (cv::Exception& e) {
                        std::cout << crop.top_left().x << " " << crop.top_left().y << "    "
                                  << crop.bottom_right().x << " " << crop.bottom_right().y << std::endl;
                        std::cout << image_data->mat().size().width << " "  << image_data->mat().size().height << std::endl;
                    }
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
