#ifndef APPLICATION_H
#define APPLICATION_H

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

namespace appUI
{
        GLuint loadTexture(const char* path);
        void renderUI();
}

#endif