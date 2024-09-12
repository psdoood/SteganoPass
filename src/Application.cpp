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
    static std::string outImagePath = "Select New Path";
    static GLuint inImageTexture = 0;
    static char masterKey[256];//adjust
    static char data[1024];//adjust
    static char extractedData[1024];//adjust
    static std::string currentPath = std::filesystem::current_path().string();
    static std::string lastLoadedPath;
    static std::vector<std::string> files;
    bool noKeyWarning = false;
    bool noDataWarning = false;

    
    
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
                Image loadedImg = steganoObj.loadAndConvert(inImagePath);
                aspectRatio = (float)loadedImg.width / loadedImg.height;
                imageSize.x = std::min(imageSize.x, imageSize.y * aspectRatio);
                imageSize.y = halfWidth / aspectRatio;
                steganoObj.cleanImage(loadedImg);
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

        ImGui::InputTextWithHint(" ","<Master Key>", masterKey, AES_KEYLEN, ImGuiInputTextFlags_Password);
        ImGui::InputTextWithHint("  ", "<Data to Hide>", data, IM_ARRAYSIZE(data)); //adjust buffer size at some point

        if(ImGui::Button("Hide Data in Image")){
            if(masterKey[0] == '\0'){
                noKeyWarning = true;
            } else if(data[0] == '\0'){
                noDataWarning = true;
            } else{
            Image loadedImg = steganoObj.loadAndConvert(inImagePath);
            std::string dataStr = data;
            std::vector<uint8_t> dataBits(dataStr.begin(), dataStr.end());
            steganoObj.hideData(loadedImg, dataBits);
            }
        }
        ImGui::InputTextWithHint("   ", "<Extracted Data>", extractedData, IM_ARRAYSIZE(extractedData), ImGuiInputTextFlags_ReadOnly);
        if(ImGui::Button("Extract Data from Image")){
            //call steg/crypto functions 
        }
        ImGui::End();

        //........................................................................................................//

        //Save image window section
        ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + halfWidth, main_viewport->WorkPos.y + topSectionHeight));
        ImGui::SetNextWindowSize(ImVec2(halfWidth, bottomSectionHeight));
        ImGui::Begin("Settings", nullptr, window_flags | ImGuiWindowFlags_NoScrollbar);

        if(ImGui::Button("Save to new location")){
            
        }
        ImGui::SameLine();
        ImGui::Text(outImagePath.c_str(), ImGuiInputTextFlags_ReadOnly);

        if(ImGui::Button("Save over original")){
           
        }
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNone)){
            ImGui::SetTooltip("Not Recommended.");
        }

        if(ImGui::Button("Clear Input Image")){
            inImageTexture = 0;
            inImagePath = "Input Image Shown Here";
        }

        if(ImGui::Button("Clear Control Data")){
            memset(masterKey, 0, sizeof(masterKey));
            memset(data, 0, sizeof(data));
            memset(extractedData, 0, sizeof(extractedData));
        }

        if(ImGui::Button("Clear Save Location")){
            outImagePath = "Select New Path";
        }

        ImGui::End();

        //........................................................................................................//

        //Pop up window section
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

        ImGui::End();
    }
}