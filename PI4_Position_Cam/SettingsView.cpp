//
//  SettingsView.cpp
//  PI4_Position_Cam
//
//  Created by Cedric Leblond Menard on 17-03-20.
//  Copyright © 2017 AER4875. All rights reserved.
//

#include "SettingsView.hpp"

ImVec4 clearColor = ImColor(255,255,255);
ImVec4 titleBarColor = ImColor(150,150,150);
ImVec4 titleBarColorDark = ImColor(90,90,90);

SettingsView::SettingsView(std::string windowName, int x, int y, int w, int h, int* bracketValue) :
_windowName(windowName), _bracketValue(bracketValue)
{
    InitializeSDLOGLForViews();
    
    SDL_GetCurrentDisplayMode(0, &_displayMode);
    _window = SDL_CreateWindow(windowName.c_str(), x, y, w, h, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    _glContext = SDL_GL_CreateContext(_window);
    ImGui_ImplSdl_Init(_window);
    
    ImGui::PushStyleColor(ImGuiCol_TitleBg, titleBarColorDark);
    ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, titleBarColorDark);
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, titleBarColor);
    
}

SettingsView::~SettingsView() {
    ImGui_ImplSdl_Shutdown();
    SDL_GL_DeleteContext(_glContext);
    SDL_DestroyWindow(_window);
    
    QuitSDLOGLForViews();
}

void SettingsView::runForThisFrame(SDL_Event &event, unsigned char* bgr, unsigned char* grayscale, int w, int h, const char* text, std::vector<float> data) {
    // This is the main loop for rendering this window
    static bool showSettings = true;
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
            ImGui::Begin("Settings", &showSettings);
            ImGui::Text("This is a test");
            ImGui::SliderInt("Color bracket", _bracketValue, 1, 150);
            //ImGui::TextWrapped("%s", text);
            ImGui::PlotLines("Detected Lines", data.data(), data.size());
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        
        {
            ImGui::SetNextWindowPos(ImVec2(300, 20), ImGuiSetCond_Once);
            ImGui::Begin("Initial Image", &showInitialImage, ImVec2(0,0), -1.0f, ImGuiWindowFlags_NoResize);
            //ImGui::Begin("Initial Image", &showInitialImage);
            ImTextureID tex = (ImTextureID)bgrImageToTexture(bgr, w, h);
            ImGui::Image(tex, ImVec2(w,h));
            if (ImGui::IsItemClicked()) {
                if (_mouseCallbackOnBGRImage != NULL) {
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
            ImGui::SetNextWindowPos(ImVec2(600, 20), ImGuiSetCond_Once);
            ImGui::Begin("Binary Image", &showBinaryImage, ImVec2(0,0), -1.0f, ImGuiWindowFlags_NoResize);
            ImTextureID tex = (ImTextureID)grayscaleImageToTexture(grayscale, w, h);
            ImGui::Image(tex, ImVec2(w,h));
            ImGui::End();
        }
        
        
        
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
