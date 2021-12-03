#pragma once

#include <string>
#include <future>

#include <gdkmm/pixbuf.h>
#include <opencv2/imgproc.hpp>

namespace img {
    class Image {
        public:
            Image(std::string filename);
            cv::Mat mat();
            Glib::RefPtr<Gdk::Pixbuf> pixbuf();
            const std::string& filename() const;

        private:
            std::string m_filename;
            cv::Mat m_mat;
            Glib::RefPtr<Gdk::Pixbuf> m_pixbuf;
    };
}
