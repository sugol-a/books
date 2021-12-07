#include <gtkmm.h>
#include <iostream>

#include <opencv2/highgui.hpp>

#include <mainwindow.hpp>

namespace ui {
    MainWindow::MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ref_builder)
        : Gtk::ApplicationWindow(cobject) {
#ifdef DEBUG
        std::cout << "THIS IS A DEBUG BUILD" << std::endl;
#endif
        m_imageDirButton = ref_builder->get_widget<Gtk::Button>("btnImageDirectory");
        m_exportDirButton = ref_builder->get_widget<Gtk::Button>("btnExportDirectory");
        m_exportButton = ref_builder->get_widget<Gtk::Button>("btnExport");
        m_marginScale = ref_builder->get_widget<Gtk::Scale>("scaleMargins");
        m_layerButton = ref_builder->get_widget<Gtk::SpinButton>("spinBtnLayer");
        m_showFeaturesChk = ref_builder->get_widget<Gtk::CheckButton>("chkShowFeatures");
        m_showFitnessChk = ref_builder->get_widget<Gtk::CheckButton>("chkShowFitness");
        m_fileTreeView = ref_builder->get_widget<Gtk::TreeView>("filesTreeView");
        m_previewPane = Gtk::Builder::get_widget_derived<ui::ImagePreview>(ref_builder, "preview");

        m_imageDirButton->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::change_input_directory));
        m_exportDirButton->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::change_output_directory));
        m_exportButton->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::do_export));
        m_marginScale->signal_value_changed().connect(sigc::mem_fun(*this, &MainWindow::margins_changed));
        m_layerButton->signal_value_changed().connect(sigc::mem_fun(*this, &MainWindow::change_layer));
        m_showFeaturesChk->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::overlay_toggled));
        m_showFitnessChk->signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::overlay_toggled));

        // m_fileListStore = Gtk::ListStore::create(m_fileColumns);
        // m_fileTreeView->set_model(m_fileListStore);
        m_fileTreeView->append_column("File", m_fileColumns.m_inputName);
        m_fileTreeView->append_column_editable("Output", m_fileColumns.m_outputName);
        m_fileTreeView->append_column_editable("Autocrop", m_fileColumns.m_autoCrop);
        m_fileTreeView->signal_row_activated().connect(sigc::mem_fun(*this, &MainWindow::selection_changed));

        m_exportErrorDialog = new Gtk::MessageDialog(*this,
                                                     "Please select an export directory",
                                                     false,
                                                     Gtk::MessageType::ERROR,
                                                     Gtk::ButtonsType::OK,
                                                     true);

    }

    MainWindow::~MainWindow() {
        delete m_exportErrorDialog;
    }

    MainWindow* MainWindow::create() {
        auto ui = Gtk::Builder::create_from_resource("/org/sugol/books/ui/MainWindow.ui");
        MainWindow* mainWindow = Gtk::Builder::get_widget_derived<MainWindow>(ui, "mainWindow");

        return mainWindow;
    }

    std::shared_ptr<img::ImageData> MainWindow::selected_image() {
        // Get the currently selected row
        auto selection = m_fileTreeView->get_selection();
        auto selected = selection->get_selected();
        auto row = *selected;

        if (!row)
            return nullptr;

        return row[m_fileColumns.m_imageData];
    }

    void MainWindow::change_input_directory() {
        // Gtk4 removed the file chooser button, so this is what we get :/
        m_fileChooser = new Gtk::FileChooserDialog("Select a folder", Gtk::FileChooser::Action::SELECT_FOLDER);
        m_fileChooser->set_transient_for(*this);
        m_fileChooser->set_modal(true);
        m_fileChooser->add_button("_Cancel", Gtk::ResponseType::CANCEL);
        m_fileChooser->add_button("_Open", Gtk::ResponseType::OK);
        m_fileChooserSignal = m_fileChooser->signal_response().connect(sigc::mem_fun(*this, &MainWindow::selected_input_directory));
        m_fileChooser->show();    
    }

    void MainWindow::change_output_directory() {
        m_fileChooser = new Gtk::FileChooserDialog("Select a folder", Gtk::FileChooser::Action::SELECT_FOLDER);
        m_fileChooser->set_transient_for(*this);
        m_fileChooser->set_modal(true);
        m_fileChooser->add_button("_Cancel", Gtk::ResponseType::CANCEL);
        m_fileChooser->add_button("_Open", Gtk::ResponseType::OK);
        m_fileChooserSignal = m_fileChooser->signal_response().connect(sigc::mem_fun(*this, &MainWindow::selected_output_directory));
        m_fileChooser->show();
    }

    void MainWindow::selected_input_directory(int id) {
        if (id != Gtk::ResponseType::OK) {
            delete m_fileChooser;
            return;
        }

        std::string directory = m_fileChooser->get_file()->get_path();

        m_imageStore.clear();
        m_imageStore.populate(directory);
        m_imageDirButton->set_label(directory);

        m_fileChooserSignal.disconnect();
        delete m_fileChooser;

        begin_import();
    }

    void MainWindow::selected_output_directory(int id) {
        if (id != Gtk::ResponseType::OK) {
            delete m_fileChooser;
            return;
        }

        m_exportDirectory = m_fileChooser->get_file()->get_path();
        m_exportDirButton->set_label(m_exportDirectory.filename().string());

        m_fileChooserSignal.disconnect();
        delete m_fileChooser;
    }

    void MainWindow::margins_changed() {
        m_previewPane->queue_draw();
    }

    void MainWindow::begin_import() {
        // Automagically determine the number of threads to use for each task
        std::vector<size_t> thread_allocations = worker::thread_allocations({ 2, 1 });

        std::cout << "Using " << thread_allocations[0] << " threads for image loading" << std::endl;
        std::cout << "Using " << thread_allocations[1] << " threads for feature detection" << std::endl;

        m_imageLoader = new worker::ImageLoaderPool(thread_allocations[0]);
        m_featureDetector = new worker::FeatureDetectorPool(thread_allocations[1], {
                ft::distance_to(util::Point<double>{0, 0}),
                ft::relative_area(0.5)
            },
            {
                2,
                5
            });

        m_imageLoader->input()->push_vec(m_imageStore.images());
        m_imageLoader->input()->finish();

        // Link the image loader and feature detector pools
        m_featureDetector->set_input(m_imageLoader->output());

        m_imageLoader->run_workers();
        m_featureDetector->run_workers();

        // Create and display a modal progress window
        m_progressWindow = ProgressWindow::create();
        m_progressWindow->set_jobs(m_imageStore.images().size());
        m_progressWindow->set_modal(true);
        m_progressWindow->set_transient_for(*this);
        Glib::signal_idle().connect(sigc::mem_fun(*this, &MainWindow::import_progress));

        m_progressWindow->show();
    }

    bool MainWindow::import_progress() {
        m_progressWindow->set_progress(m_featureDetector->output()->size());

        // Check if the worker's finished. The result queue may be larger than
        // the actual number of images due to sentinel values
        if (m_featureDetector->output()->size() >= m_imageStore.images().size()) {
            // Add the new liststore
            m_fileListStore = Gtk::ListStore::create(m_fileColumns);

            std::shared_ptr<img::ImageData> image_data = nullptr;
            while ((image_data = m_featureDetector->output()->pop()) != nullptr) {
                std::filesystem::path file_path(image_data->filename());
                auto row = *(m_fileListStore->append());

                row[m_fileColumns.m_inputName] = file_path.filename().string();
                row[m_fileColumns.m_outputName] = file_path.filename().string();
                row[m_fileColumns.m_autoCrop] = true;
                row[m_fileColumns.m_fullPath] = image_data->filename();
                row[m_fileColumns.m_imageData] = image_data;
            }

            m_fileTreeView->set_model(m_fileListStore);

            m_progressWindow->close();
            delete m_progressWindow;

            delete m_imageLoader;
            delete m_featureDetector;

            // Stop receiving idle updates
            return false;
        }

        return true;
    }

    void MainWindow::start_export_worker() {
    }

    bool MainWindow::update_export_worker_progress() {

    }

    void MainWindow::change_layer() {
    }

    void MainWindow::overlay_toggled() {
        m_previewPane->show_features(m_showFeaturesChk->get_active());

        // It only makes sense to show fitness scores when features are enabled,
        // so disable fitness scores when they're not
        if (!m_showFeaturesChk->get_active()) {
            m_showFitnessChk->set_active(false);
            m_showFitnessChk->set_sensitive(false);
        } else {
            m_showFitnessChk->set_sensitive(true);
        }

        m_previewPane->show_fitness(m_showFitnessChk->get_active());
        m_previewPane->queue_draw();
    }

    void MainWindow::selection_changed(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn*) {
        update_preview();
    }

    void MainWindow::update_preview() {
        // Unload the currently loaded image
        if (m_currentImage) {
            m_currentImage->unload();
        }

        // Load the new image
        m_currentImage = selected_image();
        if (!m_currentImage)
            return;

        m_currentImage->load(m_currentImage->filename());
        m_previewPane->set_image(m_currentImage);
        m_previewPane->queue_draw();
    }

    void MainWindow::do_export() {
        if (m_exportDirectory.empty()) {
            m_exportErrorDialog->show();
            m_exportErrorDialog->signal_response().connect(sigc::hide(sigc::mem_fun(*m_exportErrorDialog, &Gtk::Widget::hide)));
            return;
        }

        auto iter = m_fileListStore->children();
        std::cout << iter.size() << std::endl;
        for (auto& row : iter) {
            Glib::ustring input_filename = row[m_fileColumns.m_fullPath];
            Glib::ustring output_filename = row[m_fileColumns.m_outputName];
            bool do_crop = row[m_fileColumns.m_autoCrop];
            std::shared_ptr<img::ImageData> image_data = row[m_fileColumns.m_imageData];

            image_data->load(image_data->filename());
            util::Box crop = image_data->candidate().second;

            cv::Mat output_image;
            if (do_crop && crop.area() > 0) {
                // crop.expand(m_marginScale->get_value(),
                //             util::Box(0, 0, img.mat().size().width, img.mat().size().height));
                output_image = image_data->mat()(crop);
            } else {
                output_image = image_data->mat();
            }

            std::filesystem::path full_output_path = m_exportDirectory.string() + "/" + std::string(output_filename);

            // ImageData stores its cv::Mat as RGB data, but imwrite expects BGR
            cv::cvtColor(output_image, output_image, cv::COLOR_RGB2BGR);

            cv::imwrite(full_output_path.string(), output_image);

            image_data->unload();
            // std::cout << input_filename << " -> " << output_filename << ", " << do_crop << std::endl;
        }
    }
}
