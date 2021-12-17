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

#include <workqueue.hpp>
#include <worker.hpp>

namespace worker {
    class IWorkerPool {
        public:
            virtual ~IWorkerPool() { };
            virtual void run_workers() = 0;
            virtual void join_all() = 0;
            virtual void signal_done() = 0;
    };

    /**
     * Abstract worker pool class
     */
    template<typename W>
    class WorkerPool : public IWorkerPool {
        public:
            using InputQueue = typename W::InputQueue;
            using OutputQueue = typename W::OutputQueue;

            WorkerPool<W>() {
                m_input_queue = std::make_shared<InputQueue>();
                m_output_queue = std::make_shared<OutputQueue>();

                m_stopped = false;
            }

            virtual ~WorkerPool() {
                join_all();
            };

            /**
             * Gets the input queue associated with this worker pool
             *
             * @return Input queue
             */
            std::shared_ptr<InputQueue> input() {
                return m_input_queue;
            }

            /**
             * Gets the output queue associated with this worker pool
             *
             * @return Output queue
             */
            std::shared_ptr<OutputQueue> output() {
                return m_output_queue;
            }

            /**
             * Sets the worker pool's input queue
             *
             * @param in Input queue
             */
            void set_input(std::shared_ptr<InputQueue> in) {
                m_input_queue = in;
            }

            /**
             * Sets the worker pool's output queue
             *
             * @param out Output queue
             */
            void set_output(std::shared_ptr<OutputQueue> out) {
                m_output_queue = out;
            }

            /**
             * Stops the worker pool ASAP. Clears and finishes the input and
             * output queues
             */
            void stop() {
                m_input_queue->finish();
                m_input_queue->clear();
                m_output_queue->finish();
                m_output_queue->clear();

                m_stopped = true;
            }

            /**
             * Checks whether this worker pool has been stopped
             *
             * @return true if this pool has been stopped, false otherwise
             */
            bool stopped() {
                return m_stopped;
            }

            /**
             * Signal that a worker has finished. Decrements the active worker
             * count. Once the active worker count reaches zero, the output
             * queue is finished
             */
            void signal_done() override {
                std::lock_guard lock(m_mutex);
                m_active_workers--;

                if (m_active_workers == 0) {
                    m_output_queue->finish();
                }
            }

            /**
             * Joins all of the worker threads
             */
            void join_all() override {
                for (auto& worker : m_workers) {
                    worker->join();
                }
            }

        protected:
            std::shared_ptr<InputQueue> m_input_queue;
            std::shared_ptr<OutputQueue> m_output_queue;
            mutable std::mutex m_mutex;
            size_t m_active_workers;
            std::vector<std::unique_ptr<W>> m_workers;
            bool m_stopped;
    };

    /**
     * Calculates weighted thread allocations. Generates a list of thread
     * allocations, with each allocation approximately proportional to the
     * corresponding weight in `weights`. `max_threads` sets the maximum number
     * of threads to allocate - set it to 0 to automatically determine the
     * number of processors on the system.
     *
     * The minimum thread allocation is 1; so on systems with few processors,
     * this will allocate more threads than there are physical processors.
     *
     * Example:
     * thread_allocations({2, 1}, 12)
     *  -> { 8, 4 }
     */
    std::vector<size_t> thread_allocations(std::initializer_list<size_t> weights,
                                           size_t max_threads = 0);
}
