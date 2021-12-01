#ifndef CROP_H_
#define CROP_H_

#define MIN_RELATIVE_BOX_SIZE 0.03

#include <imageloader.hpp>
#include <filter/filterchain.hpp>

class Cropper {
public:
    Cropper();

    cv::Rect auto_crop(CVImage& image,
                      int margin = 32,
                      bool remove_whiteboard = true,
                      int kernel_size = 11);

    private:
    void auto_canny(CVImage& image, CVImage& edges);
    cv::Rect enclose_bounding_boxes(std::vector<cv::Rect> boxes);

    private:
        filter::FilterChain m_filterchain;
};

#endif // CROP_H_
