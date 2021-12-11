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
