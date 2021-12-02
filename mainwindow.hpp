#pragma once

#include <gtkmm.h>

#include <featuredetector.hpp>
#include <imagestore.hpp>
#include <croppreview.hpp>
#include <imageloader.hpp>
#include <imagepreview.hpp>

namespace ui {
    class MainWindow : public Gtk::ApplicationWindow {
        public:
            MainWindow(BaseObjectType* cobject,
                       const Glib::RefPtr<Gtk::Builder>& ref_builder);

            ~MainWindow();

            static MainWindow* create();

        private:
            static constexpr float CV_PRESCALING = 0.1;

            void change_input_directory();
            void change_output_directory();
            void selected_input_directory(int id);
            void selected_output_directory(int id);

            void change_layer();

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
                        add(m_features);
                        add(m_processingLayers);
                    }

                    Gtk::TreeModelColumn<Glib::ustring> m_inputName;
                    Gtk::TreeModelColumn<Glib::ustring> m_outputName;
                    Gtk::TreeModelColumn<bool> m_autoCrop;
                    Gtk::TreeModelColumn<Glib::ustring> m_fullPath;
                    Gtk::TreeModelColumn<std::vector<std::pair<double,util::Box>>> m_features;
                    Gtk::TreeModelColumn<std::vector<cv::Mat>> m_processingLayers;
            };

            img::ImageStore m_imageStore;
            ft::FeatureDetector m_featureDetector;

            std::filesystem::path m_exportDirectory;

            Gtk::MessageDialog* m_exportErrorDialog;

            Gtk::FileChooserDialog* m_fileChooser;
            sigc::connection m_fileChooserSignal;

            Gtk::Button* m_imageDirButton;
            Gtk::Button* m_exportDirButton;
            Gtk::Button* m_exportButton;
            Gtk::SpinButton* m_layerButton;

            Gtk::TreeView* m_fileTreeView;
            Glib::RefPtr<Gtk::ListStore> m_fileListStore;
            ImageModelColumns m_fileColumns;
            Gtk::TreeView::Column m_outputFileColumn;

            ui::ImagePreview* m_previewPane;
            int m_viewLayer;
    };
}
