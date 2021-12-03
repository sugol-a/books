#pragma once

#include <vector>

#include <gtkmm.h>

#include <util/box.hpp>
#include <image.hpp>

namespace ui {
    class ImagePreview : public Gtk::DrawingArea {
        public:
            ImagePreview();
            ImagePreview(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ref_builder);

            void set_image(const cv::Mat& image);
            void set_image(img::Image& image);
            void set_crop(const util::Box& crop);
            void set_features(const std::vector<std::pair<double,util::Box>>& boxes);
            void feature_scale(float scale);

            void show_crop(bool show);
            void show_features(bool show);
            void show_fitness(bool show);
        protected:
            void on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);

        private:
            void draw_features(const Cairo::RefPtr<Cairo::Context>& cr);
            void draw_crop(const Cairo::RefPtr<Cairo::Context>& cr);

        private:
            bool m_showCrop;
            bool m_showFeatures;
            bool m_showFitness;
            float m_featureScale;
            Glib::RefPtr<Gdk::Pixbuf> m_imageBuffer;
            util::Box m_crop;
            std::vector<std::pair<double, util::Box>> m_features;
    };
}
