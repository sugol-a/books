#include <cmath>
#include <iostream>
#include <croprect.hpp>

static double dist(double x1, double y1, double x2, double y2) {
    return std::sqrt(std::pow(x1 - x2, 2) + std::pow(y1 - y2, 2));
}

static unsigned int modulo(int n, int m) {
    int mod = n % m;

    if (mod < 0) {
        mod += m;
    }

    return mod;
}

static constexpr size_t TOP_LEFT = 0;
static constexpr size_t BOTTOM_RIGHT = 2;

namespace ui {
    CropRect::CropRect() {
        // Set up relationships between handles
        size_t n_handles = sizeof(m_handles) / sizeof(m_handles[0]);
        for (size_t i = 0; i < n_handles; i++) {
            // Going clockwise - starting from the top left - each handle with
            // an even index drives the x-position of the handle before it, and
            // the y-position of the handle after it. Each handle with an odd
            // index drives the y-position of the handle before it, and the
            // x-position of the handle after it
            if (i % 2 == 0) {
                m_handles[i].set_drive_x(&m_handles[modulo(i - 1, n_handles)]);
                m_handles[i].set_drive_y(&m_handles[modulo(i + 1, n_handles)]);
            } else {
                m_handles[i].set_drive_x(&m_handles[modulo(i + 1, n_handles)]);
                m_handles[i].set_drive_y(&m_handles[modulo(i - 1, n_handles)]);
            }
        }
    }

    CropHandle* CropRect::handle_at(double x, double y) {
        for (auto& handle : m_handles) {
            // Check whether (x, y) sits inside the handle radius, with some margin of error
            if (dist(handle.x(), handle.y(), x, y) <= handle.radius() * 10) {
                return &handle;
            }
        }

        return nullptr;
    }

    Gdk::Rectangle CropRect::get_rect() const {
        int x_min, y_min;
        int x_max, y_max;
        int width, height;

        // It's possible for the top left and bottom right handles to be inverted
        x_min = std::min(m_handles[TOP_LEFT].x(), m_handles[BOTTOM_RIGHT].x());
        y_min = std::min(m_handles[TOP_LEFT].y(), m_handles[BOTTOM_RIGHT].y());
        x_max = std::max(m_handles[TOP_LEFT].x(), m_handles[BOTTOM_RIGHT].x());
        y_max = std::max(m_handles[TOP_LEFT].y(), m_handles[BOTTOM_RIGHT].y());

        width = x_max - x_min;
        height = y_max - y_min;

        return { x_min, y_min, width, height };
    }

    void CropRect::set_rect(const Gdk::Rectangle& rect) {
        m_handles[TOP_LEFT].set_position(rect.get_x(), rect.get_y());
        m_handles[BOTTOM_RIGHT].set_position(rect.get_x() + rect.get_width(),
                                             rect.get_y() + rect.get_height());
    }

    std::vector<CropHandle*> CropRect::handles() {
        std::vector<CropHandle*> handles;

        for (auto& handle : m_handles) {
            handles.push_back(&handle);
        }

        return handles;
    }
}
