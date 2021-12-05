#include <workqueue.hpp>

namespace worker {
    template<typename T>
    WorkQueue<T>::WorkQueue()
        : m_closed(false)
    { }

    template<typename T>
    void WorkQueue<T>::push(const T& data) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.push(data);

        lock.unlock();
        m_has_data_cv.notify_one();
    }

    template<typename T>
    std::unique_ptr<T> WorkQueue<T>::pop() {
        std::unique_lock<std::mutex> lock(m_mutex);

        // Wait for data to appear on the queue, or for the queue to close
        m_has_data_cv.wait(lock, [this]{ return m_queue.size() > 0 || m_closed; });

        if (m_closed) {
            return nullptr;
        }

        T data = m_queue.front();
        m_queue.pop();
        return std::make_unique<T>(data);
    }

    template<typename T>
    void WorkQueue<T>::close() {
        m_closed = true;
    }
}
