#include <gtkmm.h>
#include <mainwindow.hpp>
#include <ui-resources.h>

static void on_startup(Glib::RefPtr<Gtk::Application> app) {
    ui::MainWindow* window = ui::MainWindow::create();
    app->add_window(*window);
    window->show();
}

int main(int argc, char *argv[]) {
    auto resource_bundle = Glib::wrap(ui_res_get_resource());
    resource_bundle->register_global();

    auto app = Gtk::Application::create("org.sugol.books");
    app->signal_startup().connect([&]() { on_startup(app); });

    return app->run(argc, argv);
}
