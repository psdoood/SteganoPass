//Steganography.h - Header file for handling the hiding and extracting of data from images.

#ifndef STEGANOGRAPHY_H
#define STEGANOGRAPHY_H

#include <string>
#include <vector>

class Steganography {
    public:
        Steganography();
        ~Steganography();
        
        //Returns the format of an image (PNG, BMP, etc..)
        std::string getFormat(const std::string& inPath);

        //Hides data in input image, and saves it to the output image.
        bool hideData(const std::string& inPath, const std::string& outPath, const std::vector<uint8_t>& data);

        //Extracts hidden data from an input image 
        std::vector<uint8_t> extractData(const std::string& inPath);

        //Checks if a given data size will fit in image
        bool canHideData(const std::string& inPath, size_t dataSize);
        size_t maxDataSize(const std::string& inPath);
    
    private:
        std::string format;

        //Format specific methods
        bool hideDataPNG(const std::string& inPath, const std::string& outPath, const std::vector<uint8_t>& data);
        bool hideDataJPG(const std::string& inPath, const std::string& outPath, const std::vector<uint8_t>& data);
        bool hideDataBMP(const std::string& inPath, const std::string& outPath, const std::vector<uint8_t>& data);
        std::vector<uint8_t> extractDataPNG(const std::string& inPath);
        std::vector<uint8_t> extractDataJPG(const std::string& inPath);
        std::vector<uint8_t> extractDataBMP(const std::string& inPath);

};

#endif 