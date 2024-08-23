//Steganography.h - Header file for handling the hiding and extracting of data from images.

#ifndef STEGANOGRAPHY_H
#define STEGANOGRAPHY_H

#include <string>
#include <vector>
#include "../external/stb/stb_image.h"
#include "../external/stb/stb_image_write.h"

struct Image{
    unsigned char* data;
    int width;
    int height;
    int channels;
};

class Steganography {
    public:
        Steganography(){}
        ~Steganography(){}

        //Hides data in input image, and saves it to the output image
        bool hideData(Image& img, const std::string& outPath, const std::vector<uint8_t>& data);

        //Extracts hidden data from an input image 
        std::vector<uint8_t> extractData(const std::string& inPath);

        //Checks if a given data size will fit in image
        bool canHideData(const Image& img, size_t dataSize);

        //Returns the max amount of data (in bytes) that can be hidden in the image
        size_t maxDataSize(const Image& img); 

        //Loads and converts image to PNG format
        Image loadAndConvert(const std::string& inPath);
        
        //Saves the new image to new location 
        bool saveImage(const Image& img, const std::string& outPath);

        //Cleans image data from memory
        void cleanImage(Image& img);

        //Returns original format of the input image
        std::string getOriginalFormat();
    
    private:
        //The format of the input image
        std::string originalFormat;
        Image loadedImg;
};

#endif 