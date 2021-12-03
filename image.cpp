#include <image.hpp>

#include <opencv2/imgcodecs.hpp>

namespace img {
    Image::Image(std::string filename)
        : m_filename(filename) {
    }

    cv::Mat Image::mat() {
        return cv::imread(m_filename);
        guchar* data = pixbuf()->get_pixels();
        return cv::Mat(m_pixbuf->get_height(), m_pixbuf->get_width(), CV_8UC3, data);
    }

    Glib::RefPtr<Gdk::Pixbuf> Image::pixbuf() {
        if (!m_pixbuf) {
            m_pixbuf = Gdk::Pixbuf::create_from_file(m_filename);
            m_pixbuf = m_pixbuf->apply_embedded_orientation();
        }

        return m_pixbuf;
    }

    const std::string& Image::filename() const {
        return m_filename;
    }
}
