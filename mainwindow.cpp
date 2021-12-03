#include <gtkmm.h>
#include <iostream>

#include <opencv2/highgui.hpp>

#include <mainwindow.hpp>

namespace ui {
    MainWindow::MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ref_builder)
        : Gtk::ApplicationWindow(cobject) {
        m_imageDirButton = ref_builder->get_widget<Gtk::Button>("btnImageDirectory");
        m_exportDirButton = ref_builder->get_widget<Gtk::Button>("btnExportDirectory");
        m_exportButton = ref_builder->get_widget<Gtk::Button>("btnExport");
        m_marginScale = ref_builder->get_widget<Gtk::Scale>("scaleMargins");
        m_layerButton = ref_builder->get_widget<Gtk::SpinButton>("spinBtnLayer");
        m_showFeaturesChk = ref_builder->get_widget<Gtk::CheckButton>("chkShowFeatures");
        m_showFitnessChk = ref_builder->get_widget<Gtk::CheckButton>("chkShowFitness");
        m_fileTreeView = ref_builder->get_widget<Gtk::TreeView>("filesTreeView");
        m_previewPane = Gtk::Builder::get_widget_derived<ui::ImagePreview>(ref_builder, "preview");
        // m_previewPane = Gtk::Builder::get_widget_derived<ui::CropPreview>(ref_builder, "preview");

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

        m_viewLayer = -1;
    }

    MainWindow::~MainWindow() {
        delete m_exportErrorDialog;
    }

    MainWindow* MainWindow::create() {
        auto ui = Gtk::Builder::create_from_resource("/org/sugol/books/ui/MainWindow.ui");
        MainWindow* mainWindow = Gtk::Builder::get_widget_derived<MainWindow>(ui, "mainWindow");

        return mainWindow;
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

        start_list_store_worker();
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
        update_preview();
    }

    void MainWindow::start_list_store_worker() {
        m_listStoreProgress = 0;

        m_listStoreFuture = std::async(std::launch::async, [](img::ImageStore store,
                                                              ft::FeatureDetector feature_detector,
                                                              std::atomic_int* progress) {
            ImageModelColumns model_cols;
            Glib::RefPtr<Gtk::ListStore> list_store = Gtk::ListStore::create(model_cols);

            for (auto& f : store.images()) {
                std::cout << f << std::endl;
                img::Image img(f);
                auto row = *(list_store->append());

                std::filesystem::path image_path = f;
                row[model_cols.m_inputName] = image_path.filename().string();
                row[model_cols.m_outputName] = image_path.filename().string();
                row[model_cols.m_autoCrop] = true;
                row[model_cols.m_fullPath] = f;
                row[model_cols.m_features] =
                    feature_detector.find_candidate_features(img.mat(),
                                                             {
                                                                 ft::distance_to(util::Point<double>{0, 0}),
                                                                 ft::relative_area(0.5)
                                                             },
                                                             {
                                                                 2,
                                                                 5
                                                             }, CV_PRESCALING);
                row[model_cols.m_processingLayers] = feature_detector.filter_chain()->cached();
                (*progress)++;
            }

            return list_store;
        }, m_imageStore, m_featureDetector, &m_listStoreProgress);

        // Create and display a modal progress window
        m_progressWindow = ProgressWindow::create();
        m_progressWindow->set_jobs(m_imageStore.images().size());
        m_progressWindow->set_modal(true);
        m_progressWindow->set_transient_for(*this);
        Glib::signal_idle().connect(sigc::mem_fun(*this, &MainWindow::update_worker_progress));

        m_progressWindow->show();
    }

    bool MainWindow::update_worker_progress() {
        m_progressWindow->set_progress(m_listStoreProgress);

        // Check if the worker's finished
        if (m_listStoreProgress == m_imageStore.images().size()) {
            delete m_progressWindow;

            // Add the new liststore
            m_fileListStore = m_listStoreFuture.get();
            m_fileTreeView->set_model(m_fileListStore);

            // Stop receiving idle updates
            return false;
        }

        return true;
    }

    void MainWindow::change_layer() {
        m_viewLayer = m_layerButton->get_value_as_int();
        update_preview();
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

        update_preview();
    }

    void MainWindow::selection_changed(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn*) {
        update_preview();
    }

    void MainWindow::update_preview() {
        // Get the currently selected row
        auto selection = m_fileTreeView->get_selection();
        auto selected = selection->get_selected();

        if (selected) {
            auto row = *selected;
            Glib::ustring row_value = row[m_fileColumns.m_fullPath];

            // Search for the selected image
            for (const std::string& filename : m_imageStore.images()) {
                if (filename == std::string(row_value)) {
                    img::Image img(filename);
                    std::vector<std::pair<double, util::Box>> features = row[m_fileColumns.m_features];
                    util::Box crop = m_featureDetector.best_candidate(features);

                    // Expand the crop box to include margins
                    crop.expand(m_marginScale->get_value(),
                                util::Box(0, 0, img.pixbuf()->get_width(), img.pixbuf()->get_height()));

                    m_previewPane->set_crop(crop);
                    if (m_viewLayer == -1) {
                        m_previewPane->feature_scale(1);
                        m_previewPane->set_image(img);
                    } else {
                        std::vector<cv::Mat> layers = row[m_fileColumns.m_processingLayers];
                        m_previewPane->feature_scale(CV_PRESCALING);

                        if (m_viewLayer < layers.size()) {
                            m_previewPane->set_image(layers[m_viewLayer]);
                        }
                    }

                    if (row[m_fileColumns.m_autoCrop]) {
                        m_previewPane->set_features(features);
                        m_previewPane->show_crop(true);
                    } else {
                        m_previewPane->show_crop(false);
                    }

                    m_previewPane->queue_draw();
                    break;
                }
            }
        }
    }

    void MainWindow::do_export() {
        if (m_exportDirectory.empty()) {
            m_exportErrorDialog->show();
            m_exportErrorDialog->signal_response().connect(sigc::hide(sigc::mem_fun(*m_exportErrorDialog, &Gtk::Widget::hide)));
            return;
        }

        auto iter = m_fileListStore->children();
        for (size_t i = 0; i < m_imageStore.images().size(); i++) {
            auto row = iter[i];
            Glib::ustring input_filename = row[m_fileColumns.m_fullPath];
            Glib::ustring output_filename = row[m_fileColumns.m_outputName];
            bool do_crop = row[m_fileColumns.m_autoCrop];

            std::string image_filename = m_imageStore.images()[i];
            img::Image img(image_filename);

            cv::Mat output_image;
            util::Box crop = m_featureDetector.best_candidate(row[m_fileColumns.m_features]);

            if (do_crop && crop.area() > 0) {
                crop.expand(m_marginScale->get_value(),
                            util::Box(0, 0, img.mat().size().width, img.mat().size().height));
                output_image = img.mat()(crop);
            } else {
                output_image = img.mat();
            }

            std::filesystem::path full_output_path = m_exportDirectory.string() + "/" + std::string(output_filename);
            cv::imwrite(full_output_path.string(), output_image);

            std::cout << input_filename << " -> " << output_filename << ", " << do_crop << std::endl;
        }
    }
}
