#include <vector>
#include <algorithm>

#include <crop.hpp>

Cropper::Cropper() {}

cv::Rect Cropper::auto_crop(CVImage& image, int margin, bool remove_whiteboard, int kernel_size) {
    float image_area = image.total();
    cv::Mat edges = m_filterchain.apply_filters(image);

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Point> hierarchy;

    cv::findContours(edges, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    std::vector<cv::Rect> bounding_boxes;
    for (auto c : contours) {
        cv::Rect box = cv::boundingRect(c);
        float area = box.width * box.height;

        if (area / image_area >= MIN_RELATIVE_BOX_SIZE) {
            bounding_boxes.push_back(box);
        }
    }

    if (bounding_boxes.size() == 0) {
        return cv::Rect(0, 0, 0, 0);
    }

    if (remove_whiteboard) {
        if (bounding_boxes.size() == 1) {
            return cv::Rect(0, 0, 0, 0);
        }

        // Sort bounding boxes by x-position
        std::sort(bounding_boxes.begin(), bounding_boxes.end(), [](cv::Rect r1, cv::Rect r2) {
            return r1.x < r2.x;
        });

        // Remove the first bounding box (the whiteboard)
        bounding_boxes.erase(bounding_boxes.begin());
    }

    // for (auto b : bounding_boxes) {
    //     cv::Point pt1(b.x, b.y);
    //     cv::Point pt2(b.x + b.width, b.y + b.height);
    //     cv::rectangle(image, pt1, pt2, cv::Scalar(0, 255, 0), 2);
    // }

    // Get the enclosing box
    cv::Rect enclosing = enclose_bounding_boxes(bounding_boxes);

    // Pad out the enclosing box with a margin
    enclosing.x -= margin; enclosing.x = std::max(enclosing.x, 0);
    enclosing.y -= margin; enclosing.y = std::max(enclosing.y, 0);
    enclosing.width += margin * 2;
    enclosing.width = std::min(enclosing.width, image.size().width - enclosing.x);
    enclosing.height += margin * 2;
    enclosing.height = std::min(enclosing.height, image.size().height - enclosing.y);

    return enclosing;
}

void Cropper::auto_canny(CVImage& image, CVImage& edges) {
    cv::Mat dummy;

    double otsu_thres = cv::threshold(image, dummy, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    double low_thres = otsu_thres * 0.5;

    cv::Canny(image, edges, low_thres, otsu_thres);
}

cv::Rect Cropper::enclose_bounding_boxes(std::vector<cv::Rect> boxes) {
    std::vector<int> x_vals;
    std::vector<int> y_vals;

    for (auto& rect : boxes) {
        x_vals.push_back(rect.x);
        x_vals.push_back(rect.x + rect.width);
        y_vals.push_back(rect.y);
        y_vals.push_back(rect.y + rect.height);
    }

    // TODO: Sort the two lists beforehand so we don't have to use
    // min/max_element
    int x1 = *std::min_element(x_vals.begin(), x_vals.end());
    int y1 = *std::min_element(y_vals.begin(), y_vals.end());
    int x2 = *std::max_element(x_vals.begin(), x_vals.end());
    int y2 = *std::max_element(y_vals.begin(), y_vals.end());

    return cv::Rect(x1, y1, x2 - x1, y2 - y1);
}
