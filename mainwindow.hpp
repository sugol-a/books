#pragma once

#include <atomic>
#include <filesystem>

#include <gtkmm.h>

// #include <featuredetector.hpp>
#include <imagestore.hpp>
// #include <imageloader.hpp>
#include <imageloaderpool.hpp>
#include <featuredetectorpool.hpp>
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
            void change_input_directory();
            void change_output_directory();
            void selected_input_directory(int id);
            void selected_output_directory(int id);

            void margins_changed();

            void begin_import();
            bool import_progress();

            void start_export_worker();
            bool update_export_worker_progress();

            void change_layer();
            void overlay_toggled();

            void selection_changed(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn*);
            void update_preview();

            void do_export();

        private:
            class ImageModelColumns : public Gtk::TreeModel::ColumnRecord {
                public:
                    ImageModelColumns() {
                        add(m_inputName);
                        add(m_outputName);
                        add(m_autoCrop);
                        add(m_fullPath);
                        add(m_imageData);
                    }

                    Gtk::TreeModelColumn<Glib::ustring> m_inputName;
                    Gtk::TreeModelColumn<Glib::ustring> m_outputName;
                    Gtk::TreeModelColumn<bool> m_autoCrop;
                    Gtk::TreeModelColumn<Glib::ustring> m_fullPath;
                    Gtk::TreeModelColumn<std::shared_ptr<img::ImageData>> m_imageData;
            };

            img::ImageStore m_imageStore;

            filter::FilterChain m_filterChain;
            worker::ImageLoaderPool m_imageLoader;
            worker::FeatureDetectorPool m_featureDetector;

            std::filesystem::path m_exportDirectory;

            Gtk::MessageDialog* m_exportErrorDialog;

            Gtk::FileChooserDialog* m_fileChooser;
            sigc::connection m_fileChooserSignal;

            Gtk::Button* m_imageDirButton;
            Gtk::Button* m_exportDirButton;
            Gtk::Button* m_exportButton;
            Gtk::Scale* m_marginScale;
            Gtk::SpinButton* m_layerButton;
            Gtk::CheckButton* m_showFeaturesChk;
            Gtk::CheckButton* m_showFitnessChk;

            Gtk::TreeView* m_fileTreeView;
            Glib::RefPtr<Gtk::ListStore> m_fileListStore;
            ImageModelColumns m_fileColumns;
            Gtk::TreeView::Column m_outputFileColumn;
            std::shared_ptr<img::ImageData> m_currentImage;

            ui::ProgressWindow* m_progressWindow;

            ui::ImagePreview* m_previewPane;
    };
}
