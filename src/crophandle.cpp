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
