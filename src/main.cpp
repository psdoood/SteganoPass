#include <iostream>
#include "Steganography.h"

int main() {
    std::cout << "Hello, SteganoPass!" << std::endl;
    
    Steganography steg; 
    Image img = steg.loadAndConvert("imagesInput/bear.png");
    //steg.maxDataSize();
    //steg.hideData();

    std::string password = "PASSWORD1234567897890";
    std::vector<uint8_t> data(password.begin(), password.end());
    steg.hideData(img, "imagesOutput/bear.png", data);

    steg.cleanImage(img);
    return 0;
} 