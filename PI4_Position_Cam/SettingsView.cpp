//
//  SettingsView.cpp
//  PI4_Position_Cam
//
//  Created by Cedric Leblond Menard on 17-03-20.
//  Copyright © 2017 AER4875. All rights reserved.
//

#include "SettingsView.hpp"


// MARK: GUI style variables
ImVec4 clearColor = ImColor(255,255,255);
ImVec4 titleBarColor = ImColor(150,150,150);
ImVec4 titleBarColorDark = ImColor(90,90,90);
ImVec4 closeButtonLight = ImColor(180,50,50);
ImVec4 closeButtonDark = ImColor(130,80,80);

// MARK: Currently shown windows variable
bool show_settings_window = true;
bool show_initial_image_window = true;
bool show_binary_image_window = true;
bool show_next_file_popup = false;

// MARK: State variables
bool filesystemWasSet = false;

// MARK: File system variables
std::string basePath;
std::vector<std::string> files;

// MARK: Other file global variables
std::string calibrationFilePath;

// MARK: Other functions
void setFileSystem(std::string fileExt) {
    basePath = SDL_GetBasePath();
    GetFilesInDirectory(files, basePath, fileExt);
    filesystemWasSet = true;
}

void setFilesToPath(std::string path, std::string fileExt) {
    GetFilesInDirectory(files, path, fileExt);
}

//bool getFileIdxInFiles(void* data, int idx, const char** outText) {
//    std::vector<std::string> _files = *((std::vector<std::string>*) data);
//    *outText = _files.at(idx).c_str();
//    return true;
//}

void fileSelection() {
    static int selectedIdx = -1;
    show_next_file_popup = false;
    if (!filesystemWasSet) setFileSystem("");
    if (ImGui::BeginPopupModal("fileselection")) {
        ImGui::SetWindowSize(ImVec2(600, 300),ImGuiSetCond_Once);
        static bool firstTime = true;
        static char buf[500];
        if (firstTime) {
            firstTime = false;
            basePath.copy(buf, 500);
        }
        
        if (ImGui::InputText("Filepath", buf, 500, ImGuiInputTextFlags_EnterReturnsTrue)) {
            //basePath = buf;
            files.clear();
            setFilesToPath(buf, "");
        }
        ImGui::ListBoxHeader("Files");//, ImGui::GetItemRectSize());
        for (auto iter = files.begin(); iter < files.end(); iter++) {
            if (ImGui::Selectable(iter->c_str(), (iter - files.begin()) == selectedIdx)) {
                selectedIdx = iter - files.begin();
            }
        }
        ImGui::ListBoxFooter();
        ImGui::Separator();
        
        ImGui::Columns(5, NULL, false);
        for (int i = 0; i < 3;i++) ImGui::NextColumn();
        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::NextColumn();
        if (ImGui::Button("Load Selected")) {
            if (selectedIdx >= 0) {
                calibrationFilePath = files.at(selectedIdx);
            }
        }
        
        ImGui::EndPopup();
    }
}

// MARK: Windows

void showSettingsWindow(std::vector<float> &data, int* bracketValue) {
    ImGui::Begin("Settings", &show_settings_window);
    ImGui::Text("This is a test");
    ImGui::SliderInt("Color bracket", bracketValue, 1, 150);
    //ImGui::TextWrapped("%s", text);
    ImGui::PlotLines("Detected Lines", data.data(), data.size());
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}

void showInitialImageWindow(unsigned char* bgr, int w, int h, void (*_mouseCallbackOnBGRImage)(int x, int y, void* userData), void *_mouseCallbackOnBGRImageUserData) {
    ImGui::SetNextWindowPos(ImVec2(300, 20), ImGuiSetCond_Once);
    ImGui::Begin("Initial Image", &show_initial_image_window, ImVec2(0,0), -1.0f, ImGuiWindowFlags_NoResize);
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

void showBinaryImageWindow(unsigned char* grayscale, int w, int h) {
    ImGui::SetNextWindowPos(ImVec2(600, 20), ImGuiSetCond_Once);
    ImGui::Begin("Binary Image", &show_binary_image_window, ImVec2(0,0), -1.0f, ImGuiWindowFlags_NoResize);
    ImTextureID tex = (ImTextureID)grayscaleImageToTexture(grayscale, w, h);
    ImGui::Image(tex, ImVec2(w,h));
    ImGui::End();
}

void showMenuFile() {
    if (ImGui::MenuItem("Load Calibration Data")) {
        show_next_file_popup = true;
    }
}


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
    ImGui::PushStyleColor(ImGuiCol_CloseButton, closeButtonDark);
    ImGui::PushStyleColor(ImGuiCol_CloseButtonActive, closeButtonLight);
    ImGui::PushStyleColor(ImGuiCol_CloseButtonHovered, closeButtonLight);
    
}

SettingsView::~SettingsView() {
    ImGui_ImplSdl_Shutdown();
    SDL_GL_DeleteContext(_glContext);
    SDL_DestroyWindow(_window);
    
    QuitSDLOGLForViews();
}

void SettingsView::runForThisFrame(SDL_Event &event, unsigned char* bgr, unsigned char* grayscale, int w, int h, const char* text, std::vector<float> &data) {
    // This is the main loop for rendering this window
//    static bool showSettings = true;
//    static bool showInitialImage = true;
//    static bool showBinaryImage = true;
    
    if (_toShow) {
        
        // MARK: Event processing
        if (event.type == SDL_QUIT) {
            _toQuit = true;
        }
        ImGui_ImplSdl_ProcessEvent(&event);
        ImGui_ImplSdl_NewFrame(_window);
        
        // Menu bar
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                showMenuFile();
                ImGui::EndMenu();
            }
            
            ImGui::EndMainMenuBar();
        }
        
        
        // MARK: Window processing
        
        if (show_settings_window) showSettingsWindow(data, _bracketValue);
        
        if (show_initial_image_window && bgr != NULL) showInitialImageWindow(bgr, w, h, _mouseCallbackOnBGRImage, _mouseCallbackOnBGRImageUserData);

        if (show_binary_image_window && grayscale != NULL) showBinaryImageWindow(grayscale, w, h);
        
        if (show_next_file_popup) {
            ImGui::OpenPopup("fileselection");
        }
        fileSelection();
        
        
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

void SettingsView::setLoadCalibrationCallback(void (*loadCalibration)(std::string* filePath, void* userData), void* loadCalibrationUserData) {
    _loadCalibration = loadCalibration;
    _loadCalibrationUserData = loadCalibrationUserData;
}
void SettingsView::setSaveCalibrationCallback(void (*saveCalibration)(std::string* filePath, void* userData), void* saveCalibrationUserData) {
    _saveCalibration = saveCalibration;
    _saveCalibrationUserData = saveCalibrationUserData;
}
