#pragma once

#include <cmath>

namespace util {
    template<typename T>
    struct Point {
            T x;
            T y;

            inline double distance_to(const Point& other) {
                return sqrt(pow((x - other.x), 2) + pow(y - other.y, 2));
            }

            inline void scale(double s) {
                x *= s;
                y *= s;
            }
    };
}
