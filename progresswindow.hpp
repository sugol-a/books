#pragma once

#include <gtkmm.h>

namespace ui {
    class ProgressWindow : public Gtk::Window {
        public:
            ProgressWindow(BaseObjectType* cobject,
                           const Glib::RefPtr<Gtk::Builder>& ref_builder);

            ~ProgressWindow();

            static ProgressWindow* create();

            void set_jobs(int n_jobs);
            void set_progress(int current_job);

        private:
            int m_jobs;

            Gtk::ProgressBar* m_progressbar;
    };
}
