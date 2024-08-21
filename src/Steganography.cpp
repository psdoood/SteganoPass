#include "Steganography.h"

bool Steganography::hideData(const std::string& inPath, const std::string& outPath, const std::vector<uint8_t>& data){
    
    return false;
}
     
std::vector<uint8_t> Steganography::extractData(const std::string& inPath){
    std::vector<uint8_t> result;
    return result;
}

bool Steganography::canHideData(const std::string& inPath, size_t dataSize){
    size_t maxSize = maxDataSize(inPath);
    return dataSize <= maxSize;
       
}
   
size_t Steganography::maxDataSize(const std::string& inPath){
    Image img = LoadImage(inPath.c_str());

    //each pixel can store 3 bits of data, but only least signif. bit (lsb), so / 8
    size_t maxSize = (3 * img.height * img.width) / 8;
    //add room for metadata (size of total hidden data)
    maxSize -= sizeof(size_t);
    UnloadImage(img);
    return maxSize;
}
  
Image Steganography::convertToPNG(const std::string& inPath){
    Image img = LoadImage(inPath.c_str());
    //saves the original extension to be reused later
    originalFormat = GetFileExtension(inPath.c_str());
    //converts img to png if it's format is incorrect
    if(originalFormat != ".png"){
        if(img.format != PIXELFORMAT_UNCOMPRESSED_R8G8B8A8){
            Image tempImg = ImageCopy(img);
            ImageFormat(&tempImg, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
            UnloadImage(img);
            img = tempImg;
        }
    }
    return img;
}
        
std::string Steganography::getOriginalFormat(){
    return originalFormat;
}