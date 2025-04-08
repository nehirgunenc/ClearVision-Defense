#include "GrayscaleImage.h"
#include <iostream>
#include <cstring>  // For memcpy
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <stdexcept>

// Constructor: load from a file
GrayscaleImage::GrayscaleImage(const char* filename) {
    int channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, STBI_grey);

    if (image == nullptr) {
        std::cerr << "Error: Could not load image " << filename << std::endl;
        exit(1);
    }

    // Allocate memory for the 2D pixel data matrix
    data = new int*[height];
    for (int i = 0; i < height; ++i) {
        data[i] = new int[width];
    }

    // Fill the matrix with pixel values from the image
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            data[i][j] = static_cast<int>(image[i * width + j]);
        }
    }

    // Free the stb image memory
    stbi_image_free(image);
}

// Constructor: initialize from a pre-existing data matrix
GrayscaleImage::GrayscaleImage(int** inputData, int h, int w) : width(w), height(h) {
    // Allocate memory for the 2D pixel data matrix
    data = new int*[height];
    for (int i = 0; i < height; ++i) {
        data[i] = new int[width];
        std::memcpy(data[i], inputData[i], width * sizeof(int));  // Copy each row from inputData
    }
}

// Constructor to create a blank image of given width and height
GrayscaleImage::GrayscaleImage(int w, int h) : width(w), height(h) {
    data = new int*[height];
    for (int i = 0; i < height; ++i) {
        data[i] = new int[width];
        std::memset(data[i], 0, width * sizeof(int));  // Initialize all values to 0 (black image)
    }
}

// Copy constructor
GrayscaleImage::GrayscaleImage(const GrayscaleImage& other) : width(other.width), height(other.height) {
    data = new int*[height];
    for (int i = 0; i < height; ++i) {
        data[i] = new int[width];
        std::memcpy(data[i], other.data[i], width * sizeof(int));  // Copy each row
    }

    if (data == nullptr) {
    std::cerr << "Error: Data is not initialized." << std::endl;
    exit(1);
}

}

// Destructor
GrayscaleImage::~GrayscaleImage() {
    if (data != nullptr) {
        for (int i = 0; i < height; ++i) {
            delete[] data[i];  // Free each row
        }
        delete[] data;  // Free the array of rows
        data = nullptr;  // Nullify the pointer to avoid double free
    }
}


// Equality operator
bool GrayscaleImage::operator==(const GrayscaleImage& other) const {
    if (width != other.width || height != other.height) {
        return false;  // Dimensions mismatch
    }
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (data[i][j] != other.data[i][j]) {
                return false;  // Pixel value mismatch
            }
        }
    }
    return true;  // All pixel values are the same
}

// Addition operator
GrayscaleImage GrayscaleImage::operator+(const GrayscaleImage& other) const {
    if (width != other.width || height != other.height) {
        throw std::invalid_argument("Images must have the same dimensions for addition.");
    }
    GrayscaleImage result(width, height);
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            result.data[i][j] = std::min(data[i][j] + other.data[i][j], 255);  // Clamp to 255
        }
    }
    return result;
}

// Subtraction operator
GrayscaleImage GrayscaleImage::operator-(const GrayscaleImage& other) const {
    if (width != other.width || height != other.height) {
        throw std::invalid_argument("Images must have the same dimensions for subtraction.");
    }
    GrayscaleImage result(width, height);
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            result.data[i][j] = std::max(data[i][j] - other.data[i][j], 0);  // Clamp to 0
        }
    }
    return result;
}

// Get a specific pixel value
int GrayscaleImage::get_pixel(int row, int col) const {
    if (row < 0 || row >= height || col < 0 || col >= width) {
        throw std::out_of_range("Pixel coordinates are out of range.");
    }
    return data[row][col];
}

int clamp(int value, int low, int high) {
    if (value < low) return low;
    if (value > high) return high;
    return value;
}

void GrayscaleImage::set_pixel(int row, int col, int value) {
    if (row < 0 || row >= height || col < 0 || col >= width) {
        throw std::out_of_range("Pixel coordinates are out of range.");
    }
    data[row][col] = clamp(value, 0, 255);  // Ensure value is between 0 and 255
}

// Function to save the image to a PNG file
void GrayscaleImage::save_to_file(const char* filename) const {
    if (data == nullptr) {
    std::cerr << "Error: Pixel data is not initialized." << std::endl;
    return; // Veya başka bir hata işleme
}

    unsigned char* imageBuffer = new unsigned char[width * height];

    if (imageBuffer == nullptr) {
    std::cerr << "Error: Could not allocate memory for image buffer." << std::endl;
    return;
}


    // Fill the buffer with pixel data (convert int to unsigned char)
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            imageBuffer[i * width + j] = static_cast<unsigned char>(data[i][j]);
        }
    }


    // Write the buffer to a PNG file
    if (!stbi_write_png(filename, width, height, 1, imageBuffer, width)) {
        std::cerr << "Error: Could not save image to file " << filename << std::endl;
    }

    // Clean up the allocated buffer
    delete[] imageBuffer;
}
