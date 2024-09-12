#include "Steganography.h"
#include <iostream>
#include <cstring>

bool Steganography::hideData(Image& img, const std::vector<uint8_t>& data){
    if(!canHideData(img, data.size())){
        std::cerr << "CANNOT HIDE DATA IN IMAGE" << std::endl;
        return false;
    }

    size_t dataIndex = 0; size_t bitIndex = 0;
    size_t totalPixels = img.height * img.width;
    uint8_t dataSize = static_cast<uint8_t>(data.size());
    //Hide length of data first, so that it can be extracted later
    for(size_t i = 0; i < 8; i++){
        bool sizeBit = (dataSize >> i) & 1;
        img.data[i] = (img.data[i] & 0xFE) | sizeBit;
    }
    //Hide the actual bits that are stored in data 
    for(size_t i = 8; i < totalPixels * 4; i++){
        if(dataIndex >= data.size()){
            break; //All data has been hidden
        }
        bool dataBit = (data[dataIndex] >> (7 - bitIndex) & 1);
        img.data[i] = (img.data[i] & 0xFE) | dataBit;
        bitIndex++;
        if(bitIndex >= 8){
            bitIndex = 0;
            dataIndex++;
        }
    }
    return true;
}

//************************************************************************************************************//
     
std::vector<uint8_t> Steganography::extractData(const std::string& inPath){
    Image img = loadAndConvert(inPath);
    //Determine the size of data, which is stored in the first byte
    uint8_t dataSize = 0;
    for(size_t i = 0; i < 8; i++){
        bool sizeBit = img.data[i] & 1;  
        dataSize |= (sizeBit << i);
    }

    std::vector<uint8_t> result(dataSize, 0);
    size_t dataIndex = 0; size_t bitIndex = 0;

    for(size_t i = 8; i < (dataSize * 8) + 8; i++){
        bool dataBit = img.data[i] & 1;
        result[dataIndex] |= (dataBit << (7 - bitIndex));;
        bitIndex++;

        if(bitIndex == 8){
            dataIndex++;
            bitIndex = 0;
        }
    }

    cleanImage(img);
    return result;
}

//************************************************************************************************************//

bool Steganography::canHideData(const Image& img, size_t dataSize){
    size_t maxSize = maxDataSize(img);
    return dataSize <= maxSize;
}

//************************************************************************************************************//
   
size_t Steganography::maxDataSize(const Image& img){
    //each pixel can store 4 bits of data (RGBA), but only least signif. bit (lsb), so / 8
    size_t maxSize = (4 * img.height * img.width) / 8;
    //add room for metadata (size of total hidden data)
    maxSize -= 1;
    
    return maxSize;
}
  
//************************************************************************************************************//

Image Steganography::loadAndConvert(const std::string& inPath){
    int width, height, channels;
    unsigned char* data = stbi_load(inPath.c_str(), &width, &height, &channels, 4);

    if(data == nullptr){
        std::cerr << "FAILED TO LOAD IMAGE" << std::endl;
        return Image();
    }

    Image img;
    img.data = data;
    img.height = height;
    img.width = width;
    img.channels = 4;

    //TODO -> save original format to originalFormat

    return img;
}

//************************************************************************************************************//

//TODO -> error checking 
bool Steganography::saveImage(const Image& img, const std::string& outPath){
    if(stbi_write_png(outPath.c_str(), img.width, img.height, 4, img.data, img.width * 4) == 0) {
        std::cerr << "FAILED TO SAVE THE IMAGE" << std::endl;
        return false;
    }
    return true;
}

//************************************************************************************************************//

void Steganography::cleanImage(Image& img){
    if(img.data != nullptr){
        stbi_image_free(img.data);
        img.data = nullptr;
        img.width = 0;
        img.height = 0;
        img.channels = 0;
    }
}

//************************************************************************************************************//
        
std::string Steganography::getOriginalFormat(){
    return originalFormat;
}

//************************************************************************************************************//

std::string Steganography::convertToStr(const std::vector<uint8_t>& data){
    return std::string(data.begin(), data.end());
}