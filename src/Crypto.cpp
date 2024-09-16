#include "Crypto.h"
#include <random>
#include <iostream>

//Must be called first
void Crypto::setKey(const std::string& inKey){
    reset();
    if(inKey.size() == 0){
        std::cerr << "MUST PROVIDE INPUT KEY" << std::endl;
        m_keyIsSet = false;
        return;
    }

    m_key = padKey(stringToBytes(inKey));
    m_keyIsSet = true;
}

//************************************************************************************************************//

std::vector<uint8_t> Crypto::encryptData(const std::vector<uint8_t>& data){
    if(m_key.empty() && !isKeySet()){
        std::cerr << "KEY MUST BE SET BEFORE ENCRYPTION" << std::endl;
        return std::vector<uint8_t>();
    }

    if(data.size() == 0 || data.empty()){
        std::cerr << "MUST PROVIDE DATA TO ENCRYPT" << std::endl;
        return std::vector<uint8_t>();
    }

    generateIV();
    initCtx();

    std::vector<uint8_t> paddedData = padData(data);
    std::vector<uint8_t> encryptedData(paddedData.size() + AES_BLOCKLEN); //add room for iv 
    
    std::copy(m_iv.begin(), m_iv.end(), encryptedData.begin());

    AES_CBC_encrypt_buffer(&m_ctx, paddedData.data(), paddedData.size());

    std::copy(paddedData.begin(), paddedData.end(), encryptedData.begin() + AES_BLOCKLEN);

    reset();
    return encryptedData;
}

//************************************************************************************************************//
       
std::vector<uint8_t> Crypto::decryptData(const std::vector<uint8_t>& data){
    if(m_key.empty() && !isKeySet()){
        std::cerr << "KEY MUST BE SET BEFORE DECRYPTION" << std::endl;
        return std::vector<uint8_t>();
    }
    if(data.size() < AES_BLOCKLEN){
        std::cerr << "ENCRYPTED DATA IS TOO SHORT" << std::endl;
        return std::vector<uint8_t>();
    }

    std::vector<uint8_t> iv(data.begin(), data.begin() + AES_BLOCKLEN);
    m_iv = iv;
    initCtx();

    std::vector<uint8_t> decryptedData(data.begin() + AES_BLOCKLEN, data.end());
    AES_CBC_decrypt_buffer(&m_ctx, decryptedData.data(), decryptedData.size());

    std::vector<uint8_t> unpaddedData = unpad(decryptedData);
    if(!isDecryptionValid(unpaddedData)){
        return std::vector<uint8_t>();
    }

    reset();
    return unpaddedData;
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
    if(data.empty()){
        std::cerr << "EMPTY DATA" << std::endl;
        return std::vector<uint8_t>();
    }

    size_t paddingAmount = data.back();
    if(paddingAmount == 0 || paddingAmount > AES_BLOCKLEN){
        std::cerr << "DECRYPTION FAILED" << std::endl;
        return std::vector<uint8_t>();
    }

    if(data.size() < paddingAmount){
        std::cerr << "DATA TOO SHORT FOR PADDING" << std::endl;
        return std::vector<uint8_t>();
    }
    reset();
    return std::vector<uint8_t>(data.begin(), data.end() - paddingAmount);
}

//************************************************************************************************************//

void Crypto::reset(){
    std::fill(m_key.begin(), m_key.end(), 0);
    std::fill(m_iv.begin(), m_iv.end(), 0);
    m_key.clear();
    m_iv.clear();
}

//************************************************************************************************************//

bool Crypto::isKeySet(){
    return m_keyIsSet;
}

//************************************************************************************************************//

bool Crypto::isDecryptionValid(const std::vector<uint8_t>& decryptedData){
    if(decryptedData.empty()){
        return false;
    }
    return true;
}