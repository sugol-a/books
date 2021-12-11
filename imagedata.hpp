#pragma once

#include <string>
#include <vector>

#include <gdkmm/pixbuf.h>
#include <opencv2/imgcodecs.hpp>

#include <util/box.hpp>

namespace img {
    class ImageData {
        public:
            using Feature = std::pair<float, util::Box>;

            /**
             * Constructs a new empty ImageData. A file can be loaded
             * later using ImageData::load(filename).
             */
            ImageData();

            ~ImageData();

            /**
             * Constructs a new ImageData, populated with the contents of `filename`.
             *
             * @param filename Path of the image file to load
             */
            ImageData(std::string filename);

            /**
             * Loads the contents of `filename`
             *
             * @param filename Path of the image file to load
             */
            void load(std::string filename);

            /**
             * Unloads the image data from memory
             *
             */
            void unload();

            /**
             * Returns a reference to the internal cv::Mat containing
             * the image data
             *
             * @return cv::Mat containing image data
             */
            const cv::Mat& mat() const;

            /**
             * Implicit conversion to a cv::Mat type
             *
             * @return cv::Mat containing image data
             */
            operator const cv::Mat&() const;

            /**
             * Gets a reference to the internal Gdk::Pixbuf containing the
             * image data.
             *
             * @return Pixbuf containing image data
             */
            const Glib::RefPtr<Gdk::Pixbuf> pixbuf() const;

            /**
             * Implicit conversion to a Gdk::Pixbuf
             *
             * @return Pixbuf containing image data
             */
            operator const Glib::RefPtr<Gdk::Pixbuf>() const;

            /**
             * Sets the image features
             *
             * @param features Vector containing the image bounding boxes
             */
            void set_features(const std::vector<Feature>& features);

            /**
             * Returns a const reference to the image features
             *
             * @return Vector of feature bounding boxes
             */
            std::vector<Feature>& features();

            /**
             * Sets the candidate crop rectangle. `candidate` should be a pointer to a member of `m_features`
             *
             * @param candidate Pointer to the candidate crop rectangle
             */
            void set_candidate(Feature candidate);

            /**
             * Returns a const reference to the candidate crop rectangle
             *
             * @return Reference to the candidate crop rectangle
             */
            Feature& candidate();

            /**
             * Gets the image filename
             *
             * @return Image filename
             */
            const std::string& filename() const;

            /**
             * Gets the image dimensions
             *
             * @return Image dimensions
             */
            const std::pair<int, int>& dimensions() const;

        private:
            void update_pixbuf();

        private:
            cv::Mat m_mat;
            std::string m_filename;
            std::pair<int, int> m_dimensions;
            Glib::RefPtr<Gdk::Pixbuf> m_pixbuf;
            std::vector<Feature> m_features;
            Feature m_candidate_box;
    };
}
