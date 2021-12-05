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
            WorkQueue() : m_closed(false) { }

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

            void push(const T& data) {
                push(std::make_shared<T>(data));
            }

            /**
             * Pushes a vector of resources to the internal queue. Uses a mutex
             * in order to guarantee exclusive access to the queue
             *
             * @param data Vector of resources to push to the queue
             */
            void push(std::vector<std::shared_ptr<T>> data) {
                std::unique_lock lock(m_mutex);

                for (auto& value : data) {
                    m_queue.push(data);
                }

                lock.unlock();
                m_cv.notify_all();
            }

            void push(const std::vector<T>& data) {
                std::unique_lock lock(m_mutex);

                for (const T& value : data) {
                    m_queue.push(std::make_shared<T>(value));
                }

                lock.unlock();
                m_cv.notify_all();
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
                m_cv.wait(lock, [this]{ return m_queue.size() > 0 || m_closed; });

                // If the queue's been closed we still need to flush out the
                // jobs, so only stop popping data when there's nothing left on the queue
                if (m_closed && m_queue.empty()) {
                    return nullptr;
                }

                std::shared_ptr<T> data = m_queue.front();
                m_queue.pop();
                lock.unlock();

                m_cv.notify_one();

                return data;
            }

            /**
             * Closes the queue for reading and writing. This signals to the
             * worker threads that there is no more work, and that they should
             * terminate.
             */
            void close() {
                m_closed = true;
                m_cv.notify_all();
            }

            /**
             * Check whether the queue has been closed
             *
             * @return true if the queue has been closed, false otherwise
             */
            bool closed() {
                return m_closed;
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

        private:
            std::mutex m_mutex;
            std::queue<std::shared_ptr<T>> m_queue;
            std::atomic_bool m_closed;
            std::condition_variable m_cv;
    };
}
