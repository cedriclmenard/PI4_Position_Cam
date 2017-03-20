//
//  SettingsView.cpp
//  PI4_Position_Cam
//
//  Created by Cedric Leblond Menard on 17-03-20.
//  Copyright © 2017 AER4875. All rights reserved.
//

#include "SettingsView.hpp"

ImVec4 clearColor = ImColor(114,144,154);

SettingsView::SettingsView(std::string windowName, int x, int y, int w, int h) :
_windowName(windowName)
{
    InitializeSDLOGLForViews();
    
    SDL_GetCurrentDisplayMode(0, &_displayMode);
    _window = SDL_CreateWindow(windowName.c_str(), x, y, w, h, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    _glContext = SDL_GL_CreateContext(_window);
    ImGui_ImplSdl_Init(_window);
    
}

SettingsView::~SettingsView() {
    ImGui_ImplSdl_Shutdown();
    SDL_GL_DeleteContext(_glContext);
    SDL_DestroyWindow(_window);
    
    QuitSDLOGLForViews();
}

void SettingsView::runForThisFrame(SDL_Event &event, unsigned char* bgr, unsigned char* grayscale, int w, int h) {
    // This is the main loop for rendering this window
    static bool showInitialImage = true;
    static bool showBinaryImage = true;
    
    if (_toShow) {
        
        // MARK: Event processing
        if (event.type == SDL_QUIT) {
            _toQuit = true;
        }
        ImGui_ImplSdl_ProcessEvent(&event);
        ImGui_ImplSdl_NewFrame(_window);
        
        
        
        // MARK: Window processing
        {
            ImGui::Begin("Settings");
            static float f = 0;
            ImGui::Text("This is a test");
            ImGui::SliderFloat("This is a float", &f, 0, 1);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        
        {
            ImGui::SetNextWindowPos(ImVec2(300, 20), ImGuiSetCond_Once);
            ImGui::SetNextWindowPos(ImVec2(w, h), ImGuiSetCond_Once);
            ImGui::Begin("Initial Image", &showInitialImage);
            ImTextureID tex = (ImTextureID)bgrImageToTexture(bgr, w, h);
            ImGui::Image(tex, ImVec2(w,h));
            if (ImGui::IsItemClicked()) {
                if (_mouseCallbackOnBGRImage != NULL) {
                    ImVec2 mouse = ImGui::GetCursorPos();
                    int x, y;
                    x = ImGui::GetMousePos().x - ImGui::GetWindowPos().x - ImGui::GetWindowContentRegionMin().x;
                    y = ImGui::GetMousePos().y - ImGui::GetWindowPos().y - ImGui::GetWindowContentRegionMin().y;
                    x = x > w ? w : x;
                    y = y > h ? h : y;
                    std::cout << "Mouse x = " << x << ", Mouse y = " << y << std::endl;
                    _mouseCallbackOnBGRImage(x, y, _mouseCallbackOnBGRImageUserData);
                }
                
                
            }
            ImGui::End();
        }
        {
            ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_Once);
            ImGui::Begin("Binary Image", &showBinaryImage);
            ImTextureID tex = (ImTextureID)grayscaleImageToTexture(grayscale, w, h);
            ImGui::Image(tex, ImVec2(w,h));
            ImGui::End();
        }
        
        //bool show_test_window = true;
        //ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
        //ImGui::ShowTestWindow(&show_test_window);
        
        
        
        // MARK: Rendering
        glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
        glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::Render();
        SDL_GL_SwapWindow(_window);
    }
}

bool SettingsView::hasEnded() {
    return _toQuit;
}

void SettingsView::showNextFrame() {
    _toShow = true;
}

void SettingsView::hideNextFrame() {
    _toShow = false;
}

void SettingsView::setMouseCallbackOnBGRImage(void (*mouseCallbackOnBGRImage)(int x, int y, void* userData), void* mouseCallbackOnBGRImageUserData) {
    _mouseCallbackOnBGRImage = mouseCallbackOnBGRImage;
    _mouseCallbackOnBGRImageUserData = mouseCallbackOnBGRImageUserData;
}
