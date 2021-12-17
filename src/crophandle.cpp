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

#include <crophandle.hpp>

namespace ui {
    CropHandle::CropHandle(double x, double y, int radius)
        : m_x(x), m_y(y), m_radius(radius) {
        m_drive_x = nullptr;
        m_drive_y = nullptr;
    }

    CropHandle::CropHandle(const CropHandle& other) {
        m_x = other.m_x;
        m_y = other.m_y;
        m_radius = other.m_radius;
    }

    void CropHandle::set_drive_x(CropHandle* other) {
        m_drive_x = other;
    }

    void CropHandle::set_drive_y(CropHandle* other) {
        m_drive_y = other;
    }

    void CropHandle::set_position(double x, double y) {
        m_x = x;
        m_y = y;

        // Update driven handles
        if (m_drive_x) {
            m_drive_x->m_x = x;
        }

        if (m_drive_y) {
            m_drive_y->m_y = y;
        }
    }

    double CropHandle::x() const {
        return m_x;
    }

    double CropHandle::y() const {
        return m_y;
    }

    int CropHandle::radius() const {
        return m_radius;
    }
}
