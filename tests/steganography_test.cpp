#include <gtest/gtest.h>
#include "Steganography.h"

class SteganographyTest : public ::testing::Test{
protected:
    Steganography steg;

    void SetUp() override {}
};

TEST_F(SteganographyTest, HideData){
   EXPECT_TRUE(true);
}

