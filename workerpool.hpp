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
            virtual void stop() = 0;
            virtual bool stopped() = 0;
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
