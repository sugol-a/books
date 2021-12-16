#pragma once

#include <gtkmm.h>
#include <crophandle.hpp>

namespace ui {
    class CropRect {
        public:
            CropRect();

            CropHandle* handle_at(double x, double y);

            Gdk::Rectangle get_rect() const;
            void set_rect(const Gdk::Rectangle& rect);
            std::vector<CropHandle*> handles();

        private:
            CropHandle m_handles[4];
    };
}
