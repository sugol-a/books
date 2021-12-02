#pragma once

#include <gtkmm.h>
#include <crop.hpp>

namespace ui {
    class ImagePreview : public Gtk::DrawingArea {
        public:
            ImagePreview();
            ImagePreview(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ref_builder);

            void set_image(cv::Mat const& image);
            void set_show_box(bool show_box);
            void set_box(util::Box box);

        protected:
            void on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);

        private:
            bool m_showBox;
            cv::Mat m_image;
            Glib::RefPtr<Gdk::Pixbuf> m_imageBuffer;
            util::Box m_cropBox;
    };
}
