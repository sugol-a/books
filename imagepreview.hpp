#pragma once

#include <vector>

#include <gtkmm.h>
#include <image.hpp>
#include <crop.hpp>

namespace ui {
    class ImagePreview : public Gtk::DrawingArea {
        public:
            ImagePreview();
            ImagePreview(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ref_builder);

            void set_image(const cv::Mat& image);
            void set_show_crop(bool show_crop);
            void set_crop(const util::Box& crop);
            void set_show_features(bool show_features);
            void set_features(const std::vector<std::pair<double,util::Box>>& boxes);
            void feature_scale(float scale);

        protected:
            void on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);

        private:
            void draw_features(const Cairo::RefPtr<Cairo::Context>& cr);
            void draw_crop(const Cairo::RefPtr<Cairo::Context>& cr);

        private:
            bool m_showCrop;
            bool m_showFeatures;
            float m_featureScale;
            Glib::RefPtr<Gdk::Pixbuf> m_imageBuffer;
            util::Box m_crop;
            std::vector<std::pair<double, util::Box>> m_features;
    };
}
