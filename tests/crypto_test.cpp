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

TEST_F(CryptoTest, EmptyData){
    std::string originalData = "";
    std::vector<uint8_t> data(originalData.begin(), originalData.end());

    crypto.setKey("TestMasterKey123");
    std::vector<uint8_t> encryptedData = crypto.encryptData(data);
    EXPECT_TRUE(encryptedData.empty());
}

TEST_F(CryptoTest, EmptyKey){
    crypto.setKey("");
    
    EXPECT_FALSE(crypto.isKeySet());
}

TEST_F(CryptoTest, LargeDataEncryptionDecryption){
    std::string originalData(1000, 'A');
    std::vector<uint8_t> data(originalData.begin(), originalData.end());

    crypto.setKey("TestMasterKey123");
    std::vector<uint8_t> encryptedData = crypto.encryptData(data);
    ASSERT_FALSE(encryptedData.empty());

    crypto.setKey("TestMasterKey123");
    std::vector<uint8_t> decryptedData = crypto.decryptData(encryptedData);
    ASSERT_FALSE(decryptedData.empty());

    std::string decryptedString(decryptedData.begin(), decryptedData.end());
    EXPECT_EQ(originalData, decryptedString);
}

TEST_F(CryptoTest, MultipleEncryptionsWithSameKey){
    std::string data1 = "FirstData";
    std::string data2 = "SecondData";

    crypto.setKey("TestMasterKey123");
    std::vector<uint8_t> encrypted1 = crypto.encryptData(std::vector<uint8_t>(data1.begin(), data1.end()));
    crypto.setKey("TestMasterKey123");
    std::vector<uint8_t> encrypted2 = crypto.encryptData(std::vector<uint8_t>(data2.begin(), data2.end()));

    EXPECT_NE(encrypted1, encrypted2);

    crypto.setKey("TestMasterKey123");
    std::vector<uint8_t> decrypted1 = crypto.decryptData(encrypted1);
    crypto.setKey("TestMasterKey123");
    std::vector<uint8_t> decrypted2 = crypto.decryptData(encrypted2);

    EXPECT_EQ(data1, std::string(decrypted1.begin(), decrypted1.end()));
    EXPECT_EQ(data2, std::string(decrypted2.begin(), decrypted2.end()));
}

TEST_F(CryptoTest, KeyChange){
    crypto.setKey("FirstKey123456");
    std::string originalData = "TestData";
    std::vector<uint8_t> data(originalData.begin(), originalData.end());

    std::vector<uint8_t> encryptedData = crypto.encryptData(data);

    crypto.setKey("SecondKey78901");
    std::vector<uint8_t> decryptedData = crypto.decryptData(encryptedData);
    EXPECT_TRUE(decryptedData.empty());

    crypto.setKey("FirstKey123456");
    decryptedData = crypto.decryptData(encryptedData);
    EXPECT_EQ(originalData, std::string(decryptedData.begin(), decryptedData.end()));
}

TEST_F(CryptoTest, SpecialCharacters){
    std::string originalData = "!@#$%^&*()_+{}|:<>?~`-=[]\\;',./";
    std::vector<uint8_t> data(originalData.begin(), originalData.end());

    crypto.setKey("TestMasterKey123");
    std::vector<uint8_t> encryptedData = crypto.encryptData(data);
    ASSERT_FALSE(encryptedData.empty());
    
    crypto.setKey("TestMasterKey123");
    std::vector<uint8_t> decryptedData = crypto.decryptData(encryptedData);
    std::string decryptedString(decryptedData.begin(), decryptedData.end());
    EXPECT_EQ(originalData, decryptedString);
}

int main(int argc, char **argv){
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}