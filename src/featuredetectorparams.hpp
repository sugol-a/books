#pragma once

#include <cstdint>

namespace worker {
    struct FeatureDetectorParams {
            FeatureDetectorParams(uint8_t blur = 11,
                                  uint8_t dilate = 4,
                                  uint8_t threshold = 50)
                : blur_kernel_size(blur),
                  dilate_kernel_size(dilate),
                  threshold(threshold) { }

            uint8_t blur_kernel_size;
            uint8_t dilate_kernel_size;
            uint8_t threshold;
    };
}
