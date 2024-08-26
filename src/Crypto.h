//Crypto.h - Header file for handling of the encryption and decryption of data.

#ifndef CRYPTO_H
#define CRYPTO_H

#include <string>
#include <vector>
#include "../external/tinyaes/aes.hpp"

class Crypto {
    public:
        Crypto(){}
        ~Crypto(){}

        //Sets the encryption/decryption key 
        void setKey(const std::string& inKey);

        //Inputs data to be encrypted
        std::vector<uint8_t> encryptData(const std::vector<uint8_t>& data);

        //Inputs data to be decrypted
        std::vector<uint8_t> decryptData(const std::vector<uint8_t>& data);

        //Byte to string conversion, and vice versa
        std::string bytesToString(const std::vector<uint8_t>& bytes);
        std::vector<uint8_t> stringToBytes(const std::string& str);
    
    private:
        std::vector<uint8_t> m_key; 
        std::vector<uint8_t> m_iv;
        AES_ctx m_ctx;

        //Inits AES context with m_key and m_iv
        void initCtx();

        //Generates a random IV (Initialization vector)
        void generateIV();

        //Padding functions
        //Makes sure key/data is correct length (block size) for AES 
        std::vector<uint8_t> padKey(const std::vector<uint8_t>& key);
        std::vector<uint8_t> padData(const std::vector<uint8_t>& data);
        std::vector<uint8_t> unpad(const std::vector<uint8_t>& data);

        //Resets obj for next use
        void reset();

};

#endif 