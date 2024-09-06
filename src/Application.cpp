#include "Application.h"
#include "imgui.h"
#include <string>
#include <vector>
#include "Crypto.h"

namespace appUI
{
    static char inImagePath[1024] = "Drop Image Here";
    static char outImagePath[1024] = "";
    static char masterKey[256] = "";
    static char data[1024] = "";//adjust

    void renderUI()
    {
        ImGuiWindowFlags window_flags = 0;
        window_flags |= ImGuiWindowFlags_NoScrollbar;
        window_flags |= ImGuiWindowFlags_NoCollapse;
        window_flags |= ImGuiWindowFlags_NoNav;
        window_flags |= ImGuiWindowFlags_MenuBar;
        window_flags |= ImGuiWindowFlags_NoResize;
        window_flags |= ImGuiWindowFlags_NoDecoration;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(main_viewport->WorkSize.x, main_viewport->WorkSize.y), ImGuiCond_Always);

        ImGui::Begin("SteganoPass", nullptr, window_flags);

        ImGui::Text("Input Image");
        ImGui::Button(inImagePath, ImVec2(-1, 400));
        
        ImGui::InputText("Master Key", masterKey, AES_KEYLEN);
        ImGui::InputText("Data to Hide", data, IM_ARRAYSIZE(data)); //adjust buffer size at some point
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