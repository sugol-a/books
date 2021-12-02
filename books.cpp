#include <gtkmm.h>

#include <mainwindow.hpp>
#include <ui-resources.h>

static void on_startup(Glib::RefPtr<Gtk::Application> app) {
    ui::MainWindow* window = ui::MainWindow::create();
    app->add_window(*window);
    window->show();
}

// #include <iostream>
// #include <image.hpp>
// #include <imagestore.hpp>

// int main(int argc, char *argv[]) {
//     img::ImageStore store;
//     store.populate(argv[1]);

//     for (std::string filename : store.images()) {
//         img::Image image(filename);
//         std::cout << image.filename() << std::endl;
//         std::cout << image.mat().size().width
//                   << " "
//                   << image.mat().size().height
//                   << std::endl;
//     }

//     return 0;
// }

int main(int argc, char *argv[]) {
    auto resource_bundle = Glib::wrap(ui_res_get_resource());
    resource_bundle->register_global();

    auto app = Gtk::Application::create("org.sugol.books");
    app->signal_startup().connect([&]() { on_startup(app); });

    return app->run(argc, argv);
}
