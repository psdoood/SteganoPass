#include "Application.h"
#include "imgui.h"
#include <string>
#include <vector>
#include "Crypto.h"

namespace appUI
{
    static char inImagePath[1024] = "Drop Your Image Here";
    static char outImagePath[1024] = "Altered Image Shown Here";
    static char masterKey[256] = "";
    static char data[1024] = "";//adjust
    static char extractedData[1024] = "";//adjust

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

        //Center "Input Image" text horizontally 
        float windowWidth = ImGui::GetWindowSize().x;
        float halfWidth = windowWidth * 0.5 - ImGui::GetStyle().ItemSpacing.x;
        float startY = ImGui::GetCursorPosY();
        
        ImGui::SameLine(0, ImGui::GetStyle().ItemSpacing.x);

        ImGui::BeginGroup();
        float inTextWidth = ImGui::CalcTextSize("Input Image").x;
        ImGui::SetCursorPos(ImVec2((halfWidth - inTextWidth) * 0.5f, startY));
        ImGui::Text("Input Image");
        ImGui::SetCursorPos(ImVec2(0, startY + ImGui::GetTextLineHeightWithSpacing()));
        ImGui::Button(inImagePath, ImVec2(halfWidth, 400));
        ImGui::EndGroup();

        ImGui::SameLine(0, ImGui::GetStyle().ItemSpacing.x);

        ImGui::BeginGroup();
        float outTextWidth = ImGui::CalcTextSize("Output Image").x;
        ImGui::SetCursorPos(ImVec2(windowWidth * 0.5f + (halfWidth - outTextWidth) * 0.5f, startY));
        ImGui::Text("Output Image");
        ImGui::SetCursorPos(ImVec2(windowWidth * 0.5f, startY + ImGui::GetTextLineHeightWithSpacing()));
        ImGui::Button(outImagePath, ImVec2(halfWidth, 400));
        ImGui::EndGroup();
        
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