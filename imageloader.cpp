#include <filesystem>
#include <iostream>
#include <set>

#include <imageloader.hpp>

static const std::set<std::string> ALLOWED_EXT {
    ".jpg",
    ".jpeg",
    ".png",
    ".gif"
};

Image::Image() { }

Image::Image(const Image& other) {
    filename = other.filename;
    if (other.image != nullptr) {
        image = std::make_unique<CVImage>(other.image->clone());
    } else {
        image = nullptr;
    }
}

void Image::load() {
    image = std::make_unique<CVImage>(cv::imread(filename));
}

Image ImageLoader::get_single(const std::string& filename) {
    Image img;
    img.filename = filename;
    img.image = nullptr;

    return img;
}

// TODO Recursive searching
std::vector<Image> ImageLoader::get_many(const std::string& directory) {
    std::vector<Image> images;
    std::filesystem::path path(directory);

    for (auto& entry : std::filesystem::directory_iterator{path}) {
        // If this entry is one of our allowed image files, then add it to the return vector
        std::string extension = entry.path().extension();
        if (entry.is_regular_file() && ALLOWED_EXT.find(extension) != ALLOWED_EXT.end()) {
            Image img = get_single(entry.path());
            images.push_back(img);
        }
    }

    return images;
}
