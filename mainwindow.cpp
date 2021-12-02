#include <gtkmm.h>
#include <iostream>

#include <iterator>
#include <mainwindow.hpp>

namespace ui {
    MainWindow::MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& ref_builder)
    : Gtk::ApplicationWindow(cobject) {
        m_imageDirButton = ref_builder->get_widget<Gtk::Button>("btnImageDirectory");
        m_exportDirButton = ref_builder->get_widget<Gtk::Button>("btnExportDirectory");
        m_exportButton = ref_builder->get_widget<Gtk::Button>("btnExport");
        m_layerButton = ref_builder->get_widget<Gtk::SpinButton>("spinBtnLayer");
        m_fileTreeView = ref_builder->get_widget<Gtk::TreeView>("filesTreeView");
        m_previewPane = Gtk::Builder::get_widget_derived<ui::ImagePreview>(ref_builder, "preview");
        // m_previewPane = Gtk::Builder::get_widget_derived<ui::CropPreview>(ref_builder, "preview");

        m_imageDirButton->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::change_input_directory));
        m_exportDirButton->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::change_output_directory));
        m_exportButton->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::do_export));
        m_layerButton->signal_value_changed().connect(sigc::mem_fun(*this, &MainWindow::change_layer));

        m_fileListStore = Gtk::ListStore::create(m_fileColumns);
        m_fileTreeView->set_model(m_fileListStore);
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

        std::filesystem::path file_path = m_fileChooser->get_file()->get_path();
        m_imageDirButton->set_label(file_path.filename().string());

        m_imageFiles = m_imageLoader.get_many(m_fileChooser->get_file()->get_path());
        for (auto& img : m_imageFiles) {
            auto row = *(m_fileListStore->append());
            row[m_fileColumns.m_inputName] = img.filename.filename().string();
            row[m_fileColumns.m_outputName] = img.filename.filename().string();
            row[m_fileColumns.m_autoCrop] = true;
            row[m_fileColumns.m_fullPath] = img.filename.string();
        }

        m_fileChooserSignal.disconnect();
        delete m_fileChooser;
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

    void MainWindow::change_layer() {
        m_viewLayer = m_layerButton->get_value_as_int();
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

            // Search for the selected image
            for (Image& i : m_imageFiles) {
                Glib::ustring row_value = row[m_fileColumns.m_fullPath];
                if (i.filename == std::string(row_value)) {
                    i.load();
                    cv::Rect crop = get_crop_rect(i);

                    if (m_viewLayer == -1) {
                        m_previewPane->set_image(*i.image);

                    } else {
                        filter::FilterChain& chain = m_cropper.get_filterchain();
                        if (m_viewLayer < chain.length()) {
                            m_previewPane->set_image(chain.get_cached(m_viewLayer));
                        }
                    }

                    if (row[m_fileColumns.m_autoCrop]) {
                        m_previewPane->set_box(crop);
                        m_previewPane->set_show_box(true);
                        // m_previewPane->set_crop(crop);
                        // m_previewPane->set_enable(true);
                    } else {
                        m_previewPane->set_show_box(false);
                        //m_previewPane->set_enable(false);
                    }

                    m_previewPane->queue_draw();
                    break;
                }
            }
        }
    }

    cv::Rect MainWindow::get_crop_rect(Image& image) {
        //if (m_cropRects.find(image.filename) != m_cropRects.end()) {
        //    return m_cropRects[image.filename];
        //} else {
            // No crop rectangle has been calculated, calculate and cache it
        //    image.load();
            m_cropRects[image.filename] = m_cropper.auto_crop(*image.image);
            return m_cropRects[image.filename];
        //}
    }

    void MainWindow::do_export() {
        if (m_exportDirectory.empty()) {
            m_exportErrorDialog->show();
            m_exportErrorDialog->signal_response().connect(sigc::hide(sigc::mem_fun(*m_exportErrorDialog, &Gtk::Widget::hide)));
            return;
        }

        auto iter = m_fileListStore->children();
        for (size_t i = 0; i < m_imageFiles.size(); i++) {
            auto row = iter[i];
            auto input_filename = row[m_fileColumns.m_fullPath];
            Glib::ustring output_filename = row[m_fileColumns.m_outputName];
            bool do_crop = row[m_fileColumns.m_autoCrop];

            auto image = m_imageFiles[i];
            image.load();

            cv::Mat output_image;
            cv::Rect crop_rect = get_crop_rect(image);
            if (do_crop && crop_rect.area() > 0) {
                output_image = (*image.image)(crop_rect);
            } else {
                output_image = *image.image;
            }

            std::filesystem::path full_output_path = m_exportDirectory.string() + "/" + std::string(output_filename);
            cv::imwrite(full_output_path.string(), output_image);

            std::cout << input_filename << " -> " << output_filename << ", " << do_crop << std::endl;
        }
    }
}
