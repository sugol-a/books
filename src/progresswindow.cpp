/*
 * Copyright (C) 2021  Alister Sanders
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <iostream>
#include <progresswindow.hpp>

namespace ui {
    ProgressWindow::ProgressWindow(BaseObjectType* cobject,
                                   const Glib::RefPtr<Gtk::Builder>& ref_builder)
        : Gtk::Dialog(cobject) {
        m_jobs = 0;
        m_progress_prev = 0;
        m_time_start = std::chrono::high_resolution_clock::now();
        m_progressbar = ref_builder->get_widget<Gtk::ProgressBar>("progressbar");
        m_label = ref_builder->get_widget<Gtk::Label>("lblAction");

        this->set_modal(true);
    }

    ProgressWindow::~ProgressWindow() {}

    ProgressWindow* ProgressWindow::create() {
        auto ui = Gtk::Builder::create_from_resource("/org/sugol/books/ui/ProgressWindow.ui");
        ProgressWindow* progress_window = Gtk::Builder::get_widget_derived<ProgressWindow>(ui, "progressWindow");

        return progress_window;
    }

    void ProgressWindow::set_text(const std::string& text) {
        m_label->set_text(text);
    }

    void ProgressWindow::set_jobs(int n_jobs) {
        m_jobs = n_jobs;
    }

    void ProgressWindow::set_progress(int current_job) {
        auto time_now = std::chrono::high_resolution_clock::now();
        auto time_total = std::chrono::duration_cast<std::chrono::microseconds>(time_now - m_time_start);

        // Calculate the time remaining
        if (current_job > m_progress_prev) {
            m_progress_prev = current_job;

            double speed = current_job / double(time_total.count());
            int time_remaining = int(ceil(double(m_jobs - current_job) / (1000000 * speed)));

            // Wait for the estimate to stabilise
            if (time_total.count() > 1000000) {
                std::ostringstream remaining;
                remaining << "About "
                          << time_remaining
                          << " second"
                          << (time_remaining == 1 ? "" : "s")
                          << " remaining";

                set_text(remaining.str());
            }
        }

        float fraction = float(current_job) / float(m_jobs);
        std::ostringstream ss;
        ss << current_job << " / " << m_jobs;

        m_progressbar->set_fraction(fraction);
        m_progressbar->set_text(ss.str());
    }
}
