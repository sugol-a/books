#pragma once

#include <chrono>
#include <gtkmm.h>

namespace ui {
    class ProgressWindow : public Gtk::Dialog {
        public:
            ProgressWindow(BaseObjectType* cobject,
                           const Glib::RefPtr<Gtk::Builder>& ref_builder);

            ~ProgressWindow();

            static ProgressWindow* create();

            void set_text(const std::string& text);
            void set_jobs(int n_jobs);
            void set_progress(int current_job);

        private:
            int m_jobs;
            int m_progress_prev;
            std::chrono::time_point<std::chrono::high_resolution_clock> m_time_start;

            Gtk::ProgressBar* m_progressbar;
            Gtk::Label* m_label;
    };
}
