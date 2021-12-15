#pragma once

#include <vector>

#include <gtkmm.h>

#include <util/box.hpp>
#include <croprect.hpp>
#include <imagedata.hpp>

namespace ui {
    class ImagePreview : public Gtk::Picture {
        public:
            ImagePreview();
            ImagePreview(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ref_builder);

            void set_crop(std::shared_ptr<Gdk::Rectangle> rect);
            void set_features(const std::vector<img::ImageData::Feature>& features);
            void show_crop(bool show);
            void show_features(bool show);
            void show_fitness(bool show);

        protected:
            void snapshot_vfunc(const Glib::RefPtr<Gtk::Snapshot>& snapshot) override;

        private:
            void get_visual_offsets(int& x, int& y);
            double get_visual_scale();

            void draw_features(const Glib::RefPtr<Cairo::Context>& cr, double scale);
            void draw_crop(const Glib::RefPtr<Cairo::Context>& cr, double scale);

            void on_drag_begin(double x, double y);
            void on_drag_update(double dx, double dy);
            void on_drag_end(double, double);

        private:
            bool m_showCrop;
            bool m_showFeatures;
            bool m_showFitness;
            CropRect m_cropRect;
            std::shared_ptr<Gdk::Rectangle> m_rect;
            std::vector<img::ImageData::Feature> m_features;
            CropHandle* m_currentHandle;
            Glib::RefPtr<Gtk::GestureDrag> m_gestureDrag;
    };
}
