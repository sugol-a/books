#pragma once

#include <gtkmm.h>
#include <crop.hpp>
#include <imageloader.hpp>

namespace ui {
    class CropPreview : public Gtk::DrawingArea {
        public:
        CropPreview();
        CropPreview(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ref_builder);
        void set_image(Image* img);
        void set_enable(bool do_crop);
        void set_crop(cv::Rect crop);

        protected:
        void on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);

        private:
        void load_image_buffer();

        private:
        bool m_doCrop;
        Glib::RefPtr<Gdk::Pixbuf> m_imageBuffer;
        Image* m_currentImage;
        cv::Rect m_crop;
    };
}
