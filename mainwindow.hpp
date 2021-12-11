#pragma once

#include <atomic>
#include <filesystem>
#include <optional>

#include <gtkmm.h>

#include <imagestore.hpp>
#include <imageloaderpool.hpp>
#include <featuredetectorpool.hpp>
#include <featuredetectorparams.hpp>
#include <imageexporterpool.hpp>
#include <imagepreview.hpp>
#include <progresswindow.hpp>

namespace ui {
    class MainWindow : public Gtk::ApplicationWindow {
        public:
            MainWindow(BaseObjectType* cobject,
                       const Glib::RefPtr<Gtk::Builder>& ref_builder);

            ~MainWindow();

            static MainWindow* create();

        private:
            std::shared_ptr<img::ImageData> selected_image();
            std::optional<Gtk::TreeRow> selected_row();

            void show_file_selector();

            void import_button_clicked();
            void export_button_clicked();

            void change_input_directory();
            void change_output_directory();
            void selected_input_directory(int id);
            void selected_export_directory(int id);

            void next_image();
            void prev_image();

            void margins_changed(int amount);

            void update_filter_params();

            void begin_import();
            bool import_progress();
            bool cancel_import();

            void begin_export();
            bool export_progress();
            bool cancel_export();

            void start_export_worker();
            bool update_export_worker_progress();

            void overlay_toggled();

            void selection_changed();
            void update_preview();

        private:
            class ImageModelColumns : public Gtk::TreeModel::ColumnRecord {
                public:
                    ImageModelColumns() {
                        add(m_outputName);
                        add(m_autoCrop);
                        add(m_fullPath);
                        add(m_imageData);
                        add(m_cropRect);
                    }

                    Gtk::TreeModelColumn<Glib::ustring> m_outputName;
                    Gtk::TreeModelColumn<bool> m_autoCrop;
                    Gtk::TreeModelColumn<Glib::ustring> m_fullPath;
                    Gtk::TreeModelColumn<std::shared_ptr<img::ImageData>> m_imageData;
                    Gtk::TreeModelColumn<std::shared_ptr<Gdk::Rectangle>> m_cropRect;
            };

            img::ImageStore m_imageStore;

            worker::ImageLoaderPool* m_imageLoader;
            worker::FeatureDetectorPool* m_featureDetector;
            worker::ImageExporterPool* m_imageExporter;
            worker::FeatureDetectorParams m_featureDetectorParams;

            std::filesystem::path m_exportDirectory;

            Gtk::MessageDialog* m_exportErrorDialog;

            Glib::RefPtr<Gtk::FileChooserNative> m_fileChooser;
            sigc::connection m_fileChooserSignal;

            Gtk::Button* m_importButton;
            Gtk::Button* m_exportButton;
            Gtk::Button* m_reloadButton;

            Gtk::Button* m_nextImageButton;
            Gtk::Button* m_prevImageButton;

            Gtk::Button* m_marginAddButton;
            Gtk::Button* m_marginSubtractButton;
            Gtk::Switch* m_showFeaturesSwitch;
            Gtk::Switch* m_showFitnessSwitch;

            Gtk::SpinButton* m_blurKernelSpin;
            Gtk::SpinButton* m_dilateKernelSpin;
            Gtk::SpinButton* m_thresholdSpin;

            Gtk::TreeView* m_fileTreeView;
            Glib::RefPtr<Gtk::ListStore> m_fileListStore;
            ImageModelColumns m_fileColumns;
            Gtk::TreeView::Column m_outputFileColumn;
            std::shared_ptr<img::ImageData> m_currentImage;

            ui::ProgressWindow* m_progressWindow;

            ui::ImagePreview* m_previewPane;

            size_t m_margins;
    };
}
