#include <iostream>
#include <imagepreview.hpp>

namespace ui {
    ImagePreview::ImagePreview() {}

    ImagePreview::ImagePreview(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ref_builder)
        : Gtk::DrawingArea(cobject) {
        m_showFeatures = false;
        m_showFitness = false;
        m_showCrop = true;

        set_draw_func(sigc::mem_fun(*this, &ImagePreview::on_draw));
    }

    void ImagePreview::set_image(const cv::Mat& image) {
        cv::Mat rgb;

        if (m_imageBuffer) {
            m_imageBuffer = nullptr;
        }

        if (image.channels() == 1) {
            cv::cvtColor(image, rgb, cv::COLOR_GRAY2RGB);
        } else {
            cv::cvtColor(image, rgb, cv::COLOR_BGR2RGB);
        }

        m_imageBuffer = Gdk::Pixbuf::create_from_data(rgb.data,
                                                      Gdk::Colorspace::RGB,
                                                      false,
                                                      8,
                                                      rgb.size().width,
                                                      rgb.size().height,
                                                      rgb.step);
    }

    void ImagePreview::set_image(img::Image& image) {
        m_imageBuffer = image.pixbuf();
    }

    void ImagePreview::set_crop(const util::Box& crop) {
        m_crop = crop;
    }

    void ImagePreview::feature_scale(float scale) {
        m_featureScale = scale;
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

    void ImagePreview::set_features(const std::vector<std::pair<double, util::Box>>& boxes) {
        m_features = boxes;
    }

    void ImagePreview::on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height) {
        if (m_imageBuffer == nullptr) {
            // We don't have an image buffer, don't draw anything
            return;
        }

        // Determine an appropriate image scale, using the maximum dimension of
        // the image. This way the whole image is always shown in the preview.
        double scale;
        if (m_imageBuffer->get_width() > m_imageBuffer->get_height()) {
            scale = double(width) / m_imageBuffer->get_width();
        } else {
            scale = double(height) / m_imageBuffer->get_height();
        }

        cr->scale(scale, scale);
        Gdk::Cairo::set_source_pixbuf(cr, m_imageBuffer);

        // Draw the image buffer
        cr->rectangle(0, 0, m_imageBuffer->get_width(), m_imageBuffer->get_height());
        cr->fill();

        cr->scale(m_featureScale, m_featureScale);
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

        for (auto& feature : m_features) {
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
        cr->set_source_rgb(0.0, 1.0, 0.0);
        cr->set_line_width(8);
        cr->rectangle(m_crop.top_left().x, m_crop.top_left().y, m_crop.width(), m_crop.height());
        cr->stroke();
    }
}
