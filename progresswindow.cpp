#include <progresswindow.hpp>

namespace ui {
    ProgressWindow::ProgressWindow(BaseObjectType* cobject,
                                   const Glib::RefPtr<Gtk::Builder>& ref_builder)
        : Gtk::Window(cobject) {
        m_jobs = 0;
        m_progressbar = ref_builder->get_widget<Gtk::ProgressBar>("progressbar");

        this->set_modal(true);
        this->set_decorated(false);
    }

    ProgressWindow::~ProgressWindow() {}

    ProgressWindow* ProgressWindow::create() {
        auto ui = Gtk::Builder::create_from_resource("/org/sugol/books/ui/ProgressWindow.ui");
        ProgressWindow* progress_window = Gtk::Builder::get_widget_derived<ProgressWindow>(ui, "progressWindow");

        return progress_window;
    }

    void ProgressWindow::set_jobs(int n_jobs) {
        m_jobs = n_jobs;
    }

    void ProgressWindow::set_progress(int current_job) {
        double fraction = double(current_job) / double(m_jobs);
        std::ostringstream ss;
        ss << current_job << " / " << m_jobs;

        m_progressbar->set_fraction(fraction);
        m_progressbar->set_text(ss.str());
    }
}
