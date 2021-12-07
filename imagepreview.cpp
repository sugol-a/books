#include <imagepreview.hpp>

namespace ui {
    ImagePreview::ImagePreview() {}

    ImagePreview::ImagePreview(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>&)
        : Gtk::DrawingArea(cobject) {
        m_showFeatures = false;
        m_showFitness = false;
        m_showCrop = true;
        m_imageData = nullptr;
        m_margins = 0;

        set_draw_func(sigc::mem_fun(*this, &ImagePreview::on_draw));
    }

    void ImagePreview::set_image(std::shared_ptr<img::ImageData> image) {
        m_imageData = image;
    }

    void ImagePreview::set_margins(size_t margins) {
        m_margins = margins;
    }

    void ImagePreview::show_crop(bool show_crop) {
        m_showCrop = show_crop;
    }

    void ImagePreview::show_features(bool show) {
        m_showFeatures = show;
    }

    void ImagePreview::show_fitness(bool show) {
        m_showFitness = show;
    }

    void ImagePreview::on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height) {
        if (m_imageData == nullptr) {
            // We don't have an image buffer, don't draw anything
            return;
        }

        // Determine an appropriate image scale, using the maximum dimension of
        // the image. This way the whole image is always shown in the preview.
        if (m_imageData->pixbuf()->get_width() > m_imageData->pixbuf()->get_height()) {
            m_scale = double(width) / m_imageData->pixbuf()->get_width();
        } else {
            m_scale = double(height) / m_imageData->pixbuf()->get_height();
        }

        cr->scale(m_scale, m_scale);
        Gdk::Cairo::set_source_pixbuf(cr, m_imageData->pixbuf());

        // Draw the image buffer
        cr->rectangle(0, 0, m_imageData->pixbuf()->get_width(), m_imageData->pixbuf()->get_height());
        cr->fill();

        if (m_showFeatures) {
            draw_features(cr);
        }

        if (m_showCrop) {
            draw_crop(cr);
        }
    }

    void ImagePreview::draw_features(const Cairo::RefPtr<Cairo::Context>& cr) {
        auto font = Cairo::ToyFontFace::create("Bitstream Charter",
                                               Cairo::ToyFontFace::Slant::NORMAL,
                                               Cairo::ToyFontFace::Weight::NORMAL);
        cr->set_font_face(font);

        for (auto& feature : m_imageData->features()) {
            // Render the feature bounding boxes
            cr->set_source_rgb(1.0, 0.0, 1.0);
            cr->set_line_width(4);
            cr->rectangle(feature.second.top_left().x, feature.second.top_left().y,
                          feature.second.width(), feature.second.height());

            cr->stroke();

            if (m_showFitness) {
                // Render the fitness score
                cr->set_source_rgb(1.0, 1.0, 1.0);
                cr->move_to(feature.second.top_left().x,
                            feature.second.top_left().y);
                cr->set_font_size(48);
                cr->show_text(std::to_string(feature.first));
                cr->stroke();
            }
        }

    }

    void ImagePreview::draw_crop(const Cairo::RefPtr<Cairo::Context>& cr) {
        util::Box crop = m_imageData->candidate().second;

        util::Box bounds;
        bounds.top_left() = { 0, 0 };
        bounds.bottom_right() = { m_imageData->pixbuf()->get_width(), m_imageData->pixbuf()->get_height() };

        // Apply margins to the crop rect
        crop.expand(m_margins, bounds);
        bounds.top_left().scale(m_scale);
        bounds.bottom_right().scale(m_scale);

        cr->set_source_rgb(0.0, 1.0, 0.0);
        cr->set_line_width(8);
        cr->rectangle(crop.top_left().x, crop.top_left().y, crop.width(), crop.height());
        cr->stroke();
    }
}
