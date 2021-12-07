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

            virtual ~WorkerPool() { };
            virtual std::shared_ptr<InputQueue> input() = 0;
            virtual std::shared_ptr<OutputQueue> output() = 0;
            virtual void set_input(std::shared_ptr<InputQueue> in) = 0;
            virtual void set_output(std::shared_ptr<OutputQueue> out) = 0;
            virtual void run_workers() = 0;
            virtual void join_all() = 0;
    };
}
