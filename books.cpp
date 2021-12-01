#include <ios>
#include <istream>
#include <iostream>

#include <gtkmm.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <mainwindow.hpp>
#include <imageloader.hpp>
#include <crop.hpp>
#include <ui-resources.h>

#include <filter/filterchain.hpp>

static void on_startup(Glib::RefPtr<Gtk::Application> app) {
    ui::MainWindow* window = ui::MainWindow::create();
    app->add_window(*window);
    window->show();
}

int main(int argc, char *argv[]) {
    {
        CVImage image = cv::imread(argv[1]);
        filter::FilterChain filters;

        cv::Mat result = filters.apply_filters(image);

        cv::imshow("Filter result", result);
        cv::waitKey(0);
        return 0;

        //CVImage cropped = c.auto_crop(image, 100);

        //cv::imshow("Books", cropped);
        //cv::waitKey(0);
    }

    /* {
        Cropper c;
        ImageLoader loader;
        std::vector<Image> images = loader.get_many(argv[1]);

        for (auto& i : images) {
            std::cout << i.filename << std::endl;
            i.load();

            CVImage cropped = c.auto_crop(*i.image, 100);
            cv::imshow("Book", cropped);
            cv::waitKey();
        }
    } */

    auto resource_bundle = Glib::wrap(ui_res_get_resource());
    resource_bundle->register_global();

    auto app = Gtk::Application::create("org.sugol.books");
    app->signal_startup().connect([&]() { on_startup(app); });

    return app->run(argc, argv);
}
