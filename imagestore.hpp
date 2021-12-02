#pragma once

#include <string>
#include <vector>
#include <set>

#include <image.hpp>

namespace img {
    class ImageStore {
        public:
            ImageStore();
            void clear();
            void populate(std::string directory);
            void populate(std::string directory, std::set<std::string> extension_filter);

            std::set<std::string> default_filter();
            const std::vector<std::string>& images() const;

        private:
            std::vector<std::string> m_images;
    };
}
