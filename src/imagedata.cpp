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

#include <imagedata.hpp>

namespace img {
    ImageData::ImageData() {
    }

    ImageData::ImageData(std::string filename)
    : m_filename(filename) {
        load(m_filename);
    }

    ImageData::~ImageData() {
        unload();
    }

    void ImageData::load(std::string filename) {
        m_filename = filename;
        m_mat = cv::imread(filename, cv::IMREAD_COLOR | cv::IMREAD_IGNORE_ORIENTATION);

        // Convert the mat to RGB, this will simplify the conversion to a pixbuf
        // (pixbufs only support RGB)
        cv::cvtColor(m_mat, m_mat, cv::COLOR_BGR2RGB);
        update_pixbuf();

        m_dimensions.first = m_mat.size().width;
        m_dimensions.second = m_mat.size().height;
    }

    void ImageData::unload() {
        m_mat = cv::Mat();
        m_pixbuf = nullptr;
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

    void ImageData::set_features(const std::vector<ImageData::Feature>& features) {
        m_features = features;
    }

    std::vector<ImageData::Feature>& ImageData::features() {
        return m_features;
    }

    void ImageData::set_candidate(Feature candidate) {
        m_candidate_box = candidate;
    }

    ImageData::Feature& ImageData::candidate() {
        return m_candidate_box;
    }

    const std::string& ImageData::filename() const {
        return m_filename;
    }

    const std::pair<int, int>& ImageData::dimensions() const {
        return m_dimensions;
    }

    void ImageData::update_pixbuf() {
        m_pixbuf = Gdk::Pixbuf::create_from_data(m_mat.data,
                                                 Gdk::Colorspace::RGB,
                                                 false,
                                                 8,
                                                 m_mat.size().width,
                                                 m_mat.size().height,
                                                 m_mat.step);
    }
}
