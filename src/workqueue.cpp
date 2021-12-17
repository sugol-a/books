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
