#pragma once

#define MIN_RELATIVE_BOX_SIZE 0.03

#include <imageloader.hpp>
#include <util/box.hpp>
#include <filter/filterchain.hpp>

class Cropper {
    public:
        Cropper();

        cv::Rect auto_crop(CVImage& image,
                           int margin = 32,
                           bool remove_whiteboard = true,
                           int kernel_size = 11);

        filter::FilterChain& get_filterchain();

    private:
        cv::Rect enclose_bounding_boxes(std::vector<util::Box> boxes);

    private:
        filter::FilterChain m_filterchain;
};
