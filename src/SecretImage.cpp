#include "SecretImage.h"



// Constructor: split image into upper and lower triangular arrays
SecretImage::SecretImage(const GrayscaleImage& image) {
// Get the width and height of the original image
    width = image.get_width(); 
    height = image.get_height(); 

    // Dynamically allocate memory for the upper and lower triangular matrices
    int upperSize = (width * (width + 1)) / 2; 
    int lowerSize = (width * (width - 1)) / 2; 
    upper_triangular = new int[upperSize](); // Initialize upper triangular matrix with zeros
    lower_triangular = new int[lowerSize](); // Initialize lower triangular matrix with zeros


    // Fill the matrices with pixel data
    int upperIndex = 0;
    int lowerIndex = 0;

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int pixelValue = image.get_pixel(i, j);
            if (j >= i) { 
                upper_triangular[upperIndex++] = pixelValue;
            } else { 
                lower_triangular[lowerIndex++] = pixelValue;
            }
        }
    }
}

// Constructor: instantiate based on data read from file
SecretImage::SecretImage(int w, int h, int * upper, int * lower) {

    width = w;
    height = h;

    int upperSize = (width * (width + 1)) / 2;
    int lowerSize = (width * (width - 1)) / 2;

    // Dynamically allocate memory for upper and lower matrices
    upper_triangular = new int[upperSize];
    lower_triangular = new int[lowerSize];

    // Copy the parameters to instance variables
    std::copy(upper, upper + upperSize, upper_triangular);
    std::copy(lower, lower + lowerSize, lower_triangular);
}

// Destructor: free the arrays
SecretImage::~SecretImage() {
    delete[] upper_triangular;
    delete[] lower_triangular;
}

// Reconstructs and returns the full image from upper and lower triangular matrices.
GrayscaleImage SecretImage::reconstruct() const {
    
    GrayscaleImage image(width, height);
    
    int upperIndex = 0;
    int lowerIndex = 0;


    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (j >= i) { // Upper triangular
                int value = upper_triangular[upperIndex]; 
                image.set_pixel(i, j, value);
                upperIndex++;
            } else { // Lower triangular
                int value = lower_triangular[lowerIndex];
                image.set_pixel(i, j, value);
                lowerIndex++; 
            }
        }
    }

    return image;
}



// Save the filtered image back to the triangular arrays
void SecretImage::save_back(const GrayscaleImage& image) {
    // Update the lower and upper triangular matrices based on the GrayscaleImage
    int upperIndex = 0;
    int lowerIndex = 0;

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int pixelValue = image.get_pixel(i, j);
            if (j >= i) { // Upper triangular
                upper_triangular[upperIndex++] = pixelValue;
            } else { // Lower triangular
                lower_triangular[lowerIndex++] = pixelValue;
            }
        }
    }
}

// Save the upper and lower triangular arrays to a file
void SecretImage::save_to_file(const std::string& filename) {
   std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }

    // Write width and height on the first line
    file << width << " " << height << "\n";

    // Write the upper_triangular array to the second line
    for (int i = 0; i < (width * (width + 1)) / 2; ++i) {
        file << upper_triangular[i] << (i < ((width * (width + 1)) / 2 - 1) ? " " : "");
    }
    file << "\n";

    // Write the lower_triangular array to the third line
    for (int i = 0; i < (width * (width - 1)) / 2; ++i) {
        file << lower_triangular[i] << (i < ((width * (width - 1)) / 2 - 1) ? " " : "");
    }
    file << "\n";

    file.close();
}

// Static function to load a SecretImage from a file
SecretImage SecretImage::load_from_file(const std::string& filename) {
    
    if (filename.empty()) {
        std::cerr << "Error: Filename is empty." << std::endl;
        exit(1);
    }

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file in SecretImage " << filename << std::endl;
        return SecretImage(0, 0, nullptr, nullptr);
    }


    int w, h;

    if (!(file >> w >> h)) {
        std::cerr << "Error reading width and height." << std::endl;
        return SecretImage(0, 0, nullptr, nullptr);
    }

    // Calculate the sizes of the upper and lower triangular matrices
    int upperSize = (w * (w + 1)) / 2;
    int lowerSize = (w * (w - 1)) / 2;

    // Allocate memory for both matrices
    int* upperTri = new int[upperSize];
    int* lowerTri = new int[lowerSize];

    // Read the upper triangular matrix
    for (int i = 0; i < upperSize; ++i) {
        if (!(file >> upperTri[i])) {
            std::cerr << "Error reading upper triangular matrix" << std::endl;
            delete[] upperTri;
            delete[] lowerTri;
            return SecretImage(0, 0, nullptr, nullptr); // Returns an invalid SecretImage
        }
    }

    // Read the lower triangular matrix
    for (int i = 0; i < lowerSize; ++i) {
        if (!(file >> lowerTri[i])) {
            std::cerr << "Error reading lower triangular matrix" << std::endl;
            delete[] upperTri;
            delete[] lowerTri;
            return SecretImage(0, 0, nullptr, nullptr); // Returns an invalid SecretImage
        }
    }

    // Close the file and return the SecretImage
    file.close();

    return SecretImage(w, h, upperTri, lowerTri);
}

// Returns a pointer to the upper triangular part of the secret image.
int * SecretImage::get_upper_triangular() const {
    return upper_triangular;
}

// Returns a pointer to the lower triangular part of the secret image.
int * SecretImage::get_lower_triangular() const {
    return lower_triangular;
}

// Returns the width of the secret image.
int SecretImage::get_width() const {
    return width;
}

// Returns the height of the secret image.
int SecretImage::get_height() const {
    return height;
}
