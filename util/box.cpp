#include <util/box.hpp>

namespace util {
    Box::Box()
        : m_topLeft({0, 0}),
          m_bottomRight({0, 0}) { }

    Box::Box(cv::Rect const& rect) {
        m_topLeft.x = rect.x;
        m_topLeft.y = rect.y;
        m_bottomRight.x = rect.x + rect.width;
        m_bottomRight.y = rect.y + rect.height;
    }

    Point Box::midpoint() const {
        Point mid;

        mid.x = (m_bottomRight.x - m_topLeft.x) / 2;
        mid.y = (m_bottomRight.y - m_topLeft.y) / 2;

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

    Point Box::top_left() const {
        return m_topLeft;
    }

    Point Box::bottom_right() const {
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
}
