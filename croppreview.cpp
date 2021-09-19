#include "cairomm/pattern.h"
#include <croppreview.hpp>
#include <iostream>
namespace ui {
    CropPreview::CropPreview() {
        
    }

    CropPreview::CropPreview(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ref_builder) 
    : Gtk::DrawingArea(cobject) {
        m_currentImage = nullptr;
        m_doCrop = false;
        set_draw_func(sigc::mem_fun(*this, &CropPreview::on_draw));
    }

    void CropPreview::set_image(Image* img) {
        m_currentImage = img;
        m_imageBuffer = nullptr;
        queue_draw();
    }

    void CropPreview::set_enable(bool do_crop) {
        m_doCrop = do_crop;
        queue_draw();
    }

    void CropPreview::set_crop(cv::Rect crop) {
        m_crop = crop;
        queue_draw();
    }

    void CropPreview::on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height) {
        if (m_currentImage == nullptr) {
            return;
        }

        if (m_imageBuffer == nullptr) {
            load_image_buffer();
        }

        Gdk::Cairo::set_source_pixbuf(cr, m_imageBuffer);
        double scale;
        if (m_imageBuffer->get_width() > m_imageBuffer->get_height()) {
            scale = double(width) / m_imageBuffer->get_width();
        } else {
            scale = double(height) / m_imageBuffer->get_height();
        }
    
        cr->scale(scale, scale);

        Gdk::Cairo::set_source_pixbuf(cr, m_imageBuffer);
        cr->rectangle(0, 0, m_imageBuffer->get_width(), m_imageBuffer->get_height());
        cr->fill();

        // Show the cropping rectangle
        if (m_doCrop) {
            cr->set_source_rgb(0.0, 1.0, 0);
            cr->set_line_width(8);
            cr->rectangle(m_crop.x, m_crop.y, m_crop.width, m_crop.height);
            cr->stroke();
        }
    }

    void CropPreview::load_image_buffer() {
        // Load this image in seperately from the opencv data, since cairo doesn't seem to support BGR
        m_imageBuffer = Gdk::Pixbuf::create_from_file(m_currentImage->filename);
    }
}