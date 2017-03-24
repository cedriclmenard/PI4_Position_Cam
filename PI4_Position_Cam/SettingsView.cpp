//
//  SettingsView.cpp
//  PI4_Position_Cam
//
//  Created by Cedric Leblond Menard on 17-03-20.
//  Copyright © 2017 AER4875. All rights reserved.
//

#include "SettingsView.hpp"
#include "Fonts.hpp"

// MARK: GUI style variables
ImVec4 clearColor = ImColor(255,255,255);
ImVec4 titleBarColor = ImColor(150,150,150);
ImVec4 titleBarColorDark = ImColor(90,90,90);
ImVec4 closeButtonLight = ImColor(180,50,50);
ImVec4 closeButtonDark = ImColor(130,80,80);
ImVec4 menuBarBg = ImColor(90, 90, 90);
ImVec4 menuBarHighlight = ImColor(150, 150, 150);
ImVec4 buttonColor = ImColor(130,204,51,153);
ImVec4 buttonHoveredColor = ImColor(100,153,20,255);





// MARK: Currently shown windows variable
bool show_settings_window = true;
bool show_initial_image_window = true;
bool show_binary_image_window = true;
bool show_load_file_popup = false;
bool show_save_file_popup = false;
bool mode_calibration = false;






// MARK: State variables
bool filesystemWasSet = false;






// MARK: File system variables
std::string basePath;
std::vector<std::string> files;







// MARK: Other file global variables
std::string calibrationFilePath;

//CalibrationParameters calibParams;








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

void fileSelection(void (*callback)(std::string* filePath, void* userData) = NULL, void* callbackUserData = NULL) {
    static int selectedIdx = -1;
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
                if (callback != NULL) {
                    callback(&calibrationFilePath, callbackUserData);
                }
            }
        }
        
        ImGui::EndPopup();
    }
}

void fileSave(void (*callback)(std::string* filePath, void* userData) = NULL, void* callbackUserData = NULL) {
    if (ImGui::BeginPopupModal("filesave")) {
        static char buf[500] = "";
        ImGui::InputText("Save file (including path)", buf, 500);
        ImGui::Separator();
        
        ImGui::Columns(5, NULL, false);
        for (int i = 0; i < 3;i++) ImGui::NextColumn();
        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::NextColumn();
        if (ImGui::Button("Save")) {
            calibrationFilePath = buf;
            if (callback != NULL) {
                callback(&calibrationFilePath, callbackUserData);
            }
        }
        ImGui::EndPopup();
    }
}










// MARK: Windows

void showSettingsWindow(SyncThreadsParameters *sync , int* bracketValue) {
    ImGui::Begin("Settings", &show_settings_window);
    ImGui::Text("This is a test");
    ImGui::SliderInt("Color bracket", bracketValue, 1, 150);
    //ImGui::TextWrapped("%s", text);
    ImGui::PlotLines("Detected Lines", (*sync).result.data(), (*sync).result.size());
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    
    ImGui::Separator();
    ImGui::Text("Calibration");
    ImGui::Text("Number of squares in the calibration pattern");
    ImGui::PushItemWidth(100);
    ImGui::InputInt("Width", &(*sync).calibParams.boardNumOfSquaresInWidth);
    if ((*sync).calibParams.boardNumOfSquaresInWidth < 2) (*sync).calibParams.boardNumOfSquaresInWidth = 2;
    ImGui::SameLine();
    ImGui::InputInt("Height", &(*sync).calibParams.boardNumOfSquaresInHeight);
    if ((*sync).calibParams.boardNumOfSquaresInHeight < 2) (*sync).calibParams.boardNumOfSquaresInHeight = 2;
    ImGui::InputFloat("Square Size in meter", &(*sync).calibParams.squareSizeInM);
    if ((*sync).calibParams.squareSizeInM <= 0) (*sync).calibParams.boardNumOfSquaresInHeight = 0.01;
    ImGui::PopItemWidth();
    
    if (ImGui::Button("Start Calibration")) {
        (*sync).startCalibration = true;
    }
    
    
    ImGui::End();
}

