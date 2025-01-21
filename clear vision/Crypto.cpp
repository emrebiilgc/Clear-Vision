#include "Crypto.h"
#include <stdexcept>
#include <bitset>
#include <vector>

// Extract the least significant bits (LSBs) from SecretImage, calculating x, y based on message length
std::vector<int> Crypto::extract_LSBits(SecretImage& secret_image, int message_length) {

    std::vector<int> LSB_array;

    // 1. Reconstruct the SecretImage to a GrayscaleImage.
    // 2. Calculate the image dimensions.

    GrayscaleImage image = secret_image.reconstruct();
    int width = image.get_width();
    int height = image.get_height();

    // 3. Determine the total bits required based on message length.

    int total_bits = message_length * 7;

    // 4. Ensure the image has enough pixels; if not, throw an error.

    int total_pixels = width * height;
    if (total_pixels < total_bits) {
        throw std::runtime_error("Image does not have enough pixels to extract the message.");
    }

    // 5. Calculate the starting pixel from the message_length knowing that
    //    the last LSB to extract is in the last pixel of the image.

    int bit_index = 0;
    for (int i = height - 1; i >= 0 && bit_index < total_bits; --i) {
        for (int j = width - 1; j >= 0 && bit_index < total_bits; --j) {
            int pixel_value = image.get_pixel(i, j);
            int lsb = pixel_value & 1; // En az anlamlı bit
            LSB_array.push_back(lsb);
            bit_index++;
        }
    }

    // 6. Extract LSBs from the image pixels and return the result.
    std::reverse(LSB_array.begin(), LSB_array.end());

    return LSB_array;
}


// Decrypt message by converting LSB array into ASCII characters
std::string Crypto::decrypt_message(const std::vector<int>& LSB_array) {

    std::string message;
    int bit_size = LSB_array.size();

    // 1. Verify that the LSB array size is a multiple of 7, else throw an error.
    if (bit_size % 7 != 0) {
        throw std::runtime_error("Invalid LSB array size, must be multiple of 7.");
    }

    // 2. Convert each group of 7 bits into an ASCII character.
    // 3. Collect the characters to form the decrypted message.
    for (size_t i = 0; i < bit_size; i += 7) {
        int ascii_value = 0;
        for (int j = 0; j < 7; ++j) {
            ascii_value = (ascii_value << 1) | LSB_array[i + j];
        }
        message += static_cast<char>(ascii_value);
    }

    // 4. Return the resulting message.

    return message;
}

// Encrypt message by converting ASCII characters into LSBs
std::vector<int> Crypto::encrypt_message(const std::string& message) {

    std::vector<int> LSB_array;

    // 1. Convert each character of the message into a 7-bit binary representation.
    //    You can use std::bitset.

    for (char c : message) {

        // 2. Collect the bits into the LSB array.
        std::bitset<7> char_bits(c);

        for (int i = 6; i >= 0; --i) {
            LSB_array.push_back(char_bits[i]);
        }
    }

    // 3. Return the array of bits.
    return LSB_array;
}
// Embed LSB array into GrayscaleImage starting from the last bit of the image
SecretImage Crypto::embed_LSBits(GrayscaleImage& image, const std::vector<int>& LSB_array) {

    // 1. Get image dimensions
    int width = image.get_width();
    int height = image.get_height();
    int total_bits = LSB_array.size();
    int total_pixels = width * height;

    // 1. Ensure the image has enough pixels to store the LSB array, else throw an error.
    if (total_pixels < total_bits) {
        throw std::runtime_error("Image does not have enough pixels to embed the message.");
    }

    // 2. Find the starting pixel based on the message length knowing that
    //    the last LSB to embed should end up in the last pixel of the image.
    int bit_index = 0;
    int start_pixel = total_pixels - total_bits;  // Position where the first bit will be embedded

    // 3. Iterate over the image pixels, embedding LSBs from the array.
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int pixel_position = i * width + j;

            if (pixel_position >= start_pixel && bit_index < total_bits) {
                int pixel_value = image.get_pixel(i, j);

                int current_lsb = pixel_value & 1;
                if (current_lsb != LSB_array[bit_index]) {
                    if (LSB_array[bit_index] == 1) {
                        pixel_value |= 1;
                    } else {
                        pixel_value &= ~1;
                    }
                }

                image.set_pixel(i, j, pixel_value);
                bit_index++;
            }
        }
    }

    // 4. Return a SecretImage object constructed from the given GrayscaleImage
    //    with the embedded message.
    SecretImage secret_image(image);
    return secret_image;
}

