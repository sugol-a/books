#pragma once

#include <opencv2/imgproc.hpp>

#include <util/point.hpp>

namespace util {
    class Box {
    public:
        Box();
        Box(cv::Rect const& rect);

        Point<int> midpoint() const;
        unsigned long area() const;
        unsigned int width() const;
        unsigned int height() const;
        Point<int>& top_left();
        Point<int>& bottom_right();
        operator cv::Rect() const;

    private:
        Point<int> m_topLeft;
        Point<int> m_bottomRight;
    };
}
