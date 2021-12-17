/*
 * Copyright (C) 2021  Alister Sanders
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <iostream>
#include <filesystem>
#include <imageexporterpool.hpp>
#include <imageexporter.hpp>

namespace fs { using path = std::filesystem::path; }

namespace worker {
    void ImageExporter::run(IWorkerPool* wp) {
        auto work_fn = [=] {
            ImageExporterPool* worker_pool = static_cast<ImageExporterPool*>(wp);
            std::shared_ptr<InputQueue> input_queue = worker_pool->input();
            std::shared_ptr<OutputQueue> output_queue = worker_pool->output();

            std::shared_ptr<std::pair<std::shared_ptr<img::ImageData>, ExportParameters>> image = nullptr;

            while ((image = input_queue->pop()) != nullptr) {
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

                output_queue->push(nullptr);
            }

            worker_pool->signal_done();
        };

        m_thread = std::thread(work_fn);
    }
}
