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

#pragma once

#include <queue>
#include <mutex>
#include <atomic>
#include <condition_variable>

namespace worker {
    /**
     * A thread-safe queue for passing data between worker threads
     */
    template<typename T>
    class WorkQueue {
        public:
            /**
             * Constructs a new empty work queue
             */
            WorkQueue() : m_finished(false) { }

            /**
             * Pushes a resource to the internal queue. Uses a mutex internally
             * in order to guarantee exclusive access to the queue
             *
             * @param data Resource to push to the queue
             */
            void push(std::shared_ptr<T> data) {
                std::unique_lock lock(m_mutex);
                m_queue.push(data);

                lock.unlock();
                m_cv.notify_one();
            }

            /**
             * Pushes a vector of resources to the internal queue. Uses a mutex
             * in order to guarantee exclusive access to the queue
             *
             * @param data Vector of resources to push to the queue
             */
            void push_vec(std::vector<std::shared_ptr<T>> data) {
                std::unique_lock lock(m_mutex);

                for (auto& value : data) {
                    m_queue.push(data);
                }

                lock.unlock();
                m_cv.notify_one();
            }

            void push_vec(const std::vector<T>& data) {
                std::unique_lock lock(m_mutex);

                for (const T& value : data) {
                    m_queue.push(std::make_shared<T>(value));
                }

                lock.unlock();
                m_cv.notify_one();
            }

            /**
             * Pops a resource from the internal queue. Uses a mutex internally
             * to guarantee exclusive access to the queue. Blocks until there is
             * data on the queue
             *
             * @return Data from the front end of the internal queue before
             *         popping
             */
            std::shared_ptr<T> pop() {
                std::unique_lock lock(m_mutex);

                // Wait for data to appear on the queue, or for the queue to close
                m_cv.wait(lock, [this]{ return m_queue.size() > 0 || m_finished; });

                if (m_queue.size() > 0) {
                    auto data = std::move(m_queue.front());
                    m_queue.pop();

                    return data;
                } else {
                    return nullptr;
                }
            }

            /**
             * Signal to workers that there is no more data to process
             */
            void finish() {
                m_finished = true;
                m_cv.notify_all();
            }

            bool is_finished() {
                return m_finished;
            }

            /**
             * Check whether the queue has any items
             *
             * @return true if the queue contains items, false otherwise
             */
            bool empty() {
                std::lock_guard lock(m_mutex);
                return m_queue.empty();
            }

            /**
             * Get the number of items in the queue
             *
             * @return Number of items in the queue
             */
            size_t size() {
                std::lock_guard lock(m_mutex);
                return m_queue.size();
            }

            void clear() {
                std::lock_guard lock(m_mutex);
                // Clear the queue by swapping it with an empty queue
                std::queue<std::shared_ptr<T>>().swap(m_queue);
            }

        private:
            std::atomic_bool m_finished;
            std::mutex m_mutex;
            std::queue<std::shared_ptr<T>> m_queue;
            std::condition_variable m_cv;
    };
}
