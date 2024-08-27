#include <gtest/gtest.h>
#include "Crypto.h"

class CryptoTest : public ::testing::Test{
protected:
    Crypto crypto;

    void SetUp() override {}
};

TEST_F(CryptoTest, EncryptionDecryption){
    crypto.setKey("TestMasterKey123");
    std::string originalData = "TestSecretData123";
    std::vector<uint8_t> data(originalData.begin(), originalData.end());

    std::vector<uint8_t> encryptedData = crypto.encryptData(data);
    ASSERT_FALSE(encryptedData.empty());

    crypto.setKey("TestMasterKey123");
    std::vector<uint8_t> decryptedData = crypto.decryptData(encryptedData);
    ASSERT_FALSE(decryptedData.empty());

    std::string decryptedString(decryptedData.begin(), decryptedData.end());
    EXPECT_EQ(originalData, decryptedString);
}

TEST_F(CryptoTest, WrongKeyDecryption){
    crypto.setKey("TestMasterKey123");
    std::string originalData = "TestSecretData123";
    std::vector<uint8_t> data(originalData.begin(), originalData.end());

    std::vector<uint8_t> encryptedData = crypto.encryptData(data);
    ASSERT_FALSE(encryptedData.empty());

    crypto.setKey("WrongKey123456");
    std::vector<uint8_t> decryptedData = crypto.decryptData(encryptedData);
    EXPECT_TRUE(decryptedData.empty());
}

TEST_F(CryptoTest, emptyData){
    crypto.setKey("TestMasterKey123");
    std::string originalData = "";
    std::vector<uint8_t> data(originalData.begin(), originalData.end());

    std::vector<uint8_t> encryptedData = crypto.encryptData(data);
    EXPECT_TRUE(encryptedData.empty());
}

TEST_F(CryptoTest, emptyKey){
    crypto.setKey("");
    
    EXPECT_FALSE(crypto.isKeySet());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}