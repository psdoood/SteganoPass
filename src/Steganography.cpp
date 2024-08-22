#include "Steganography.h"
#include <iostream>
#include <cstring>

bool Steganography::hideData(const std::string& inPath, const std::string& outPath, const std::vector<uint8_t>& data){
    //TODO
    return false;
}
     
std::vector<uint8_t> Steganography::extractData(const std::string& inPath){
    std::vector<uint8_t> result;
    //TODO
    return result;
}

bool Steganography::canHideData(const std::string& inPath, size_t dataSize){
    size_t maxSize = maxDataSize(inPath);
    return dataSize <= maxSize;
       
}
   
size_t Steganography::maxDataSize(const std::string& inPath){
    int width, height, channels;
    stbi_info(inPath.c_str(), &width, &height, &channels);

    //each pixel can store 3 bits of data, but only least signif. bit (lsb), so / 8
    size_t maxSize = (3 * height * width) / 8;
    //add room for metadata (size of total hidden data)
    maxSize -= sizeof(size_t);
    
    return maxSize;
}
  

Image Steganography::convertToPNG(const std::string& inPath){
    int width, height, channels;
    unsigned char* data = stbi_load(inPath.c_str(), &width, &height, &channels, 4);

    if(data == nullptr){
        std::cerr << "FAILED TO LOAD IMAGE" << std::endl;
    }

    Image img;
    img.data = data;
    img.height = height;
    img.width = width;
    img.channels = channels;

    //originalFormat = GetFileExtension(inPath.c_str());

    stbi_write_png("imagesOutput/output.png", img.width, img.height, 4, img.data, img.width * 4) != 0; //TESTING

    return img;
}

void Steganography::cleanImage(Image& img){
    if(img.data != nullptr){
        stbi_image_free(img.data);
        img.data = nullptr;
        img.width = 0;
        img.height = 0;
        img.channels = 0;
    }
}
        
std::string Steganography::getOriginalFormat(){
    return originalFormat;
}