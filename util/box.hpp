#pragma once

#include <gdkmm.h>
#include <opencv2/imgproc.hpp>

#include <util/point.hpp>

namespace util {
    class Box {
        public:
            Box();
            Box(int x1, int y1, int x2, int y2);
            Box(const cv::Rect& rect);
            Box(const Gdk::Rectangle& rect);

            void expand(int amount, Box bounds);
            Point<int> midpoint() const;
            unsigned long area() const;
            unsigned int width() const;
            unsigned int height() const;
            Point<int>& top_left();
            Point<int>& bottom_right();

            operator cv::Rect() const;
            operator Gdk::Rectangle() const;

        private:
            Point<int> m_topLeft;
            Point<int> m_bottomRight;
    };
}
