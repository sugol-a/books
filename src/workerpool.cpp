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

#include <vector>
#include <numeric>
#include <workerpool.hpp>

namespace worker {
    std::vector<size_t> thread_allocations(std::initializer_list<size_t> weights, size_t max_threads) {
        size_t weight_sum = std::accumulate(weights.begin(), weights.end(), 0);

        if (max_threads == 0) {
            max_threads = std::thread::hardware_concurrency();
        }

        float scale = float(max_threads) / float(weight_sum);
        std::vector<size_t> allocations;

        // First pass: Assign thread allocation based on their weight
        // and the available processor count
        for (auto w : weights) {
            allocations.push_back(w * scale);
        }

        // Second pass: If any allocations are zero, steal a thread from
        // the max allocation
        for (size_t& allocation : allocations) {
            if (allocation == 0) {
                auto max = std::max(allocations.begin(), allocations.end());

                // Edge case: If this system doesn't have many hardware threads
                // available, we may need to allocate a new thread here even
                // though the total number of allocated threads will exceed the
                // total number of hardware threads on the system
                if (*max > 1) {
                    (*max)--;
                }

                allocation++;
            }
        }

        // Third pass: Allocate any remaining threads
        int remaining = max_threads - std::accumulate(allocations.begin(), allocations.end(), 0);
        if (remaining > 0) {
            for (size_t i = 0; i < size_t(remaining); i++) {
                auto min = std::min(allocations.begin(), allocations.end());
                (*min)++;
            }
        }

        return allocations;
    }
}
