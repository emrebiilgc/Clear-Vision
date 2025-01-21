#include "SecretImage.h"
#include <fstream>
#include <iostream>
#include <algorithm>

// Constructor: split image into upper and lower triangular arrays
SecretImage::SecretImage(const GrayscaleImage& image) {

    width = image.get_width();
    height = image.get_height();

    // Calculate the number of elements in the upper and lower triangular matrices

    int num_upper_elements = (width * (width + 1)) / 2;
    int num_lower_elements = (width * (width - 1)) / 2;

    // 1. Dynamically allocate the memory for the upper and lower triangular matrices.
    upper_triangular = new int[num_upper_elements];
    lower_triangular = new int[num_lower_elements];

    // 2. Fill both matrices with the pixels from the GrayscaleImage.
    int upper_position = 0, lower_position = 0;
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (i <= j) {
                upper_triangular[upper_position++] = image.get_pixel(i, j);
            } else {
                lower_triangular[lower_position++] = image.get_pixel(i, j);
            }
        }
    }
}

// Constructor: instantiate based on data read from file
SecretImage::SecretImage(int widht, int height, int* upper, int* lower)
    : width(widht), height(height), upper_triangular(upper), lower_triangular(lower) {

    // Since file reading part should dynamically allocate upper and lower matrices.

    int num_upper_elements = (width * (width + 1)) / 2;
    int num_lower_elements = (width * (width - 1)) / 2;

    // You should simply copy the parameters to instance variables.

    std::copy(upper, upper + num_upper_elements, upper_triangular);
    std::copy(lower, lower + num_lower_elements, lower_triangular);
}

// Destructor: free the arrays
SecretImage::~SecretImage() {

    // Simply free the dynamically allocated memory
    // for the upper and lower triangular matrices.
    delete[] upper_triangular;
    delete[] lower_triangular;
}

// Copy constructor
SecretImage::SecretImage(const SecretImage& other) {
    width = other.width;
    height = other.height;

    int num_upper_elements = (width * (width + 1)) / 2;
    int num_lower_elements = (width * (width - 1)) / 2;

    upper_triangular = new int[num_upper_elements];  // Allocate memory
    lower_triangular = new int[num_lower_elements];  // Allocate memory

    std::copy(other.upper_triangular, other.upper_triangular + num_upper_elements, upper_triangular);
    std::copy(other.lower_triangular, other.lower_triangular + num_lower_elements, lower_triangular);
}

// Copy assignment operator
SecretImage& SecretImage::operator=(const SecretImage& other) {
    if (this == &other) {
        return *this;
    }

    // Free existing memory
    delete[] upper_triangular;
    delete[] lower_triangular;

    width = other.width;
    height = other.height;

    int num_upper_elements = (width * (width + 1)) / 2;
    int num_lower_elements = (width * (width - 1)) / 2;

    // Allocate new memory and copy elements
    upper_triangular = new int[num_upper_elements];
    lower_triangular = new int[num_lower_elements];

    std::copy(other.upper_triangular, other.upper_triangular + num_upper_elements, upper_triangular);
    std::copy(other.lower_triangular, other.lower_triangular + num_lower_elements, lower_triangular);

    return *this;
}

// Reconstructs and returns the full image from upper and lower triangular matrices.
GrayscaleImage SecretImage::reconstruct() const {
    GrayscaleImage image(width, height);


    int upper_position = 0, lower_position = 0;
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (i <= j) {
                image.set_pixel(i, j, upper_triangular[upper_position++]);
            } else {
                image.set_pixel(i, j, lower_triangular[lower_position++]);
            }
        }
    }

    return image;
}

// Save back the filtered image to triangular arrays
void SecretImage::save_back(const GrayscaleImage& image) {

    // Update the lower and upper triangular matrices
    // based on the GrayscaleImage given as the parameter.

    int upper_position = 0, lower_position = 0;
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (i <= j) {
                upper_triangular[upper_position++] = image.get_pixel(i, j);
            } else {
                lower_triangular[lower_position++] = image.get_pixel(i, j);
            }
        }
    }
}

// Save the upper and lower triangular arrays to a file
void SecretImage::save_to_file(const std::string& filename) {

    std::ofstream outfile(filename);

    if (!outfile.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    // 1. Write width and height on the first line, separated by a single space.

    outfile << width << " " << height << "\n";

    // 2. Write the upper_triangular array to the second line.
    // Ensure that the elements are space-separated.
    // If there are 15 elements, write them as: "element1 element2 ... element15"

    int num_upper_elements = (width * (width + 1)) / 2;
    for (int i = 0; i < num_upper_elements; ++i) {
        outfile << upper_triangular[i] << " ";
    }
    outfile << "\n";

    // 3. Write the lower_triangular array to the third line in a similar manner
    // as the second line.

    int num_lower_elements = (width * (width - 1)) / 2;
    for (int i = 0; i < num_lower_elements; ++i) {
        outfile << lower_triangular[i] << " ";
    }

    outfile.close();
}

// Static function to load a SecretImage from a file
SecretImage SecretImage::load_from_file(const std::string& filename) {

    // 1. Open the file and read width and height from the first line, separated by a space.
    // 2. Calculate the sizes of the upper and lower triangular arrays.
    std::ifstream infile(filename);

    if (!infile.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return SecretImage(0, 0, nullptr, nullptr);
    }

    int w, h;
    infile >> w >> h;

    int num_upper_elements = (w * (w + 1)) / 2;
    int num_lower_elements = (w * (w - 1)) / 2;

    // 3. Allocate memory for both arrays.

    int* upper = new int[num_upper_elements];
    int* lower = new int[num_lower_elements];

    // 4. Read the upper_triangular array from the second line, space-separated.

    for (int i = 0; i < num_upper_elements; ++i) {
        infile >> upper[i];
    }

    // 5. Read the lower_triangular array from the third line, space-separated.

    for (int i = 0; i < num_lower_elements; ++i) {
        infile >> lower[i];
    }

    // 6. Close the file and return a SecretImage object initialized with the
    //    width, height, and triangular arrays.

    infile.close();

    SecretImage image(w, h, upper, lower);
    return image;
}


// Returns a pointer to the upper triangular part of the secret image.
int* SecretImage::get_upper_triangular() const {
    return upper_triangular;
}

// Returns a pointer to the lower triangular part of the secret image.
int* SecretImage::get_lower_triangular() const {
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
