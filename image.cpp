#include <image.hpp>

#include <opencv2/imgcodecs.hpp>

namespace img {
    Image::Image(std::string filename)
        : m_filename(filename) {
        m_matFuture = std::async(std::launch::async, [](std::string filename) {
            return cv::imread(filename);
        }, m_filename);
    }

    cv::Mat Image::mat() {
        // The cv::Mat is loaded asynchronously, so we need to pull it from the
        // future here
        if (m_matFuture.valid()) {
             m_mat = m_matFuture.get();
        }

        return m_mat;
    }

    Glib::RefPtr<Gdk::Pixbuf> Image::pixbuf() {
        if (!m_pixbuf) {
            // cv::Mat original = mat();
            // cv::Mat rgb;

            // // Pixbuf only supports RGB, so we need to convert OpenCV's BGR data
            // cv::cvtColor(original, rgb, cv::COLOR_BGR2RGB);

            // // Convert the mat to a pixbuf
            // m_pixbuf = Gdk::Pixbuf::create_from_data(rgb.data,
            //                                          Gdk::Colorspace::RGB,
            //                                          false,
            //                                          8,
            //                                          rgb.size().width,
            //                                          rgb.size().height,
            //                                          3);
            m_pixbuf = Gdk::Pixbuf::create_from_file(m_filename);
        }

        return m_pixbuf;
    }

    const std::string& Image::filename() const {
        return m_filename;
    }
}
