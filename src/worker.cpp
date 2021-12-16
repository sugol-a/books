#include <worker.hpp>

namespace worker {
    template<typename I, typename O>
    Worker<I, O>::~Worker() {
        m_thread.join();
    }

    template<typename I, typename O>
    void Worker<I,O>::set_input_queue(std::shared_ptr<InputQueue> input_queue) {
        m_input_queue = input_queue;
    }

    template <typename I, typename O>
    void Worker<I,O>::set_output_queue(std::shared_ptr<OutputQueue> output_queue) {
        m_output_queue = output_queue;
    }

    template<typename I, typename O>
    std::shared_ptr<WorkQueue<I>> Worker<I,O>::input_queue() const {
        return m_input_queue;
    }

    template<typename I, typename O>
    std::shared_ptr<WorkQueue<O>> Worker<I,O>::output_queue() const {
        return m_output_queue;
    }
}
