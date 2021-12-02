#pragma once

#include <opencv2/imgproc.hpp>

#include <util/point.hpp>

namespace util {
    class Box {
    public:
        Box();
        Box(cv::Rect const& rect);

        Point midpoint() const;
        unsigned long area() const;
        unsigned int width() const;
        unsigned int height() const;
        Point top_left() const;
        Point bottom_right() const;
        operator cv::Rect() const;

    private:
        Point m_topLeft;
        Point m_bottomRight;
    };
}