void showInitialImageWindow(GLuint bgrTex, int w, int h, void (*_mouseCallbackOnBGRImage)(int x, int y, void* userData), void *_mouseCallbackOnBGRImageUserData) {
    ImGui::SetNextWindowPos(ImVec2(300, 20), ImGuiSetCond_Once);
    ImGui::Begin("Initial Image", &show_initial_image_window, ImVec2(0,0), -1.0f, ImGuiWindowFlags_NoResize);
    ImGui::Image((ImTextureID) bgrTex, ImVec2(w,h));
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

void showBinaryImageWindow(GLuint grayTex, int w, int h) {
    ImGui::SetNextWindowPos(ImVec2(600, 20), ImGuiSetCond_Once);
    ImGui::Begin("Binary Image", &show_binary_image_window, ImVec2(0,0), -1.0f, ImGuiWindowFlags_NoResize);
    ImGui::Image((ImTextureID) grayTex, ImVec2(w,h));
    ImGui::End();
}

void showMenuFile() {
    if (ImGui::MenuItem("Load Calibration Data",NULL,false, !mode_calibration)) {
        show_load_file_popup = true;
    }
    if (ImGui::MenuItem("Save Calibration Data",NULL,false, !mode_calibration)) {
        show_save_file_popup = true;
    }
}


SettingsView::SettingsView(std::string windowName, int x, int y, int w, int h, int* bracketValue) :
_windowName(windowName), _bracketValue(bracketValue)
{
    InitializeSDLOGLForViews();
    
    SDL_GetCurrentDisplayMode(0, &_displayMode);
    _window = SDL_CreateWindow(windowName.c_str(), x, y, w, h, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    _glContext = SDL_GL_CreateContext(_window);
    ImGui_ImplSdl_Init(_window);
    
    ImGui::PushStyleColor(ImGuiCol_TitleBg, titleBarColorDark);
    ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, titleBarColorDark);
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, titleBarColor);
    ImGui::PushStyleColor(ImGuiCol_CloseButton, closeButtonDark);
    ImGui::PushStyleColor(ImGuiCol_CloseButtonActive, closeButtonLight);
    ImGui::PushStyleColor(ImGuiCol_CloseButtonHovered, closeButtonLight);
    ImGui::PushStyleColor(ImGuiCol_MenuBarBg, menuBarBg);
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, menuBarHighlight);
    ImGui::PushStyleColor(ImGuiCol_Header, menuBarHighlight);
    ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonHoveredColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonColor);
    
    
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontDefault();
    //    static ImFontConfig config = ImFontConfig::ImFontConfig();
    //    config.OversampleH = 8;
    //    config.OversampleV = 1;
    io.Fonts->AddFontFromMemoryCompressedTTF(font_cousine_regular_compressed_data, font_cousine_regular_compressed_size, 15.0f);
    //ImGui::PushFont(font);
    io.FontDefault = io.Fonts->Fonts[1];
    //io.FontGlobalScale = io.FontGlobalScale*0.5;
    
}

SettingsView::~SettingsView() {
    ImGui_ImplSdl_Shutdown();
    SDL_GL_DeleteContext(_glContext);
    SDL_DestroyWindow(_window);
    
    QuitSDLOGLForViews();
}

void SettingsView::runForThisFrame(int w, int h, SyncThreadsParameters *sync) {
    SDL_Event event;
    SDL_PollEvent(&event);
    ImGui_ImplSdl_ProcessEvent(&event);
    ImGui_ImplSdl_NewFrame(_window);
    
    // MARK: Event processing
    if (event.type == SDL_QUIT) {
        _toQuit = true;
        return;
    }
    
    
    
    
    
    // Menu bar
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            showMenuFile();
            ImGui::EndMenu();
        }
        
        ImGui::EndMainMenuBar();
    }
    
    
    // MARK: Window processing
    if (!(*sync).startCalibration) {
        
        static ImGuiStyle style = ImGui::GetStyle();
        ImGui::ShowStyleEditor(&style);
        if (show_settings_window) showSettingsWindow(sync , _bracketValue);
        
        if (show_initial_image_window && (*sync).bgrTex != 0) showInitialImageWindow((*sync).bgrTex, w, h, _mouseCallbackOnBGRImage, _mouseCallbackOnBGRImageUserData);
        
        if (show_binary_image_window && (*sync).grayTex != 0) showBinaryImageWindow((*sync).grayTex, w, h);
        
        
        
        if (show_load_file_popup) {
            ImGui::OpenPopup("fileselection");
            show_load_file_popup = false;
        }
        if (show_save_file_popup) {
            ImGui::OpenPopup("filesave");
            show_save_file_popup = false;
        }
        
        
        fileSelection(_loadCalibration, _loadCalibrationUserData);
        fileSave(_saveCalibration, _saveCalibrationUserData);
        
        
    } else {
        
        // MARK: Calibration procedure
        static unsigned int numOfFrames = 0;
        
        ImGui::Begin("Calibration Image", NULL, ImGuiWindowFlags_NoResize);
        
        if (ImGui::Button("Cancel")) {
            (*sync).startCalibration = false;
            
        }
        
        if ((*sync).calibImgTex != 0) {
            ImGui::Image((ImTextureID)(*sync).calibImgTex, ImVec2(w,h));
        } else {
            // Renders black texture, clip to edges
            ImGui::Image((ImTextureID) singleColorRGBAToTexture(0x000000FF), ImVec2(w,h));
        }
        
        ImGui::Text("Number of valid frames kept for calibration : %u", numOfFrames);
        
        if ((*sync).lastFrameIsValid) {
            if (ImGui::Button("Keep")) {
                (*sync).useLastFrame = true;
                (*sync).lastFrameIsValid = false;
                numOfFrames++;
            }
            ImGui::SameLine();
            if (ImGui::Button("Discard")) {
                (*sync).lastFrameIsValid = false;
            }
        }
        
        
        
        if (numOfFrames > 0) {
            if (ImGui::Button("Calibrate!")) {
                (*sync).finalizeCalibration = true;
            }
        }
        
        if (!(*sync).startCalibration) {
            numOfFrames = 0;
            
        }
        
        ImGui::End();
    }
    
    
    
    
    
    
    // MARK: Rendering
    glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
    glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui::Render();
    SDL_GL_SwapWindow(_window);
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
