#include "Filter.h"
#include <algorithm>
#include <cmath>
#include <vector>
#include <numeric>
#include <math.h>
#include <stdexcept>
#define _USE_MATH_DEFINES  // For M_PI
#include <cmath>
#include "Filter.h"
#include "GrayscaleImage.h"
#include <algorithm>

// Mean Filter
void Filter::apply_mean_filter(GrayscaleImage& image, int kernelSize) {

    // Ensure kernel size is odd
    if (kernelSize % 2 == 0) {
        throw std::invalid_argument("Kernel size must be odd.");
    }

    int width = image.get_width();
    int height = image.get_height();
    int halfKernel = kernelSize / 2;

    // Create a new image to store the filtered result
    GrayscaleImage filteredImage(width, height);

    // Loop over each pixel in the image
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int sum = 0;
            int count = 0;

            // Iterate over the kernel
            for (int ky = -halfKernel; ky <= halfKernel; ++ky) {
                for (int kx = -halfKernel; kx <= halfKernel; ++kx) {
                    int neighborX = x + kx;
                    int neighborY = y + ky;

                    // Check if the neighbor is within image bounds
                    if (neighborX >= 0 && neighborX < width && neighborY >= 0 && neighborY < height) {
                        sum += image.get_pixel(neighborX, neighborY);
                        count++;
                    } else {
                        // Out of bounds, treat as black pixel (0)
                        sum += 0;
                        count++;
                    }
                }
            }

            // Calculate the mean value and set the pixel in the filtered image
            int meanValue = (count > 0) ? (sum / count) : 0; // Avoid division by zero
            filteredImage.set_pixel(x, y, meanValue); // Set the pixel value in the filtered image
        }
    }

    // Replace the original image with the filtered image
    image = filteredImage;
}

// Gaussian Smoothing Filter
void Filter::apply_gaussian_smoothing(GrayscaleImage& image, int kernelSize, double sigma) {

    // Ensure the kernel size is odd
    if (kernelSize % 2 == 0) {
        kernelSize++;  // If even, increment by 1 to make it odd
    }

    int radius = kernelSize / 2;
    std::vector<std::vector<double>> kernel(kernelSize, std::vector<double>(kernelSize, 0.0));
    double sum = 0.0;

    // Create the Gaussian kernel
    for (int y = -radius; y <= radius; ++y) {
        for (int x = -radius; x <= radius; ++x) {
            double exponent = -(x * x + y * y) / (2 * sigma * sigma);
            kernel[y + radius][x + radius] = exp(exponent) / (2 * M_PI * sigma * sigma);
            sum += kernel[y + radius][x + radius];
        }
    }

    // Normalize the kernel
    for (int y = 0; y < kernelSize; ++y) {
        for (int x = 0; x < kernelSize; ++x) {
            kernel[y][x] /= sum;
        }
    }

    // Create a temporary image for the result
    GrayscaleImage result(image.get_width(), image.get_height());

    // Apply Gaussian smoothing
    for (int y = 0; y < image.get_height(); ++y) {
        for (int x = 0; x < image.get_width(); ++x) {
            double newValue = 0.0;

            // Apply the kernel to the current pixel
            for (int ky = -radius; ky <= radius; ++ky) {
                for (int kx = -radius; kx <= radius; ++kx) {
                    int pixelY = y + ky;
                    int pixelX = x + kx;

                    // Handle boundary conditions
                    int pixelValue = 0;
                    if (pixelY >= 0 && pixelY < image.get_height() && pixelX >= 0 && pixelX < image.get_width()) {
                        pixelValue = image.get_pixel(pixelY, pixelX);
                    }

                    newValue += pixelValue * kernel[ky + radius][kx + radius];
                }
            }

            // Clamp the new pixel value to [0, 255]
            newValue = std::min(std::max(newValue, 0.0), 255.0);

            result.set_pixel(y, x, static_cast<int>(newValue));
        }
    }

    // Copy the result back to the original image
    for (int y = 0; y < image.get_height(); ++y) {
        for (int x = 0; x < image.get_width(); ++x) {
            image.set_pixel(y, x, result.get_pixel(y, x));
        }
    }

}

// Unsharp Masking Filter
void Filter::apply_unsharp_mask(GrayscaleImage& image, int kernelSize, double amount) {

    // 1. Blur the image using Gaussian smoothing, use the default sigma given in the header.

    GrayscaleImage blurredImage(image);

    apply_gaussian_smoothing(blurredImage, kernelSize, 1.0);

    // 2. For each pixel, apply the unsharp mask formula: original + amount * (original - blurred).
    // 3. Clip values to ensure they are within a valid range [0-255].

    for (int y = 0; y < image.get_height(); ++y) {
        for (int x = 0; x < image.get_width(); ++x) {
            int original = image.get_pixel(y, x);
            int blurred = blurredImage.get_pixel(y, x);

            // Unsharp Masking formula: I_sharp = I_original + amount * (I_original - I_blur)
            double edge = static_cast<double>(original) - static_cast<double>(blurred);
            double newValue = static_cast<double>(original) + amount * edge;

            if (newValue < 0.0) {
                newValue = 0.0;
            } else if (newValue > 255.0) {
                newValue = 255.0;
            }

            image.set_pixel(y, x, static_cast<int>(newValue));
        }
    }
}
