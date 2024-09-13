#include "Application.h"
#include "imgui.h"
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <iostream>

#include "Crypto.h"
#include "Steganography.h"

namespace appUI
{
    static Steganography steganoObj;
    static Crypto cryptoObj;
    static std::string inImagePath = "Input Image Shown Here";
    static std::string outImagePath = "Select New Path in File Explorer";
    static GLuint inImageTexture = 0;
    static char masterKeyBuffer[AES_KEYLEN] = "";
    static std::string masterKey;
    static char dataBuffer[1024] = "";//adjust
    static std::string data;  
    static std::string extractedData;
    static std::string currentPath = std::filesystem::current_path().string();
    static std::string lastLoadedPath;
    static Image loadedImg;
    static std::string loadedImgFilename;
    static std::vector<std::string> files;
    static bool noImageWarning = false;
    static bool noKeyWarning = false;
    static bool noDataWarning = false;
    static bool saveWarning = false;
    
    //************************************************************************************************************//

    GLuint loadTexture(const std::string path){
        Image loadedImg = steganoObj.loadAndConvert(path);

        GLuint image_texture;
        glGenTextures(1, &image_texture);
        glBindTexture(GL_TEXTURE_2D, image_texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, loadedImg.width, loadedImg.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, loadedImg.data);

        steganoObj.cleanImage(loadedImg);
        return image_texture;
    }

    //************************************************************************************************************//

    void updateFiles(){
        files.clear();
        for(const auto& item : std::filesystem::directory_iterator(currentPath)){
            if(item.is_directory() || isImageFile(item.path().extension().string())) {
            files.push_back(item.path().filename().string());
            }
        }
        std::sort(files.begin(), files.end());
    }   

    //************************************************************************************************************//

    bool isImageFile(const std::string& extension){
        std::vector<std::string> extensions = {".jpeg", ".jpg", ".bmp", ".png", ".tga", ".psd"};
        return std::find(extensions.begin(), extensions.end(), extension) != extensions.end();
    }

    //************************************************************************************************************//

    void renderUI(){
        ImGuiWindowFlags window_flags = 0;
        window_flags |= ImGuiWindowFlags_NoCollapse;
        window_flags |= ImGuiWindowFlags_NoResize;
        window_flags |= ImGuiWindowFlags_NoMove;

        //Cause the UI to be fullscreen in relation to the glfw window
        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(main_viewport->WorkSize.x, main_viewport->WorkSize.y), ImGuiCond_Always);
        ImGui::Begin("SteganoPass", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);

        //Get width of the window for button and text placement 
        float windowWidth = ImGui::GetWindowWidth();
        float windowHeight = ImGui::GetWindowHeight();
        float halfWidth = windowWidth * 0.5f; 
        float topSectionHeight = windowHeight * 0.8f;
        float bottomSectionHeight = windowHeight * 0.2f;

        //........................................................................................................//

