#pragma once

#include <thread>
#include <workqueue.hpp>

namespace worker {
    /**
     * Worker thread base class
     */
    template<typename I, typename O>
    class Worker {
        public:
            using InputQueue = WorkQueue<I>;
            using OutputQueue = WorkQueue<O>;

            virtual ~Worker() {}

            /**
             * Sets the input queue for the worker thread. The thread reads
             * input values (jobs) from `input_queue`
             *
             * @param input_queue Queue to get jobs from
             */
            void set_input_queue(std::shared_ptr<InputQueue> input_queue) {
                m_input_queue = input_queue;
            }

            /**
             * Sets the output queue for the worker thread. The threads writes
             * the result of finished jobs to `output_queue`
             *
             * @param output_queue Queue to write job results to
             */
            void set_output_queue(std::shared_ptr<OutputQueue> output_queue) {
                m_output_queue = output_queue;
            }

            /**
             * Gets the input queue associated with this worker
             *
             * @return Input queue of this worker
             */
            std::shared_ptr<InputQueue> input_queue() const {
                return m_input_queue;
            }

            /**
             * Gets the output queue associated with this worker
             *
             * @return Output queue of this worker
             */
            std::shared_ptr<OutputQueue> output_queue() const {
                return m_output_queue;
            }

            /**
             * Runs the worker thread
             */
            virtual void run() = 0;

            /**
             * Joins the worker thread
             */
            void join() {
                m_thread.join();
            }

        protected:
            std::thread m_thread;
            std::shared_ptr<InputQueue> m_input_queue;
            std::shared_ptr<OutputQueue> m_output_queue;
    };
}
