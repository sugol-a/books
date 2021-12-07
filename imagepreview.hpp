#pragma once

#include <vector>

#include <gtkmm.h>

#include <util/box.hpp>
#include <imagedata.hpp>

namespace ui {
    class ImagePreview : public Gtk::DrawingArea {
        public:
            ImagePreview();
            ImagePreview(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ref_builder);

            void set_image(std::shared_ptr<img::ImageData> image);
            void set_margins(size_t margins);

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
            size_t m_margins;
            double m_scale;
            std::shared_ptr<img::ImageData> m_imageData;
    };
}
