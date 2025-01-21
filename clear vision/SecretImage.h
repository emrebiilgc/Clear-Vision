#ifndef SECRET_IMAGE_H
#define SECRET_IMAGE_H

#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>
#include "GrayscaleImage.h"

class SecretImage {

private:
    int *upper_triangular; // Üst üçgen kısmı (diyagonal dahil) için dizi
    int *lower_triangular; // Alt üçgen kısmı (diyagonal hariç) için dizi
    int width, height;

public:
    // GrayscaleImage alır ve iki üçgen diziye böler
    SecretImage(const GrayscaleImage &image);

    // Dosyadan okunan verilere göre başlatma
    SecretImage(int widht, int height, int *upper, int *lower);

    // Destructor
    ~SecretImage();

    // Kopya yapıcı
    SecretImage(const SecretImage& other);

    // Kopya atama operatörü
    SecretImage& operator=(const SecretImage& other);

    // İki diziden görüntüyü yeniden oluşturur
    GrayscaleImage reconstruct() const;

    // Filtreleme sonrası üçgen dizilere kaydeder
    void save_back(const GrayscaleImage &image);

    // Gizli bir görüntüyü belirtilen dosyaya kaydeder
    void save_to_file(const std::string &filename);

    // Belirtilen dosyadan gizli bir görüntüyü okur
    static SecretImage load_from_file(const std::string &filename);

    // Özel değişkenler için getter ve setter fonksiyonları
    int *get_upper_triangular() const;
    int *get_lower_triangular() const;
    int get_width() const;
    int get_height() const;
};

#endif // SECRET_IMAGE_H
