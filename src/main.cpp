#include <iostream>
#include "Steganography.h"
#include "Crypto.h"

int main() {
    std::cout << "Hello, SteganoPass!" << std::endl;
    
    //TESTING
    Steganography steg; 
    Image img = steg.loadAndConvert("imagesInput/bear.png");
    //steg.maxDataSize();
    //steg.hideData();

    std::string password = "password123$";
    std::vector<uint8_t> data(password.begin(), password.end());
    steg.hideData(img, "imagesOutput/bear.png", data);
    steg.extractData("imagesOutput/bear.png");

    steg.cleanImage(img);
    return 0;
} 