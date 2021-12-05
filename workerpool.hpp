#pragma once

#include <workqueue.hpp>
#include <worker.hpp>
#include <memory>

namespace worker {
    /**
    * Abstract worker pool class
    */
    template<typename W>
    class WorkerPool {
        public:
            using InputQueue = typename W::InputQueue;
            using OutputQueue = typename W::OutputQueue;

            virtual ~WorkerPool() { };
            virtual std::shared_ptr<InputQueue> input() = 0;
            virtual std::shared_ptr<OutputQueue> output() = 0;
            virtual void run_workers() = 0;
            virtual void join_all() = 0;
    };
}
