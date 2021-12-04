#include <imagedata.hpp>

namespace img {
    ImageData::ImageData() {
    }

    ImageData::ImageData(std::string filename) {
        load(filename);
    }

    void ImageData::load(std::string filename) {
        m_mat = cv::imread(filename);

        // Convert the mat to RGB, this will simplify the conversion to a pixbuf
        // (pixbufs only support RGB)
        cv::cvtColor(m_mat, m_mat, cv::COLOR_BGR2RGB);

        m_pixbuf = Gdk::Pixbuf::create_from_data(m_mat.data,
                                                 Gdk::Colorspace::RGB,
                                                 false,
                                                 8,
                                                 m_mat.size().width,
                                                 m_mat.size().height,
                                                 m_mat.step);
    }

    const cv::Mat& ImageData::mat() const {
        return m_mat;
    }

    ImageData::operator const cv::Mat&() const {
        return mat();
    }

    const Glib::RefPtr<Gdk::Pixbuf> ImageData::pixbuf() const {
        return m_pixbuf;
    }

    ImageData::operator const Glib::RefPtr<Gdk::Pixbuf>() const {
        return pixbuf();
    }

    void ImageData::set_features(const std::vector<util::Box>& features) {
        m_features = features;
    }

    const std::vector<util::Box>& ImageData::features() const {
        return m_features;
    }
}
