#include <imageloaderpool.hpp>
#include <imageloader.hpp>

namespace worker {
    ImageLoader::ImageLoader(std::shared_ptr<InputQueue> input_queue,
                             std::shared_ptr<OutputQueue> output_queue) {
        set_input_queue(input_queue);
        set_output_queue(output_queue);
    }

    void ImageLoader::run(IWorkerPool* wp) {
        auto work_fn = [=] {
            ImageLoaderPool* worker_pool = static_cast<ImageLoaderPool*>(wp);

            std::shared_ptr<std::string> filename = nullptr;
            while ((filename = m_input_queue->pop()) != nullptr) {
                std::shared_ptr<img::ImageData> image = std::make_shared<img::ImageData>(*filename);
                m_output_queue->push(std::move(image));
            }

            // This worker is finished
            worker_pool->signal_done();
        };

        m_thread = std::thread(work_fn);
    }
}
