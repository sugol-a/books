#pragma once

#include <thread>
#include <workqueue.hpp>

namespace worker {
    class IWorkerPool;

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
             * Runs the worker thread
             */
            virtual void run(IWorkerPool*) = 0;

            /**
             * Joins the worker thread
             */
            void join() {
                m_thread.join();
            }

        protected:
            std::thread m_thread;
    };
}
