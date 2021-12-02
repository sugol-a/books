#include <iostream>
#include <imagepreview.hpp>

namespace ui {
    ImagePreview::ImagePreview() {}

    ImagePreview::ImagePreview(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ref_builder)
        : Gtk::DrawingArea(cobject) {
        m_showBox = false;
        m_imageBuffer = nullptr;

        set_draw_func(sigc::mem_fun(*this, &ImagePreview::on_draw));
    }

    void ImagePreview::set_image(cv::Mat const& image) {
        // Convert from BGR to RGB
        cv::cvtColor(image, m_image, cv::COLOR_BGR2RGB);

        // Convert the cv::Mat to a Gdk pixbuf
        m_imageBuffer = Gdk::Pixbuf::create_from_data(m_image.data,
                                                      Gdk::Colorspace::RGB,
                                                      false,
                                                      8,
                                                      m_image.size().width,
                                                      m_image.size().height,
                                                      m_image.step);
    }

    void ImagePreview::set_show_box(bool show_box) {
        m_showBox = show_box;
    }

    void ImagePreview::set_box(util::Box box) {
        m_cropBox = box;
    }

    void ImagePreview::on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height) {
        if (m_imageBuffer == nullptr) {
            // We don't have an image buffer, don't draw anything
            return;
        }

        // Determine an appropriate image scale, using the maximum dimension of
        // the image. This way the whole image is always shown in the preview.
        double scale;
        Gdk::Cairo::set_source_pixbuf(cr, m_imageBuffer);
        if (m_imageBuffer->get_width() > m_imageBuffer->get_height()) {
            scale = double(width) / m_imageBuffer->get_width();
        } else {
            scale = double(height) / m_imageBuffer->get_height();
        }

        cr->scale(scale, scale);

        // Draw the image buffer
        Gdk::Cairo::set_source_pixbuf(cr, m_imageBuffer);
        cr->rectangle(0, 0, m_imageBuffer->get_width(), m_imageBuffer->get_height());
        cr->fill();

        if (m_showBox) {
            // TODO: Make this look more presentable / make it configurable
            cr->set_source_rgb(0.0, 1.0, 0.0);
            cr->set_line_width(8);
            cr->rectangle(m_cropBox.top_left().x, m_cropBox.top_left().y, m_cropBox.width(), m_cropBox.height());
            cr->stroke();
        }
    }
}
