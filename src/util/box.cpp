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

#include <util/box.hpp>

namespace util {
    Box::Box()
        : m_topLeft({0, 0}),
          m_bottomRight({0, 0}) { }

    Box::Box(int x1, int y1, int x2, int y2)
        : m_topLeft({x1, y1}),
          m_bottomRight({x2, y2}) { }

    Box::Box(const cv::Rect& rect) {
        m_topLeft.x = rect.x;
        m_topLeft.y = rect.y;
        m_bottomRight.x = rect.x + rect.width;
        m_bottomRight.y = rect.y + rect.height;
    }

    Box::Box(const Gdk::Rectangle& rect) {
        m_topLeft.x = rect.get_x();
        m_topLeft.y = rect.get_y();
        m_bottomRight.x = rect.get_x() + rect.get_width();
        m_bottomRight.y = rect.get_y() + rect.get_height();
    }

    void Box::expand(int amount, Box bounds) {
        m_topLeft.x -= amount;
        m_topLeft.y -= amount;

        m_bottomRight.x += amount;
        m_bottomRight.y += amount;

        // Clip the resulting box inside the bounds
        if (m_topLeft.x < bounds.m_topLeft.x)
            m_topLeft.x = bounds.m_topLeft.x;

        if (m_topLeft.y < bounds.m_topLeft.y)
            m_topLeft.y = bounds.m_topLeft.y;

        if (m_bottomRight.x >= bounds.m_bottomRight.x)
            m_bottomRight.x = bounds.m_bottomRight.x - 1;

        if (m_bottomRight.y >= bounds.m_bottomRight.y)
            m_bottomRight.y = bounds.m_bottomRight.y - 1;
    }

    Point<int> Box::midpoint() const {
        Point<int> mid;

        mid.x = m_topLeft.x + width() / 2;
        mid.y = m_topLeft.y + height() / 2;

        return mid;
    }

    unsigned long Box::area() const {
        return width() * height();
    }

    unsigned int Box::width() const {
        return m_bottomRight.x - m_topLeft.x;
    }

    unsigned int Box::height() const {
        return m_bottomRight.y - m_topLeft.y;
    }

    Point<int>& Box::top_left() {
        return m_topLeft;
    }

    Point<int>& Box::bottom_right() {
        return m_bottomRight;
    }

    Box::operator cv::Rect() const {
        cv::Rect rect;
        rect.x = m_topLeft.x;
        rect.y = m_topLeft.y;
        rect.width = width();
        rect.height = height();

        return rect;
    }

    Box::operator Gdk::Rectangle() const {
        Gdk::Rectangle rect;
        rect.set_x(m_topLeft.x);
        rect.set_y(m_topLeft.y);
        rect.set_width(width());
        rect.set_height(height());

        return rect;
    }
}
