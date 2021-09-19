#include <gtkmm.h>

#include <croppreview.hpp>
#include <imageloader.hpp>

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
        
        void change_preview(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn*);

        cv::Rect get_crop_rect(Image& image);

        void do_export();

        private:
        class ImageModelColumns : public Gtk::TreeModel::ColumnRecord {
            public:
            ImageModelColumns() {
                add(m_inputName);
                add(m_outputName);
                add(m_autoCrop);
                add(m_fullPath);
            }

            Gtk::TreeModelColumn<Glib::ustring> m_inputName;
            Gtk::TreeModelColumn<Glib::ustring> m_outputName;
            Gtk::TreeModelColumn<bool> m_autoCrop;
            Gtk::TreeModelColumn<Glib::ustring> m_fullPath;
        };

        ImageLoader m_imageLoader;
        std::vector<Image> m_imageFiles;
        std::filesystem::path m_exportDirectory;

        Gtk::MessageDialog* m_exportErrorDialog;

        Gtk::FileChooserDialog* m_fileChooser;
        sigc::connection m_fileChooserSignal;

        Gtk::Button* m_imageDirButton;
        Gtk::Button* m_exportDirButton;
        Gtk::Button* m_exportButton;

        Gtk::TreeView* m_fileTreeView;
        Glib::RefPtr<Gtk::ListStore> m_fileListStore;
        ImageModelColumns m_fileColumns;
        Gtk::TreeView::Column m_outputFileColumn;

        ui::CropPreview* m_previewPane;
        std::map<std::filesystem::path, cv::Rect> m_cropRects;
    };
}