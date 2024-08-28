#include <gtest/gtest.h>
#include "Steganography.h"
#include <filesystem>
#include <iostream>
namespace fs = std::filesystem;

class SteganographyTest : public ::testing::Test{
protected:
    Steganography steg;
    const std::string testImgPath = (fs::path(__FILE__).parent_path() / "testImage" / "ele.jpg").string();
    const std::string outImgPath = (fs::path(__FILE__).parent_path() / "testImage" / "outputEle.png").string();

    void SetUp() override {}
    void TearDown() override{
        std::remove(outImgPath.c_str());
    }
};

TEST_F(SteganographyTest, HideAndExtract){
    std::string originalData = "TestSecretData123";
    std::vector<uint8_t> data(originalData.begin(), originalData.end());

    Image img = steg.loadAndConvert(testImgPath);
    steg.hideData(img, outImgPath, data);

    std::string extractedData = steg.convertToStr(steg.extractData(outImgPath));
    
    EXPECT_EQ(originalData, extractedData);
}

TEST_F(SteganographyTest, canHideData){ 
    Image img = steg.loadAndConvert(testImgPath);
    size_t maxSize = steg.maxDataSize(img);

    EXPECT_TRUE(steg.canHideData(img, maxSize));
    EXPECT_FALSE(steg.canHideData(img, maxSize + 1));
}

TEST_F(SteganographyTest, maxDataSize){ 
    Image img = steg.loadAndConvert(testImgPath);
    size_t maxSize = ((img.height * img.width * 4) / 8) - 1;

    EXPECT_EQ(maxSize, steg.maxDataSize(img));
}

TEST_F(SteganographyTest, loadAndConvert){ 
    Image img = steg.loadAndConvert(testImgPath);
    
    EXPECT_NE(img.data, nullptr);
    EXPECT_GT(img.height, 0);
    EXPECT_GT(img.width, 0);
    EXPECT_EQ(img.channels, 4);
}

TEST_F(SteganographyTest, saveImage){ 
    Image img = steg.loadAndConvert(testImgPath);
    
    EXPECT_TRUE(steg.saveImage(img, outImgPath));
}

TEST_F(SteganographyTest, cleanImage){ 
    Image img = steg.loadAndConvert(testImgPath);
    steg.cleanImage(img);

    EXPECT_EQ(img.data, nullptr);
    EXPECT_EQ(img.height, 0);
    EXPECT_EQ(img.width, 0);
    EXPECT_EQ(img.channels, 0);
}

TEST_F(SteganographyTest, hideDataTooLarge){ 
    Image img = steg.loadAndConvert(testImgPath);
    std::vector<uint8_t> largeData(steg.maxDataSize(img) + 1, '0');

    EXPECT_FALSE(steg.hideData(img, outImgPath, largeData));
}