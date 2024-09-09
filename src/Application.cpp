#include "Application.h"
#include "imgui.h"
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>

#include "Crypto.h"
#include "Steganography.h"

namespace appUI
{
    static Steganography steganoObj;
    static Crypto cryptoObj;
    static std::string inImagePath = "Input Image Shown Here";
    static std::string outImagePath = "Altered Image Shown Here";
    static GLuint inImageTexture = 0;
    static GLuint outImageTexture = 0; 
    static char masterKey[256] = "";
    static char data[1024] = "";//adjust
    static char extractedData[1024] = "";//adjust
    static bool showFileExplorer = false;
    static std::string currentPath = std::filesystem::current_path().string();
    static std::string lastLoadedPath = "";
    static std::vector<std::string> files;
    
    
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
        window_flags |= ImGuiWindowFlags_NoScrollbar;
        window_flags |= ImGuiWindowFlags_NoCollapse;
        window_flags |= ImGuiWindowFlags_NoNav;
        window_flags |= ImGuiWindowFlags_MenuBar;
        window_flags |= ImGuiWindowFlags_NoResize;
        window_flags |= ImGuiWindowFlags_NoDecoration;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

        //Cause the UI to be fullscreen in relation to the glfw window
        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(main_viewport->WorkSize.x, main_viewport->WorkSize.y), ImGuiCond_Always);


        ImGui::Begin("SteganoPass", nullptr, window_flags);

        //Get width of the window for button and text placement 
        float windowWidth = ImGui::GetWindowSize().x;
        float halfWidth = windowWidth * 0.5 - ImGui::GetStyle().ItemSpacing.x;
        float startY = ImGui::GetCursorPosY();
        
        ImGui::SameLine(0, ImGui::GetStyle().ItemSpacing.x);


        //Input image section
        ImGui::BeginGroup();
        float inTextWidth = ImGui::CalcTextSize("Input Image").x;
        ImGui::SetCursorPos(ImVec2((halfWidth - inTextWidth) * 0.5f, startY));
        ImGui::Text("Input Image");
        ImGui::SetCursorPos(ImVec2(0, startY + ImGui::GetTextLineHeightWithSpacing()));

        if(inImageTexture == 0){
            ImGui::Button(inImagePath.c_str(), ImVec2(halfWidth, 400));
        } else{
            float aspectRatio = 0.0f;
            ImVec2 imageSize(halfWidth, halfWidth);
            if(inImagePath != lastLoadedPath){
                Image loadedImg = steganoObj.loadAndConvert(inImagePath);
                aspectRatio = (float)loadedImg.width / loadedImg.height;
                imageSize.x = halfWidth;
                imageSize.y = halfWidth / aspectRatio;
                steganoObj.cleanImage(loadedImg);
                lastLoadedPath = inImagePath;
            }
            ImGui::Image((void*)(intptr_t)inImageTexture, imageSize);
        }
        ImGui::EndGroup();

        ImGui::SameLine(0, ImGui::GetStyle().ItemSpacing.x);


        //Output image section
        ImGui::BeginGroup();
        float outTextWidth = ImGui::CalcTextSize("Output Image").x;
        ImGui::SetCursorPos(ImVec2(windowWidth * 0.5f + (halfWidth - outTextWidth) * 0.5f, startY));
        ImGui::Text("Output Image");
        ImGui::SetCursorPos(ImVec2(windowWidth * 0.5f, startY + ImGui::GetTextLineHeightWithSpacing()));
        
        if(outImageTexture == 0){
            ImGui::Button(outImagePath.c_str(), ImVec2(halfWidth, 400));
        } else{
            ImGui::Image((void*)(intptr_t)outImageTexture, ImVec2(halfWidth, 400));
        }
        ImGui::EndGroup();


        //Section for file explorer
        if(ImGui::Button("Open File Explorer")){
            showFileExplorer = true;
            updateFiles();
        }
        if(showFileExplorer){
            ImGui::Begin("File Explorer", &showFileExplorer);
            
            if(ImGui::Button("<- Go Back")){
                currentPath = std::filesystem::path(currentPath).parent_path().string();
                updateFiles();
            }

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
                        showFileExplorer = false;
                    }
                }
            }
            ImGui::End();
        }


        ImGui::InputText("Master Key", masterKey, AES_KEYLEN);
        ImGui::InputText("Data to Hide", data, IM_ARRAYSIZE(data)); //adjust buffer size at some point
        //ImGui::Text("Extracted Data", extractedData, IM_ARRAYSIZE(extractedData));

        if(ImGui::Button("Hide Data in Image")){
            //call steg/crypto funcitons
        }
        ImGui::SameLine();
        if(ImGui::Button("Extract Data from Image")){
            //call steg/crypto functions 
        }

        ImGui::End();
    }
}