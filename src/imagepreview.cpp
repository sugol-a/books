#include <imagepreview.hpp>

namespace ui {
    ImagePreview::ImagePreview() {}

    ImagePreview::ImagePreview(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>&)
        : Gtk::Picture(cobject) {
        m_showFeatures = false;
        m_showFitness = false;
        m_showCrop = true;

        m_currentHandle = nullptr;

        m_gestureDrag = Gtk::GestureDrag::create();
        m_gestureDrag->signal_drag_begin().connect(sigc::mem_fun(*this, &ImagePreview::on_drag_begin));
        m_gestureDrag->signal_drag_update().connect(sigc::mem_fun(*this, &ImagePreview::on_drag_update));
        m_gestureDrag->signal_drag_end().connect(sigc::mem_fun(*this, &ImagePreview::on_drag_end));

        add_controller(m_gestureDrag);
    }

    void ImagePreview::set_crop(std::shared_ptr<Gdk::Rectangle> rect) {
        m_rect = rect;
        m_cropRect.set_rect(*rect);
    }

    void ImagePreview::set_features(const std::vector<img::ImageData::Feature>& features) {
        m_features = features;
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

    void ImagePreview::snapshot_vfunc(const Glib::RefPtr<Gtk::Snapshot>& snapshot) {
        // Render the underlying image first
        Gtk::Picture::snapshot_vfunc(snapshot);

        if (get_file() == nullptr) {
            return;
        }

        // Get the offset of the image content
        int x_offset, y_offset;
        double visual_scale = get_visual_scale();
        get_visual_offsets(x_offset, y_offset);

        // Gtkmm doesn't seem to expose gtk_snapshot_translate, so we need to
        // drop to the C api here
        GtkSnapshot* c_snapshot = snapshot->gobj();
        graphene_point_t pt = {
            (float)x_offset, (float)y_offset
        };

        gtk_snapshot_save(c_snapshot);
        gtk_snapshot_translate(c_snapshot, &pt);

        const auto allocation = get_allocation();
        const Gdk::Rectangle rect(0, 0, allocation.get_width(), allocation.get_height());
        Glib::RefPtr<Cairo::Context> context = snapshot->append_cairo(rect);

        // Overwrite RGBA
        context->set_operator(Cairo::Context::Operator::SOURCE);

        if (m_showCrop) {
            draw_crop(context, visual_scale);
        }

        context->set_operator(Cairo::Context::Operator::OVER);
        if (m_showFeatures) {
            draw_features(context, visual_scale);
        }

        gtk_snapshot_restore(c_snapshot);
    }

    void ImagePreview::get_visual_offsets(int& x, int& y) {
        // Adapted from gtk_picture_snapshot() (in gtkpicture.c)
        int width = get_width();
        int height = get_height();
        double ratio = get_paintable()->get_intrinsic_aspect_ratio();
        double widget_ratio = (double)width / height;
        double w;
        double h;

        if (ratio > widget_ratio) {
            w = width;
            h = width / ratio;
        } else {
            w = height * ratio;
            h = height;
        }

        x = (width - ceil(w)) / 2;
        y = floor(height - ceil(h)) / 2;
    }

    double ImagePreview::get_visual_scale() {
        double scale;
        int width = get_width();
        int height = get_height();
        auto paintable = get_paintable();
        double ratio = paintable->get_intrinsic_aspect_ratio();
        double widget_ratio = (double)width / height;

        if (ratio > widget_ratio) {
            scale = (double)width / paintable->get_intrinsic_width();
        } else{
            scale = (double)height / paintable->get_intrinsic_height();
        }

        return scale;
    }

    void ImagePreview::draw_features(const Glib::RefPtr<Cairo::Context>& cr, double scale) {
        auto font = Cairo::ToyFontFace::create("Cantarell",
                                               Cairo::ToyFontFace::Slant::NORMAL,
                                               Cairo::ToyFontFace::Weight::NORMAL);
        cr->set_font_face(font);

        for (auto& feature : m_features) {
            Gdk::Rectangle r = feature.second;

            double x, y, w, h;
            x = r.get_x() * scale;
            y = r.get_y() * scale;
            w = r.get_width() * scale;
            h = r.get_height() * scale;

            // Render the feature bounding boxes
            cr->set_source_rgb(0.382, 0.625, 0.914);
            cr->set_line_width(1);
            cr->rectangle(x, y, w, h);
            cr->stroke();

            if (m_showFitness) {
                // Render the fitness score
                cr->set_source_rgb(1.0, 1.0, 1.0);
                cr->move_to(x, y);
                cr->set_font_size(12);
                cr->show_text(std::to_string(feature.first));
                cr->stroke();
            }
        }
    }

    void ImagePreview::draw_crop(const Glib::RefPtr<Cairo::Context>& cr, double scale) {
        Gdk::Rectangle crop = m_cropRect.get_rect();

        // Transform the crop rectangle so that it's relative to this widget's
        // contents
        double x1, y1, x2, y2;
        x1 = crop.get_x() * scale;
        y1 = crop.get_y() * scale;
        x2 = (crop.get_x() + crop.get_width()) * scale;
        y2 = (crop.get_y() + crop.get_height()) * scale;

        double width, height;
        width = x2 - x1;
        height = y2 - y1;

        // Draw a translucent black overlay
        cr->set_source_rgba(0, 0, 0, 0.5);
        cr->rectangle(0, 0, scale * get_paintable()->get_intrinsic_width(), scale * get_paintable()->get_intrinsic_height());
        cr->fill();

        // Cut out the crop rectangle
        cr->set_source_rgba(0, 0, 0, 0);
        cr->rectangle(x1, y1, width, height);
        cr->fill();

        // Draw the outer frame of the crop rect
        cr->set_source_rgba(1, 1, 1, 0.75);
        cr->rectangle(x1, y1, width, height);
        cr->set_line_width(2);
        cr->stroke();

        // Draw the 'thirds' inside the crop rect
        const double third1 = 1.0 / 3.0;
        const double third2 = 2.0 / 3.0;

        // Along x
        cr->move_to(x1 + (width * third1), y1);
        cr->line_to(x1 + (width * third1), y2);
        cr->move_to(x1 + (width * third2), y1);
        cr->line_to(x1 + (width * third2), y2);

        // Along y
        cr->move_to(x1, y1 + (height * third1));
        cr->line_to(x2, y1 + (height * third1));
        cr->move_to(x1, y1 + (height * third2));
        cr->line_to(x2, y1 + (height * third2));

        cr->set_line_width(1);
        cr->stroke();

        // Draw the handles
        for (const auto& handle : m_cropRect.handles()) {
            int x = handle->x() * scale;
            int y = handle->y() * scale;

            if (handle == m_currentHandle) {
                cr->set_source_rgba(1.0, 1.0, 1.0, 1.0);
            } else {
                cr->set_source_rgba(1.0, 1.0, 1.0, 0.75);
            }
            cr->arc(x, y, handle->radius(), 0, 2 * G_PI);
            cr->fill();
        }
    }

    void ImagePreview::on_drag_begin(double x, double y) {
        if (!m_showCrop || !get_file()) {
            return;
        }

        double scale = get_visual_scale();
        int x_offset, y_offset;
        get_visual_offsets(x_offset, y_offset);

        int xp = (x - x_offset) / scale;
        int yp = (y - y_offset) / scale;

        m_currentHandle = m_cropRect.handle_at(xp, yp);
        queue_draw();
    }

    void ImagePreview::on_drag_update(double dx, double dy) {
        if (!m_currentHandle || !get_file()) {
            return;
        }

        double scale = get_visual_scale();
        int x_offset, y_offset;
        get_visual_offsets(x_offset, y_offset);

        double start_x, start_y;
        m_gestureDrag->get_start_point(start_x, start_y);

        double xpos, ypos;
        xpos = (start_x + dx - x_offset) / scale;
        ypos = (start_y + dy - y_offset) / scale;

        int image_width = get_paintable()->get_intrinsic_width();
        int image_height = get_paintable()->get_intrinsic_height();

        // Make sure the handle doesn't leave the bounds of the image
        if (xpos < 0) {
            xpos = 0;
        } else if (xpos >= image_width) {
            xpos = image_width - 1;
        }

        if (ypos < 0) {
            ypos = 0;
        } else if (ypos >= image_height) {
            ypos = image_height - 1;
        }

        m_currentHandle->set_position(xpos, ypos);

        queue_draw();
    }

    void ImagePreview::on_drag_end(double, double) {
        if (get_file() != nullptr) {
            m_currentHandle = nullptr;
            *m_rect = m_cropRect.get_rect();
            queue_draw();
        }
    }
}
