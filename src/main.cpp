#include <iostream>
#include "Steganography.h"

int main() {
    std::cout << "Hello, SteganoPass!" << std::endl;
    
    Steganography steg; 
    steg.convertToPNG("imagesInput/lion.jpg");

    return 0;
} 