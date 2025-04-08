#include "Crypto.h"
#include "GrayscaleImage.h"



// Extract the least significant bits (LSBs) from SecretImage, calculating x, y based on message length
std::vector<int> Crypto::extract_LSBits(SecretImage& secret_image, int message_length) {
    std::vector<int> LSB_array;
    // Reconstruct the SecretImage to a GrayscaleImage.
    GrayscaleImage image = secret_image.reconstruct();

    // Calculate the image dimensions.
    int imageWidth = secret_image.get_width();
    int imageHeight = secret_image.get_height();

    // Determine the total bits required based on message length.
    int total_bits = message_length * 7;

    // Ensure the image has enough pixels; if not, throw an error.
    if (total_bits > imageWidth * imageHeight) {
        throw std::runtime_error("Not enough pixels in the image to extract the message.");
    }

    // Calculate the starting pixel from the message_length knowing that  
    //    the last LSB to extract is in the last pixel of the image.
    int start_pixel = (imageWidth * imageHeight) - total_bits;

    // Extract LSBs from the image pixels and return the result.
    for (int i = start_pixel; i < imageWidth * imageHeight; ++i) {
        int pixelValue = image.get_pixel(i / imageWidth, i % imageWidth);
        LSB_array.push_back(pixelValue & 1); // Get the LSB
    }

    return LSB_array;
}


// Decrypt message by converting LSB array into ASCII characters
std::string Crypto::decrypt_message(const std::vector<int>& LSB_array) {
    std::string message;
    //1. Verify that the LSB array size is a multiple of 7, else throw an error.
    if (LSB_array.size() % 7 != 0) {
        throw std::runtime_error("LSB array size is not a multiple of 7.");
    }

    // 2. Convert each group of 7 bits into an ASCII character.
    for (size_t i = 0; i < LSB_array.size(); i += 7) {
        std::bitset<7> bits;
        for (int j = 0; j < 7; ++j) {
            bits[6-j] = LSB_array[i + j];
        }
        message += static_cast<char>(bits.to_ulong()); // Convert to char
    }

    // 4. Return the resulting message.
    return message;

}

// Encrypt message by converting ASCII characters into LSBs
std::vector<int> Crypto::encrypt_message(const std::string& message) {
    std::vector<int> LSB_array;
    // Convert each character of the message into a 7-bit binary representation.
    for (char c : message) {
        std::bitset<7> bits(c); // 7-bit representation
        for (int i = 0; i < 7; ++i) {
            LSB_array.push_back(bits[6-i]); // Add each bit to the array
        }
    }

    // Return the array of bits.
    return LSB_array;
}

// Embed LSB array into GrayscaleImage starting from the last bit of the image
SecretImage Crypto::embed_LSBits(GrayscaleImage& image, const std::vector<int>& LSB_array) {
    
    // Ensure the image has enough pixels to store the LSB array, else throw an error.
    int total_bits = LSB_array.size();
    int width = image.get_width();
    int height = image.get_height();

    if (total_bits > width * height) {
        throw std::runtime_error("Not enough pixels in the image to embed the message.");
    }


    // Iterate over the image pixels, embedding LSBs from the array.
    int start_pixel = (width * height) - total_bits; // Starting pixel index
    for (int i = start_pixel; i < width * height; ++i) {
        int row = i / width;
        int col = i % width;
        int pixelValue = image.get_pixel(row, col);
        
        // Replace the LSB
        pixelValue = (pixelValue & ~1) | LSB_array[i - start_pixel]; // Clear LSB and set new LSB
        image.set_pixel(row, col, pixelValue); // Update the pixel
    }

    // Construct the SecretImage using the modified GrayscaleImage

    SecretImage secret_image(image);

    
    return secret_image;
}
