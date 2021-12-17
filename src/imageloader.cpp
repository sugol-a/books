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

#include <imageloaderpool.hpp>
#include <imageloader.hpp>

namespace worker {
    void ImageLoader::run(IWorkerPool* wp) {
        auto work_fn = [=] {
            ImageLoaderPool* worker_pool = static_cast<ImageLoaderPool*>(wp);
            std::shared_ptr<InputQueue> input_queue = worker_pool->input();
            std::shared_ptr<OutputQueue> output_queue = worker_pool->output();

            std::shared_ptr<std::string> filename = nullptr;
            while ((filename = input_queue->pop()) != nullptr) {
                std::shared_ptr<img::ImageData> image = std::make_shared<img::ImageData>(*filename);
                output_queue->push(std::move(image));
            }

            // This worker is finished
            worker_pool->signal_done();
        };

        m_thread = std::thread(work_fn);
    }
}
