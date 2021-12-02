#pragma once

#include <cmath>

namespace util {
    struct Point {
            unsigned int x;
            unsigned int y;

            inline double distance_to(const Point& other) {
                return sqrt(pow((x - other.x), 2) + pow(y - other.y, 2));
            }
    };
}
