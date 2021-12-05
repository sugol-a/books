#include <iostream>
#include <imageloader.hpp>

namespace worker {
    ImageLoader::ImageLoader(std::shared_ptr<InputQueue> input_queue,
                             std::shared_ptr<OutputQueue> output_queue) {
        set_input_queue(input_queue);
        set_output_queue(output_queue);
    }

    void ImageLoader::run() {
        auto work_fn = [&] {
            std::shared_ptr<std::string> filename = nullptr;
            while ((filename = m_input_queue->pop()) != nullptr) {
                std::shared_ptr<img::ImageData> image = std::make_shared<img::ImageData>(*filename);
                m_output_queue->push(std::move(image));
            }

            if (m_input_queue->empty())
                m_output_queue->close();
        };

        m_thread = std::thread(work_fn);
    }
}
