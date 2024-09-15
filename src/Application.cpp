#include "Application.h"

namespace appUI
{
    static AppState appState; 

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
        float topSectionHeight = windowHeight * TOP_SECTION_HEIGHT_RATIO;
        float bottomSectionHeight = windowHeight * BOTTOM_SECTION_HEIGHT_RATIO;

        renderFileExplorer(main_viewport, window_flags, halfWidth, topSectionHeight);
        renderInputImageWindow(main_viewport, window_flags, halfWidth, topSectionHeight);
        renderControlWindow(main_viewport, window_flags, halfWidth, topSectionHeight);
        renderSettingsWindow(main_viewport, window_flags, halfWidth, topSectionHeight);
        renderPopUps();
        
        ImGui::End();
    }

    //************************************************************************************************************//

    void renderFileExplorer(const ImGuiViewport* main_viewport, const ImGuiWindowFlags& window_flags, const float& halfWidth, const float& topSectionHeight){
        ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y));
        ImGui::SetNextWindowSize(ImVec2(halfWidth, topSectionHeight));
        ImGui::Begin("File Explorer", nullptr, window_flags);
        if(ImGui::Button("Set as New Location to Save Image")){
            appState.outImagePath = appState.currentPath;
        }
        //ImGui::SameLine();
        ImGui::Text("Current Path: %s", appState.currentPath.c_str());
        ImGui::Text("");
        if(ImGui::Button("<- Go Back")){
            appState.currentPath = std::filesystem::path(appState.currentPath).parent_path().string();
            appState.updateFiles();
        }
        if(appState.files.empty()){
            ImGui::Text("No images/folders in this directory");
        }else{
            for(const auto& file : appState.files){
                if(ImGui::Selectable(file.c_str())){
                    std::string fullPath = (std::filesystem::path(appState.currentPath) / file).string();
                    bool isDirectory = std::filesystem::is_directory(fullPath);
                    if(isDirectory){
                        appState.currentPath = fullPath;
                        appState.updateFiles();
                    }else{
                        appState.inImagePath = fullPath;
                        if(appState.inImageTexture != 0){
                            glDeleteTextures(1, &appState.inImageTexture);
                            appState.inImageTexture = 0;
                        }
                        appState.inImageTexture = loadTexture(appState.inImagePath);
                        appState.loadedImgFilename = std::filesystem::path(appState.inImagePath).filename().string();
                    }
                }
                if(ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNone) && appState.alteredImageNotSaved){
                    ImGui::SetTooltip("Image hasn't been saved.");
                }
            }
        }
        ImGui::End();
    }

    //************************************************************************************************************//

    void renderInputImageWindow(const ImGuiViewport* main_viewport, const ImGuiWindowFlags& window_flags, const float& halfWidth, const float& topSectionHeight){
        ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + halfWidth, main_viewport->WorkPos.y));
        ImGui::SetNextWindowSize(ImVec2(halfWidth, topSectionHeight));
        ImGui::Begin("Input Image", nullptr, window_flags);

        if(appState.inImageTexture == 0){
            ImGui::Button(appState.inImagePath.c_str(), ImVec2(halfWidth - 20, topSectionHeight - 40));
        } else{
            float aspectRatio = 0.0f;
            ImVec2 imageSize(halfWidth - 20, halfWidth - 40);
            if(appState.inImagePath != appState.lastLoadedPath){
                appState.loadedImg = appState.steganoObj.loadAndConvert(appState.inImagePath);
                aspectRatio = (float)appState.loadedImg.width / appState.loadedImg.height;
                imageSize.x = std::min(imageSize.x, imageSize.y * aspectRatio);
                imageSize.y = halfWidth / aspectRatio;
                appState.lastLoadedPath = appState.inImagePath;
            }
            ImGui::SetCursorPos(ImVec2((halfWidth - imageSize.x) * 0.5f, (topSectionHeight - imageSize.y) * 0.5f));
            ImGui::Image((void*)(intptr_t)appState.inImageTexture, imageSize);
        }
        ImGui::End();
    }

    //************************************************************************************************************//

    void renderControlWindow(const ImGuiViewport* main_viewport, const ImGuiWindowFlags& window_flags, const float& halfWidth, const float& topSectionHeight){
        ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y + topSectionHeight));
        ImGui::SetNextWindowSize(ImVec2(halfWidth, 1.0 - topSectionHeight));
        ImGui::Begin("Control", nullptr, window_flags | ImGuiWindowFlags_NoScrollbar);

        if(ImGui::InputTextWithHint(" ","<Master Key>", appState.masterKeyBuffer, IM_ARRAYSIZE(appState.masterKeyBuffer), ImGuiInputTextFlags_Password)){
            appState.masterKey = std::string(appState.masterKeyBuffer);
        }
        if(ImGui::InputTextWithHint("  ", "<Data>", appState.dataBuffer, IM_ARRAYSIZE(appState.dataBuffer))){
            appState.data = std::string(appState.dataBuffer);
        }

        if(ImGui::Button("Hide Data in Image")){
            if(appState.inImagePath == "Input Image Shown Here"){
                appState.noImageWarning = true;
            } else if(appState.masterKey[0] == '\0'){
                appState.noKeyWarning = true;
            } else if(appState.data[0] == '\0'){
                appState.noDataWarning = true;
            } else{
                appState.cryptoObj.setKey(appState.masterKey);
                std::string dataStr = appState.data;
                std::vector<uint8_t> dataBits(dataStr.begin(), dataStr.end());
                std::vector<uint8_t> encryptedData = appState.cryptoObj.encryptData(dataBits);
                appState.steganoObj.hideData(appState.loadedImg, encryptedData);
                memset(appState.masterKeyBuffer, 0, sizeof(appState.masterKeyBuffer));
                appState.masterKey.clear();
                memset(appState.dataBuffer, 0, sizeof(appState.dataBuffer));
                appState.data.clear();
                appState.alteredImageNotSaved = true;
            }
        }
        ImGui::InputTextWithHint("   ", "<Extracted Data>", &appState.extractedData[0], appState.extractedData.size(), ImGuiInputTextFlags_ReadOnly);
        if(ImGui::Button("Extract Data from Image")){
            if(appState.inImagePath == "Input Image Shown Here"){
                appState.noImageWarning = true;
            } else if(appState.masterKey[0] == '\0'){
                appState.noKeyWarning = true;
            } else{
                appState.cryptoObj.setKey(appState.masterKey);
                std::vector<uint8_t> foundData = appState.steganoObj.extractData(appState.inImagePath);
                std::vector<uint8_t> decryptedData = appState.cryptoObj.decryptData(foundData);
                std::string decryptedString(decryptedData.begin(), decryptedData.end());
                appState.extractedData = decryptedString;
                memset(appState.masterKeyBuffer, 0, sizeof(appState.masterKeyBuffer));
                appState.masterKey.clear();
                memset(appState.dataBuffer, 0, sizeof(appState.dataBuffer));
                appState.data.clear();
            }
        }
        ImGui::End();
    }

    //************************************************************************************************************//

    void renderSettingsWindow(const ImGuiViewport* main_viewport, const ImGuiWindowFlags& window_flags, const float& halfWidth, const float& topSectionHeight){
        ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + halfWidth, main_viewport->WorkPos.y + topSectionHeight));
        ImGui::SetNextWindowSize(ImVec2(halfWidth, 1.0 - topSectionHeight));
        ImGui::Begin("Settings", nullptr, window_flags | ImGuiWindowFlags_NoScrollbar);

        if(ImGui::Button("Save to new location")){
            if(appState.outImagePath == "Select New Path in File Explorer" || appState.loadedImgFilename.empty()){
                appState.saveAsWarning = true;
            }else{
                std::string fullSavePath = (std::filesystem::path(appState.outImagePath) / appState.loadedImgFilename).string();
                appState.steganoObj.saveImage(appState.loadedImg, fullSavePath);
                appState.updateFiles();
                appState.alteredImageNotSaved = false;
            }
        }
        if(ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNone)){
            if(appState.outImagePath == "Select New Path in File Explorer"){
                ImGui::SetTooltip("Choose a Path in the File Explorer.");
            }
        }
        ImGui::SameLine();
        ImGui::Text(appState.outImagePath.c_str(), ImGuiInputTextFlags_ReadOnly);

        if(ImGui::Button("Save over original")){
            if(appState.loadedImgFilename.empty()){
                appState.saveOverWarning = true;
            }else{
                appState.steganoObj.saveImage(appState.loadedImg, appState.inImagePath);
                appState.updateFiles();
                appState.alteredImageNotSaved = false;
            }
        }
        if(ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNone)){
            ImGui::SetTooltip("Not Recommended.");
        }

        if(ImGui::Button("Clear Input Image")){
            appState.inImageTexture = 0;
            appState.inImagePath = "Input Image Shown Here";
            appState.loadedImgFilename.clear();
            appState.steganoObj.cleanImage(appState.loadedImg);
            appState.extractedData.clear();
        }

        if(ImGui::Button("Clear Control Data")){
            memset(appState.masterKeyBuffer, 0, sizeof(appState.masterKeyBuffer));
            appState.masterKey.clear();
            memset(appState.dataBuffer, 0, sizeof(appState.dataBuffer));
            appState.data.clear();
            appState.extractedData.clear();
        }

        if(ImGui::Button("Clear Save Location")){
            appState.outImagePath = "Select New Path in File Explorer";
        }

        ImGui::End();
    }

    //************************************************************************************************************//

    void renderPopUps(){
        if(appState.noImageWarning){
            ImGui::OpenPopup("Warning: No Image Set");
        }
        if(ImGui::BeginPopupModal("Warning: No Image Set", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)){
            ImGui::Text("Choose an image in the file explorer.");
            if(ImGui::Button("Close", ImVec2(120, 0))){
                appState.noImageWarning = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if(appState.noKeyWarning){
            ImGui::OpenPopup("Warning: No Key Set");
        }
        if(ImGui::BeginPopupModal("Warning: No Key Set", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)){
            ImGui::Text("Enter a key in the control window before hiding/extracting data.");
            if(ImGui::Button("Close", ImVec2(120, 0))){
                appState.noKeyWarning = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if(appState.noDataWarning){
            ImGui::OpenPopup("Warning: No Data Entered");
        }
        if(ImGui::BeginPopupModal("Warning: No Data Entered", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)){
            ImGui::Text("Enter data in the control window before hiding data.");
            if(ImGui::Button("Close", ImVec2(120, 0))){
                appState.noDataWarning = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if(appState.saveAsWarning){
            ImGui::OpenPopup("Warning: Failed to save image");
        }
        if(ImGui::BeginPopupModal("Warning: Failed to save image", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)){
            ImGui::Text("Select a new location or image.");
            if(ImGui::Button("Close", ImVec2(120, 0))){
                appState.saveAsWarning = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if(appState.saveOverWarning){
            ImGui::OpenPopup("Warning: No Image Loaded");
        }
        if(ImGui::BeginPopupModal("Warning: No Image Loaded", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)){
            ImGui::Text("Select a new image in the File Explorer.");
            if(ImGui::Button("Close", ImVec2(120, 0))){
                appState.saveOverWarning = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    //************************************************************************************************************//

    GLuint loadTexture(const std::string& path){
        Image loadedImg = appState.steganoObj.loadAndConvert(path);

        GLuint image_texture;
        glGenTextures(1, &image_texture);
        glBindTexture(GL_TEXTURE_2D, image_texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, loadedImg.width, loadedImg.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, loadedImg.data);

        appState.steganoObj.cleanImage(loadedImg);
        return image_texture;
    }
}