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
