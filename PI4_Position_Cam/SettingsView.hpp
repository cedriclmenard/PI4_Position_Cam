//
//  SettingsView.hpp
//  PI4_Position_Cam
//
//  Created by Cedric Leblond Menard on 17-03-20.
//  Copyright © 2017 AER4875. All rights reserved.
//

#ifndef SettingsView_hpp
#define SettingsView_hpp

#include <stdio.h>
#include <iostream>
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include "Views.hpp"
#include <vector>

class SettingsView {
    SDL_DisplayMode _displayMode;
    SDL_Window* _window;
    SDL_GLContext _glContext;
    std::string _windowName;
    bool _toShow = true;
    bool _toQuit = false;
    int* _bracketValue;
    
    // Callbacks
    void (*_mouseCallbackOnBGRImage)(int x, int y, void* userData) = NULL;
    void *_mouseCallbackOnBGRImageUserData = NULL;
    void (*_loadCalibration)(std::string* filePath, void* userData) = NULL;
    void* _loadCalibrationUserData = NULL;
    void (*_saveCalibration)(std::string* filePath, void* userData) = NULL;
    void* _saveCalibrationUserData = NULL;
    
public:
    SettingsView(std::string windowName, int x, int y, int w, int h, int* bracketValue);
    ~SettingsView();
    void runForThisFrame(SDL_Event &event, unsigned char* bgr, unsigned char* grayscale, int w, int h, const char* text, std::vector<float> &data);
    void showNextFrame();
    void hideNextFrame();
    bool hasEnded();
    void setMouseCallbackOnBGRImage(void (*mouseCallbackOnBGRImage)(int x, int y, void* userData), void* mouseCallbackOnBGRImageUserData = NULL);
    void setLoadCalibrationCallback(void (*loadCalibration)(std::string* filePath, void* userData), void* loadCalibrationUserData = NULL);
    void setSaveCalibrationCallback(void (*saveCalibration)(std::string* filePath, void* userData), void* saveCalibrationUserData = NULL);
};


#endif /* SettingsView_hpp */