        //File explorer window section
        ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y));
        ImGui::SetNextWindowSize(ImVec2(halfWidth, topSectionHeight));
        ImGui::Begin("File Explorer", nullptr, window_flags);
            
        if(ImGui::Button("Set as New Location to Save Image")){
            outImagePath = currentPath;
        }
        ImGui::SameLine();
        ImGui::Text("Current Path: %s", currentPath.c_str());
        ImGui::Text("");
        if(ImGui::Button("<- Go Back")){
            currentPath = std::filesystem::path(currentPath).parent_path().string();
            updateFiles();
        }
        if(files.empty()){
            ImGui::Text("No images/folders in this directory");
        }else{
            for(const auto& file : files){
                if(ImGui::Selectable(file.c_str())){
                    std::string fullPath = (std::filesystem::path(currentPath) / file).string();
                    bool isDirectory = std::filesystem::is_directory(fullPath);
                    if(isDirectory){
                        currentPath = fullPath;
                        updateFiles();
                    }else{
                        inImagePath = fullPath;
                        if(inImageTexture != 0){
                            glDeleteTextures(1, &inImageTexture);
                            inImageTexture = 0;
                        }
                            inImageTexture = loadTexture(inImagePath);
                            loadedImgFilename = std::filesystem::path(inImagePath).filename().string();
                    }
                }
            }
        }
        ImGui::End();
         
        //........................................................................................................//
        
        //Input image window section
        ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + halfWidth, main_viewport->WorkPos.y));
        ImGui::SetNextWindowSize(ImVec2(halfWidth, topSectionHeight));
        ImGui::Begin("Input Image", nullptr, window_flags);

        if(inImageTexture == 0){
            ImGui::Button(inImagePath.c_str(), ImVec2(halfWidth - 20, topSectionHeight - 40));
        } else{
            float aspectRatio = 0.0f;
            ImVec2 imageSize(halfWidth - 20, halfWidth - 40);
            if(inImagePath != lastLoadedPath){
                loadedImg = steganoObj.loadAndConvert(inImagePath);
                aspectRatio = (float)loadedImg.width / loadedImg.height;
                imageSize.x = std::min(imageSize.x, imageSize.y * aspectRatio);
                imageSize.y = halfWidth / aspectRatio;
                lastLoadedPath = inImagePath;
            }
            ImGui::SetCursorPos(ImVec2((halfWidth - imageSize.x) * 0.5f, (topSectionHeight - imageSize.y) * 0.5f));
            ImGui::Image((void*)(intptr_t)inImageTexture, imageSize);
        }
        ImGui::End();

        //........................................................................................................//

        //Control window section
        ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y + topSectionHeight));
        ImGui::SetNextWindowSize(ImVec2(halfWidth, bottomSectionHeight));
        ImGui::Begin("Control", nullptr, window_flags | ImGuiWindowFlags_NoScrollbar);

        if(ImGui::InputTextWithHint(" ","<Master Key>", masterKeyBuffer, IM_ARRAYSIZE(masterKeyBuffer), ImGuiInputTextFlags_Password)){
            masterKey = std::string(masterKeyBuffer);
        }
        if(ImGui::InputTextWithHint("  ", "<Data>", dataBuffer, IM_ARRAYSIZE(dataBuffer))){
            data = std::string(dataBuffer);
        }

        if(ImGui::Button("Hide Data in Image")){
            if(inImagePath == "Input Image Shown Here"){
                noImageWarning = true;
            } else if(masterKey[0] == '\0'){
                noKeyWarning = true;
            } else if(data[0] == '\0'){
                noDataWarning = true;
            } else{
                std::cout << "Master Key: " << masterKey << std::endl; //EMPTY FOR SOME REASON
                cryptoObj.setKey(masterKey);
                std::string dataStr = data;
                std::vector<uint8_t> dataBits(dataStr.begin(), dataStr.end());
                std::vector<uint8_t> encryptedData = cryptoObj.encryptData(dataBits);
                steganoObj.hideData(loadedImg, encryptedData);
            }
        }
        ImGui::InputTextWithHint("   ", "<Extracted Data>", &extractedData[0], extractedData.size(), ImGuiInputTextFlags_ReadOnly);
        if(ImGui::Button("Extract Data from Image")){
            if(inImagePath == "Input Image Shown Here"){
                noImageWarning = true;
            } else{
                cryptoObj.setKey(masterKey);
                std::vector<uint8_t> foundData = steganoObj.extractData(inImagePath);
                std::vector<uint8_t> decryptedData = cryptoObj.decryptData(foundData);
                std::string decryptedString(decryptedData.begin(), decryptedData.end());
                
                extractedData = decryptedString;
            }
        }
        ImGui::End();

        //........................................................................................................//

        //Save image window section
        ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + halfWidth, main_viewport->WorkPos.y + topSectionHeight));
        ImGui::SetNextWindowSize(ImVec2(halfWidth, bottomSectionHeight));
        ImGui::Begin("Settings", nullptr, window_flags | ImGuiWindowFlags_NoScrollbar);

        if(ImGui::Button("Save to new location")){
            if(outImagePath == "Select New Path in File Explorer" || loadedImgFilename.empty()){
                saveWarning = true;
            }else{
                std::string fullSavePath = (std::filesystem::path(outImagePath) / loadedImgFilename).string();
                steganoObj.saveImage(loadedImg, fullSavePath);
            }
        }
        ImGui::SameLine();
        ImGui::Text(outImagePath.c_str(), ImGuiInputTextFlags_ReadOnly);

        if(ImGui::Button("Save over original")){
           //TODO: FIGURE THIS OUT
        }
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNone)){
            ImGui::SetTooltip("Not Recommended.");
        }

        if(ImGui::Button("Clear Input Image")){
            inImageTexture = 0;
            inImagePath = "Input Image Shown Here";
            loadedImgFilename.clear();
            steganoObj.cleanImage(loadedImg);
        }

        if(ImGui::Button("Clear Control Data")){
            memset(masterKeyBuffer, 0, sizeof(masterKeyBuffer));
            masterKey.clear();
            memset(dataBuffer, 0, sizeof(dataBuffer));
            data.clear();
            extractedData.clear();
        }

        if(ImGui::Button("Clear Save Location")){
            outImagePath = "Select New Path in File Explorer";
        }

        ImGui::End();

        //........................................................................................................//

        //Pop up window section
        if(noImageWarning){
            ImGui::OpenPopup("Warning: No Image Set");
        }
        if(ImGui::BeginPopupModal("Warning: No Image Set", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)){
            ImGui::Text("Choose an image in the file explorer.");
            if(ImGui::Button("Close", ImVec2(120, 0))){
                noImageWarning = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if(noKeyWarning){
            ImGui::OpenPopup("Warning: No Key Set");
        }
        if(ImGui::BeginPopupModal("Warning: No Key Set", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)){
            ImGui::Text("Enter a key in the control window before hiding data.");
            if(ImGui::Button("Close", ImVec2(120, 0))){
                noKeyWarning = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if(noDataWarning){
            ImGui::OpenPopup("Warning: No Data Entered");
        }
        if(ImGui::BeginPopupModal("Warning: No Data Entered", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)){
            ImGui::Text("Enter data in the control window before hiding data.");
            if(ImGui::Button("Close", ImVec2(120, 0))){
                noDataWarning = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if(saveWarning){
            ImGui::OpenPopup("Warning: Failed to save image");
        }
        if(ImGui::BeginPopupModal("Warning: Make sure you have selected a new location/image", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)){
            ImGui::Text("Select a new location or image.");
            if(ImGui::Button("Close", ImVec2(120, 0))){
                saveWarning = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }


        ImGui::End();
    }
}