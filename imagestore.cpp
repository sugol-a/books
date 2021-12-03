#include <filesystem>
#include <imagestore.hpp>
#include <iostream>

namespace img {
    ImageStore::ImageStore() {
    }

    void ImageStore::clear() {
        m_images.clear();
    }

    void ImageStore::populate(std::string directory) {
        return populate(directory, default_filter());
    }

    void ImageStore::populate(std::string directory, std::set<std::string> extension_filter) {
        std::filesystem::path path = directory;

        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            // Check that this entry has an allowed extension, and is actually a file
            if (extension_filter.find(entry.path().extension().string()) != extension_filter.end()
                && entry.is_regular_file()) {
                m_images.push_back(entry.path().string());
            }
        }
    }

    const std::vector<std::string>& ImageStore::images() const {
        return m_images;
    }

    std::set<std::string> ImageStore::default_filter() {
        return std::set<std::string>({
            ".png", ".jpeg", ".jpg"
        });
    }
}
