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

#pragma once

#include <vector>

namespace ui {
    /**
     * Crop rectangle handle. Used for adjusting a crop rectangle.
     * Optionally drives the x- or y-axes of other crop handles
     */
    class CropHandle {
        public:
            /**
             * Default constructor.
             * Optionally set the position and radius of this handle
             */
            CropHandle(double x = 0, double y = 0, int radius = 10);

            /**
             * Copy constructor
             */
            CropHandle(const CropHandle& other);

            /**
             * Sets a handle to drive the x-axis of
             *
             * @param other Handle to drive
             */
            void set_drive_x(CropHandle* other);

            /**
             * Sets a handle to drive the y-axis of
             *
             * @param other Handle to drive
             */
            void set_drive_y(CropHandle* other);

            /**
             * Sets the position of this handle, and updates driven handles
             *
             * @param x X position
             * @param y Y position
             */
            void set_position(double x, double y);

            /**
             * Gets the x position of this handle
             *
             * @return X position
             */
            double x() const;

            /**
             * Gets the y position of this handle
             *
             * @return Y position
             */
            double y() const;

            /**
             * Gets the radius of this handle
             *
             * @return Radius
             */
            int radius() const;

        private:
            double m_x;
            double m_y;
            int m_radius;

            CropHandle* m_drive_x;
            CropHandle* m_drive_y;
    };
}
