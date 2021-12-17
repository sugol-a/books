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

#include <filesystem>
#include <imagestore.hpp>

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
