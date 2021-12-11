#include <iostream>
#include <string>

#include <gtkmm.h>

#include <mainwindow.hpp>

namespace ui {
    MainWindow::MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ref_builder)
        : Gtk::ApplicationWindow(cobject) {
#ifdef DEBUG
        std::cout << "THIS IS A DEBUG BUILD" << std::endl;
#endif
        m_importButton = ref_builder->get_widget<Gtk::Button>("btnImport");
        m_exportButton = ref_builder->get_widget<Gtk::Button>("btnExport");
        m_reloadButton = ref_builder->get_widget<Gtk::Button>("btnReload");

        m_marginAddButton = ref_builder->get_widget<Gtk::Button>("btnMarginAdd");
        m_marginSubtractButton = ref_builder->get_widget<Gtk::Button>("btnMarginSubtract");

        m_showFeaturesSwitch = ref_builder->get_widget<Gtk::Switch>("switchShowFeatures");
        m_showFitnessSwitch = ref_builder->get_widget<Gtk::Switch>("switchShowFitness");

        m_blurKernelSpin = ref_builder->get_widget<Gtk::SpinButton>("spinBlurKernel");
        m_dilateKernelSpin = ref_builder->get_widget<Gtk::SpinButton>("spinDilateKernel");
        m_thresholdSpin = ref_builder->get_widget<Gtk::SpinButton>("spinThreshold");

        m_fileTreeView = ref_builder->get_widget<Gtk::TreeView>("filesTreeView");
        m_previewPane = Gtk::Builder::get_widget_derived<ui::ImagePreview>(ref_builder, "preview");

        m_importButton->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::import_button_clicked));
        m_exportButton->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::export_button_clicked));
        m_reloadButton->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::begin_import));

        m_marginAddButton->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::margins_changed), 5));
        m_marginSubtractButton->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::margins_changed), -5));

        m_showFeaturesSwitch->property_active().signal_changed().connect(sigc::mem_fun(*this, &MainWindow::overlay_toggled));
        m_showFitnessSwitch->property_active().signal_changed().connect(sigc::mem_fun(*this, &MainWindow::overlay_toggled));

        m_blurKernelSpin->signal_value_changed().connect(sigc::mem_fun(*this, &MainWindow::update_filter_params));
        m_dilateKernelSpin->signal_value_changed().connect(sigc::mem_fun(*this, &MainWindow::update_filter_params));
        m_thresholdSpin->signal_value_changed().connect(sigc::mem_fun(*this, &MainWindow::update_filter_params));

        m_fileListStore = Gtk::ListStore::create(m_fileColumns);
        m_fileTreeView->set_model(m_fileListStore);
        m_fileTreeView->append_column_editable("File", m_fileColumns.m_outputName);
        m_fileTreeView->append_column_editable("Autocrop", m_fileColumns.m_autoCrop);

        // Make all of the columns sortable
        m_fileTreeView->get_column(0)->set_sort_column(m_fileColumns.m_outputName);
        m_fileTreeView->get_column(1)->set_sort_column(m_fileColumns.m_autoCrop);
        m_fileTreeView->get_column(0)->set_resizable();
        m_fileTreeView->get_column(1)->set_resizable();

        // Make the treeview searchable
        m_fileTreeView->set_search_column(0);
        m_fileTreeView->set_enable_search();
        m_fileTreeView->set_search_equal_func([](const Glib::RefPtr<Gtk::TreeModel>&,
                                                 int col,
                                                 const Glib::ustring& key,
                                                 const Gtk::TreeModel::const_iterator& iter) -> bool {
            // Just check whether the filename contains the key. Default is
            // to check whether the field starts with the key.
            std::string name;
            iter->get_value<std::string>(col, name);
            return name.find(key) == std::string::npos;
        });

        // m_fileTreeView->signal_row_activated().connect(sigc::mem_fun(*this, &MainWindow::selection_changed));
        m_fileTreeView->signal_cursor_changed().connect(sigc::mem_fun(*this, &MainWindow::selection_changed));

        m_exportErrorDialog = new Gtk::MessageDialog(*this,
                                                     "Please select an export directory",
                                                     false,
                                                     Gtk::MessageType::ERROR,
                                                     Gtk::ButtonsType::OK,
                                                     true);

        // Read in the default filter parameters from the UI file
        update_filter_params();

        m_margins = 0;
    }

    MainWindow::~MainWindow() {
        delete m_exportErrorDialog;
    }

    MainWindow* MainWindow::create() {
        auto ui = Gtk::Builder::create_from_resource("/org/sugol/books/ui/MainWindow.ui");
        MainWindow* mainWindow = Gtk::Builder::get_widget_derived<MainWindow>(ui, "mainWindow");

        return mainWindow;
    }

    void MainWindow::import_button_clicked() {
        m_fileChooser = Gtk::FileChooserNative::create("Select a folder",
                                                       Gtk::FileChooser::Action::SELECT_FOLDER,
                                                       "_Open",
                                                       "_Cancel");

        m_fileChooser->set_transient_for(*this);
        m_fileChooser->set_modal(true);
        m_fileChooserSignal = m_fileChooser->signal_response().connect(sigc::mem_fun(*this, &MainWindow::selected_input_directory));
        m_fileChooser->show();
    }

    void MainWindow::export_button_clicked() {
        m_fileChooser = Gtk::FileChooserNative::create("Select a folder",
                                                       Gtk::FileChooser::Action::SELECT_FOLDER,
                                                       "_Open",
                                                       "_Cancel");
        m_fileChooser->set_transient_for(*this);
        m_fileChooser->set_modal(true);
        m_fileChooserSignal = m_fileChooser->signal_response().connect(sigc::mem_fun(*this, &MainWindow::selected_export_directory));
        m_fileChooser->show();
    }

    std::optional<Gtk::TreeRow> MainWindow::selected_row() {
        auto selection = m_fileTreeView->get_selection();
        if (selection && selection->get_selected()) {
            return *selection->get_selected();
        } else {
            return {};
        }
    }

    std::shared_ptr<img::ImageData> MainWindow::selected_image() {
        auto row = selected_row();

        if (!row)
            return nullptr;

        return row.value()[m_fileColumns.m_imageData];
    }

    void MainWindow::selected_input_directory(int id) {
        if (id != Gtk::ResponseType::ACCEPT) {
            return;
        }

        std::string directory = m_fileChooser->get_file()->get_path();

        m_imageStore.clear();
        m_imageStore.populate(directory);

        m_fileChooserSignal.disconnect();

        begin_import();
    }

    void MainWindow::selected_export_directory(int id) {
        if (id != Gtk::ResponseType::ACCEPT) {
            return;
        }

        m_exportDirectory = m_fileChooser->get_file()->get_path();
        m_fileChooserSignal.disconnect();

        begin_export();
    }

    void MainWindow::margins_changed(int amount) {
        for (auto row : m_fileListStore->children()) {
            std::shared_ptr<Gdk::Rectangle> rect = row[m_fileColumns.m_cropRect];
            std::shared_ptr<img::ImageData> image_data = row[m_fileColumns.m_imageData];

            int x, y, w, h;

            x = rect->get_x();
            y = rect->get_y();
            w = rect->get_width();
            h = rect->get_height();

            // Grow the crop rectangle (bounded by the size of the image)
            x = std::max(0, x - amount);
            y = std::max(0, y - amount);
            w = std::min(image_data->dimensions().first, w + 2 * amount);
            h = std::min(image_data->dimensions().second, h + 2 * amount);

            rect->set_x(x);
            rect->set_y(y);
            rect->set_width(w);
            rect->set_height(h);
        }

        update_preview();
    }

    void MainWindow::update_filter_params() {
        size_t threshold = m_thresholdSpin->get_value();
        size_t blur_size = m_blurKernelSpin->get_value();
        size_t dilate_size = m_dilateKernelSpin->get_value();

        m_featureDetectorParams.blur_kernel_size = blur_size;
        m_featureDetectorParams.dilate_kernel_size = dilate_size;
        m_featureDetectorParams.threshold = threshold;
    }

    void MainWindow::begin_import() {
        // If no files have been selected, then there's no work to do
        if (m_imageStore.images().size() == 0) {
            return;
        }

        // Automagically determine the number of threads to use for each task
        std::vector<size_t> thread_allocations = worker::thread_allocations({ 2, 1 });

        std::cout << "Using " << thread_allocations[0] << " threads for image loading" << std::endl;
        std::cout << "Using " << thread_allocations[1] << " threads for feature detection" << std::endl;

        m_imageLoader = new worker::ImageLoaderPool(thread_allocations[0]);
        m_featureDetector = new worker::FeatureDetectorPool(thread_allocations[1], {
                ft::distance_to(util::Point<double>{0, 0}),
                ft::relative_area(0.5)
            },
            { 2, 5},
            m_featureDetectorParams);

        m_imageLoader->input()->push_vec(m_imageStore.images());
        m_imageLoader->input()->finish();

        // Link the image loader and feature detector pools
        m_featureDetector->set_input(m_imageLoader->output());

        m_imageLoader->run_workers();
        m_featureDetector->run_workers();

        m_fileTreeView->get_selection()->unselect_all();

        // Create and display a modal progress window
        m_progressWindow = ProgressWindow::create();
        m_progressWindow->set_jobs(m_imageStore.images().size());
        m_progressWindow->set_title("Importing images");
        m_progressWindow->set_modal(true);
        m_progressWindow->set_transient_for(*this);
        Glib::signal_idle().connect(sigc::mem_fun(*this, &MainWindow::import_progress));

        m_progressWindow->signal_close_request().connect(sigc::mem_fun(*this, &MainWindow::cancel_import), false);

        m_progressWindow->show();
    }

    bool MainWindow::import_progress() {
        if (m_featureDetector->stopped()) {
            // This operation has been cancelled
            delete m_progressWindow;
            delete m_imageLoader;
            delete m_featureDetector;

            return false;
        }

        m_progressWindow->set_progress(m_featureDetector->output()->size());

        // Check if the worker's finished. The result queue may be larger than
        // the actual number of images due to sentinel values
        if (m_featureDetector->output()->size() >= m_imageStore.images().size()) {
            // Clear the old contents of the list store
            m_fileListStore->clear();

            std::shared_ptr<img::ImageData> image_data = nullptr;
            while ((image_data = m_featureDetector->output()->pop()) != nullptr) {
                std::filesystem::path file_path(image_data->filename());
                auto row = *(m_fileListStore->append());

                row[m_fileColumns.m_outputName] = file_path.filename().string();
                row[m_fileColumns.m_autoCrop] = true;
                row[m_fileColumns.m_fullPath] = image_data->filename();
                row[m_fileColumns.m_imageData] = image_data;
                row[m_fileColumns.m_cropRect] = std::make_shared<Gdk::Rectangle>(image_data->candidate().second);
            }

            m_progressWindow->close();
            delete m_progressWindow;

            delete m_imageLoader;
            delete m_featureDetector;

            // Stop receiving idle updates
            return false;
        }

        return true;
    }

    bool MainWindow::cancel_import() {
        m_imageLoader->stop();
        m_featureDetector->stop();

        return true;
    }

    void MainWindow::begin_export() {
        std::vector<std::pair<std::shared_ptr<img::ImageData>, worker::ExportParameters>> export_data;

        auto iter = m_fileListStore->children();
        for (auto& row : iter) {
            Glib::ustring output_filename = row[m_fileColumns.m_outputName];
            bool do_crop = row[m_fileColumns.m_autoCrop];
            std::shared_ptr<img::ImageData> image_data = row[m_fileColumns.m_imageData];

            worker::ExportParameters params = {
                m_exportDirectory.string(),
                m_margins,
                output_filename,
                do_crop
            };

            auto pair = std::pair<std::shared_ptr<img::ImageData>, worker::ExportParameters>(
                image_data,
                params);

            export_data.push_back(pair);
        }

        // Create and run the workers
        std::vector<size_t> thread_allocations = worker::thread_allocations({1});

        std::cout << "Exporting with " << thread_allocations[0] << " threads" << std::endl;

        m_imageExporter = new worker::ImageExporterPool(thread_allocations[0]);
        m_imageExporter->input()->push_vec(export_data);
        m_imageExporter->input()->finish();
        m_imageExporter->run_workers();

        // Create and display a progress window
        m_progressWindow = ProgressWindow::create();
        m_progressWindow->set_jobs(m_imageStore.images().size());
        m_progressWindow->set_title("Exporting images");
        m_progressWindow->set_modal(true);
        m_progressWindow->set_transient_for(*this);
        Glib::signal_idle().connect(sigc::mem_fun(*this, &MainWindow::export_progress));
        m_progressWindow->signal_close_request().connect(sigc::mem_fun(*this, &MainWindow::cancel_export), false);

        m_progressWindow->show();
    }

    bool MainWindow::export_progress() {
        if (m_imageExporter->stopped()) {
            delete m_progressWindow;
            delete m_imageExporter;
        }

        m_progressWindow->set_progress(m_imageExporter->output()->size());

        if (m_imageExporter->output()->size() >= m_imageStore.images().size()) {
            m_progressWindow->close();

            delete m_progressWindow;
            delete m_imageExporter;

            // The currently exported image will get unloaded during the export
            // process, so the preview needs to be reloaded
            update_preview();

            return false;
        }

        return true;
    }

    bool MainWindow::cancel_export() {
        m_imageExporter->stop();

        return true;
    }

    void MainWindow::overlay_toggled() {
        m_previewPane->show_features(m_showFeaturesSwitch->get_active());

        // It only makes sense to show fitness scores when features are enabled,
        // so disable fitness scores when they're not
        if (!m_showFeaturesSwitch->get_active()) {
            m_showFitnessSwitch->set_active(false);
            m_showFitnessSwitch->set_sensitive(false);
        } else {
            m_showFitnessSwitch->set_sensitive(true);
        }

        m_previewPane->show_fitness(m_showFitnessSwitch->get_active());
        m_previewPane->queue_draw();
    }

    void MainWindow::selection_changed() {
        update_preview();
    }

    void MainWindow::update_preview() {
        m_currentImage = selected_image();
        if (!m_currentImage)
            return;

        auto row = selected_row();
        if (row) {
            m_previewPane->set_crop(row.value()[m_fileColumns.m_cropRect]);
            m_previewPane->set_filename(m_currentImage->filename());
            m_previewPane->show_crop(row.value()[m_fileColumns.m_autoCrop]);
        }
    }
}
