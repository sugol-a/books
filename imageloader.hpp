#pragma once

#include <vector>
#include <memory>
#include <filesystem>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

using CVImage = cv::Mat;

class Image {
public:
    std::filesystem::path filename;
    std::unique_ptr<CVImage> image;

    Image();
    Image(const Image& other);
    void load();
};

class ImageLoader {
public:
    Image get_single(const std::string& filename);
    std::vector<Image> get_many(const std::string& directory);
};
