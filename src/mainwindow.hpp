/*
 * Copyright (C) 2021  Alister Sanders
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

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
                        add(m_doCrop);
                        add(m_fullPath);
                        add(m_imageData);
                        add(m_cropRect);
                    }

                    Gtk::TreeModelColumn<Glib::ustring> m_outputName;
                    Gtk::TreeModelColumn<bool> m_doCrop;
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
            Gtk::Scale*  m_rejectionScale;
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
