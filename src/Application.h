#ifndef APPLICATION_H
#define APPLICATION_H

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <iostream>

#include "Crypto.h"
#include "Steganography.h"

const int MAX_DATA_BUFFER_SIZE = 1024;
const float TOP_SECTION_HEIGHT_RATIO = 0.8f;
const float BOTTOM_SECTION_HEIGHT_RATIO = 0.2f;

struct AppState{
    Steganography steganoObj;
    Crypto cryptoObj;
    std::string inImagePath = "Input Image Shown Here";
    std::string outImagePath = "Select New Path in File Explorer";
    GLuint inImageTexture = 0;
    char masterKeyBuffer[AES_KEYLEN] = "";
    std::string masterKey;
    char dataBuffer[MAX_DATA_BUFFER_SIZE] = "";
    std::string data;  
    std::string extractedData;
    std::string currentPath = std::filesystem::current_path().string();
    std::string lastLoadedPath;
    Image loadedImg;
    std::string loadedImgFilename;
    std::vector<std::string> files;

    bool noImageWarning = false;
    bool noKeyWarning = false;
    bool noDataWarning = false;
    bool saveAsWarning = false;
    bool saveOverWarning = false;
    bool alteredImageNotSaved = false;

    void updateFiles(){
        files.clear();
        for(const auto& item : std::filesystem::directory_iterator(currentPath)){
            if(item.is_directory() || isImageFile(item.path().extension().string())) {
            files.push_back(item.path().filename().string());
            }
        }
        std::sort(files.begin(), files.end());
    }   

    bool isImageFile(const std::string& extension){
        std::vector<std::string> extensions = {".jpeg", ".jpg", ".bmp", ".png", ".tga", ".psd"};
        return std::find(extensions.begin(), extensions.end(), extension) != extensions.end();
    }

};


namespace appUI
{
        void renderUI();
        GLuint loadTexture(const std::string& path);
        void renderFileExplorer(const ImGuiViewport* main_viewport, const ImGuiWindowFlags& window_flags, const float& halfWidth, const float& topSectionHeight);
        void renderInputImageWindow(const ImGuiViewport* main_viewport, const ImGuiWindowFlags& window_flags, const float& halfWidth, const float& topSectionHeight);
        void renderControlWindow(const ImGuiViewport* main_viewport, const ImGuiWindowFlags& window_flags, const float& halfWidth, const float& topSectionHeight);
        void renderSettingsWindow(const ImGuiViewport* main_viewport, const ImGuiWindowFlags& window_flags, const float& halfWidth, const float& topSectionHeight);
        void renderPopUps();
}

#endif