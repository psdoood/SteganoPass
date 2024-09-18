//Application.h - header for the general GUI related functionality

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

//Struct to hold all variables needed for the function of the GUI
struct AppState{
    Steganography steganoObj;
    Crypto cryptoObj;
    std::string inImagePath = "Input Image Shown Here";
    std::string currentPath = std::filesystem::current_path().string();
    std::string lastLoadedPath;
    GLuint inImageTexture = 0;
    char masterKeyBuffer[AES_KEYLEN] = "";
    char dataBuffer[MAX_DATA_BUFFER_SIZE] = "";
    std::string masterKey;
    std::string data;  
    std::string extractedData;
    Image loadedImg;
    std::string loadedImgFilename;
    std::vector<std::string> files;

    //Popup window warning flags
    bool noImageWarning = false;
    bool noKeyWarning = false;
    bool noDataWarning = false;
    bool saveAsWarning = false;
    bool saveOverWarning = false;
    bool alteredImageNotSaved = false;

    //Iterates through each item in currentPath, and if it is a directory or an
    //Image it is added to AppState.files
    void updateFiles(){
        files.clear();
        for(const auto& item : std::filesystem::directory_iterator(currentPath)){
            if(item.is_directory() || isImageFile(item.path().extension().string())) {
            files.push_back(item.path().filename().string());
            }
        }
        std::sort(files.begin(), files.end());
    }   

    //Returns true if extension parameter is .jpeg, .jpg, .bmp, .png, .tga, .psd
    bool isImageFile(const std::string& extension){
        std::vector<std::string> extensions = {".jpeg", ".jpg", ".bmp", ".png", ".tga", ".psd"};
        return std::find(extensions.begin(), extensions.end(), extension) != extensions.end();
    }

    //Resets masterKeyBuffer, masterKey, dataBuffer, and data for reuse.
    void cleanControlFields(){
        memset(masterKeyBuffer, 0, sizeof(masterKeyBuffer));
        masterKey.clear();
        memset(dataBuffer, 0, sizeof(dataBuffer));
        data.clear();
    }

    //Cleans the loaded Image and related variables for use in loading new image.
    void cleanInputImage(){
        steganoObj.cleanImage(loadedImg);
        if(inImageTexture != 0){
            glDeleteTextures(1, &inImageTexture);
            inImageTexture = 0;
        }
        inImagePath = "Input Image Shown Here";
        loadedImgFilename.clear();
        lastLoadedPath.clear();
    }
};

//Functions for GUI rendering, more specific descriptions in Application.cpp
namespace appUI{
    void renderUI();
    void renderFileExplorer(const ImGuiViewport* mainViewport, const ImGuiWindowFlags& windowFlags, const float& halfWidth, const float& topSectionHeight, AppState& appState);
    void renderInputImageWindow(const ImGuiViewport* mainViewport, const ImGuiWindowFlags& windowFlags, const float& halfWidth, const float& topSectionHeight, AppState& appState);
    void renderControlWindow(const ImGuiViewport* mainViewport, const ImGuiWindowFlags& windowFlags, const float& halfWidth, const float& topSectionHeight, AppState& appState);
    void renderSettingsWindow(const ImGuiViewport* mainViewport, const ImGuiWindowFlags& windowFlags, const float& halfWidth, const float& topSectionHeight, AppState& appState);
    void renderPopUps(AppState& appState);
    GLuint loadTexture(const std::string& path, AppState& appState);
}

#endif