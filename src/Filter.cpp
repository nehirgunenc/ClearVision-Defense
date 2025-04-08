#include "Filter.h"
#include <algorithm>
#include <cmath>
#include <vector>
#include <numeric>
#include <math.h>


// Mean Filter
void Filter::apply_mean_filter(GrayscaleImage& image, int kernelSize) {
    GrayscaleImage original = image;

    int halfKernelSize = kernelSize / 2; // Half of the kernel size
    int height = image.get_height();  // Image height
    int width = image.get_width();    // Image width

    // Calculate the average of the neighboring pixels for each pixel
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int sum = 0;
            int count = 0;

            // Sum the pixels within the kernel area
            for (int k = -halfKernelSize; k <= halfKernelSize; ++k) {
                for (int l = -halfKernelSize; l <= halfKernelSize; ++l) {
                    int newRow = i + k;
                    int newCol = j + l;

                    
                    if (newRow >= 0 && newRow < height && newCol >= 0 && newCol < width) {
                        sum += original.get_pixel(newRow, newCol);  
                        count++;  // Increase valid pixel count
                    } else {
                        sum += 0;
                        count++;
                    }
                }
            }

            // Calculate the mean and update
            int meanValue;
            if (count > 0) {
                meanValue = int(sum / count);
            } else {
                meanValue = original.get_pixel(i, j);
            }
            image.set_pixel(i, j, meanValue);  // Place the updated value
        }
    }
}

// Gaussian Smoothing Filter
void Filter::apply_gaussian_smoothing(GrayscaleImage& image, int kernelSize, double sigma) {
 // Create the Gaussian kernel
    std::vector<std::vector<double>> kernel(kernelSize, std::vector<double>(kernelSize));
    double sum = 0.0;

    int halfKernelSize = kernelSize / 2;

    
    for (int x = -halfKernelSize; x <= halfKernelSize; ++x) {
        for (int y = -halfKernelSize; y <= halfKernelSize; ++y) {
            double value = (1.0 / (2.0 * M_PI * sigma * sigma)) * exp(-(x * x + y * y) / (2.0 * sigma * sigma));
            kernel[x + halfKernelSize][y + halfKernelSize] = value;
            sum += value;  // Sum the kernel values
        }
    }

    // Normalize the kernel
    for (int i = 0; i < kernelSize; ++i) {
        for (int j = 0; j < kernelSize; ++j) {
            kernel[i][j] /= sum;  // Normalize to make the total sum 1
        }
    }

    GrayscaleImage original = image;  // Keep the original image

    int imageHeight = image.get_height();
    int imageWidth = image.get_width();

    // Calculate the weighted sum for each pixel
    for (int i = 0; i < imageHeight; ++i) {
        for (int j = 0; j < imageWidth; ++j) {
            double sum = 0.0;

            // Apply the kernel to the pixels around the current pixel
            for (int k = -halfKernelSize; k <= halfKernelSize; ++k) {
                for (int l = -halfKernelSize; l <= halfKernelSize; ++l) {
                    int newRow = i + k;
                    int newCol = j + l;

                    // Check boundaries of neighboring pixels
                    if (newRow >= 0 && newRow < imageHeight && newCol >= 0 && newCol < imageWidth) {
                        sum += original.get_pixel(newRow, newCol) * kernel[k + halfKernelSize][l + halfKernelSize];
                    }
                }
            }

            // Place the updated value
            int newValue = static_cast<int>(sum);
            // Clamping is applied
            newValue = std::max(0, std::min(newValue, 255));
            image.set_pixel(i, j, newValue);
        }
    }
}

// Unsharp Masking Filter
void Filter::apply_unsharp_mask(GrayscaleImage& image, int kernelSize, double amount) {
    // Blur the image using Gaussian smoothing with sigma = 1.0
    GrayscaleImage blurredImage = image;  
    apply_gaussian_smoothing(blurredImage, kernelSize, 1.0);  

    // For each pixel, apply the unsharp mask formula: original + amount * (original - blurred).
    int image_height = image.get_height();
    int image_width = image.get_width();

    for (int i = 0; i < image_height; ++i) {
        for (int j = 0; j < image_width; ++j) {
            double originalValue = image.get_pixel(i, j);          // Original pixel value
            double blurredValue = blurredImage.get_pixel(i, j);    // Blurred pixel value

            // Calculate the difference and apply the sharpening amount
            double edgeValue = originalValue - blurredValue;

            // Increase the amount value
            double sharpenedValue = originalValue + static_cast<double>(amount * edgeValue);
            int sharpened_value = int(sharpenedValue);
            
            // Clamping
            sharpened_value = std::max(0, std::min(sharpened_value, 255)); // Clamp between 0-255
            
            image.set_pixel(i, j, sharpened_value);  // Place the updated value
        }
    }
}
