#include "Crypto.h"
#include <random>
#include <iostream>

void Crypto::setKey(const std::string& inKey){
    m_key = padKey(stringToBytes(inKey));
}

//************************************************************************************************************//

std::vector<uint8_t> Crypto::encryptData(const std::vector<uint8_t>& data){
    std::vector<uint8_t> encryptedData;
    return encryptedData;
}

//************************************************************************************************************//
       
std::vector<uint8_t> Crypto::decryptData(const std::vector<uint8_t>& data){
    std::vector<uint8_t> decryptedData;
    return decryptedData;
}

//************************************************************************************************************//

std::string Crypto::bytesToString(const std::vector<uint8_t>& bytes){
    std::string str(bytes.begin(), bytes.end());
    return str;
}

//************************************************************************************************************//

std::vector<uint8_t> Crypto::stringToBytes(const std::string& str){
    std::vector<uint8_t> bytes(str.begin(), str.end());
    return bytes; 
}

//************************************************************************************************************//

void Crypto::initCtx(){
    AES_init_ctx_iv(&m_ctx, m_key.data(), m_iv.data());
}

//************************************************************************************************************//

void Crypto::generateIV(){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    m_iv.resize(AES_BLOCKLEN);
    for(auto& byte : m_iv){
        byte = static_cast<uint8_t>(dis(gen));
    }
}

//************************************************************************************************************//

std::vector<uint8_t> Crypto::padKey(const std::vector<uint8_t>& key){
    std::vector<uint8_t> paddedKey = key;
    if(paddedKey.size() < AES_KEYLEN){
        paddedKey.resize(AES_KEYLEN, 0);
    }else{
        paddedKey.resize(AES_KEYLEN);
    }
    return paddedKey;
}

//************************************************************************************************************//

std::vector<uint8_t> Crypto::padData(const std::vector<uint8_t>& data){
    std::vector<uint8_t> paddedData = data;
    size_t paddingAmount = AES_BLOCKLEN - (data.size() % AES_BLOCKLEN);
    //If no padding needed, still need to store that information at the end, so add empty AES block
    if(paddingAmount == 0){
        paddingAmount = AES_BLOCKLEN;
    }
    paddedData.insert(paddedData.end(), paddingAmount, static_cast<uint8_t>(paddingAmount));
    return paddedData;
}

//************************************************************************************************************//

std::vector<uint8_t> Crypto::unpad(const std::vector<uint8_t>& data){
    size_t paddingAmount = data.back();
    if(paddingAmount == 0 || paddingAmount > AES_BLOCKLEN){
        std::cerr << "INVALID PADDING" << std::endl;
        return std::vector<uint8_t>();
    }
    return std::vector<uint8_t>(data.begin(), data.end() - paddingAmount);
}