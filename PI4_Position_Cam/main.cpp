//
//  main.cpp
//  PI4_Position_Cam
//
//  Created by Cedric Leblond Menard on 17-02-17.
//  Copyright © 2017 AER4875. All rights reserved.
//

#include <iostream>
#include "opencv2/core.hpp"
#include "ps3eye.h"
#include "PSEyeOCVVideoDevice.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "SDL2/SDL.h"
#include "ImageView.hpp"
#include "SettingsView.hpp"

cv::Vec3b centerColor = cv::Vec3b(155,155,155);


void mouseCallback(int x, int y, void* userData) {
    centerColor = (*((cv::UMat*)userData)).getMat(cv::ACCESS_READ).at<cv::Vec3b>(y,x);
    std::cout << "BGR is : " << +centerColor[0] << ", " << +centerColor[1] << ", " << +centerColor[2] << std::endl;
}

int main(int argc, const char * argv[]) {
    
        
    
    
    // Setup input device
    PSEyeOCVVideoDevice dev = PSEyeOCVVideoDevice(0);
    cv::UMat img;
    cv::UMat binImg;
    dev.setExposure(150);
    dev.setGain(63);
    
    //ImageView view = ImageView("test", 100, 100, 640, 480);
    //ImageView view2 = ImageView("test Binary",100,580,640,480);
    //view.setMouseDownCallback(mouseCallback,&img);
    
    SettingsView settView = SettingsView("This is a test", 0, 0, 1280, 720);
    settView.setMouseCallbackOnBGRImage(mouseCallback, &img);
    
    
    
    while (!settView.hasEnded()) {
        dev >> img;
        cv::medianBlur(img, img, 7);
        cv::inRange(img, centerColor - cv::Vec3b(20,20,20), centerColor + cv::Vec3b(20,20,20), binImg);
        
        //view.showBGR(img.getMat(cv::ACCESS_READ).data, 640, 480);
        //view2.showGrayscale(binImg.getMat(cv::ACCESS_READ).data, 640, 480);
        
        
        SDL_Event e;
        SDL_PollEvent(&e);
        settView.runForThisFrame(e, img.getMat(cv::ACCESS_READ).data, binImg.getMat(cv::ACCESS_READ).data, 640, 480);
    }
    
    
    
    // SDL quit
    //SDL_Quit();
    return 0;
}





//// ImGui - standalone example application for SDL2 + OpenGL
//// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
//
//#include "imgui.h"
//#include "imgui_impl_sdl.h"
//#include <stdio.h>
//#include <SDL2/SDL.h>
//#include <SDL2/SDL_opengl.h>
//#include <iostream>
//#include "opencv2/core.hpp"
//#include "ps3eye.h"
//#include "PSEyeOCVVideoDevice.hpp"
//#include "opencv2/highgui.hpp"
//#include "opencv2/imgproc.hpp"
//#include "Views.hpp"
//
//cv::Vec3b centerColor = cv::Vec3b(155,155,155);
//
//int main(int, char**)
//{
//    // Setup input device
//    PSEyeOCVVideoDevice dev = PSEyeOCVVideoDevice(0);
//    cv::UMat img;
//    cv::UMat binImg;
//    dev.setExposure(150);
//    dev.setGain(63);
//
//    
//    // Setup SDL
//    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0)
//    {
//        printf("Error: %s\n", SDL_GetError());
//        return -1;
//    }
//    
//    // Setup window
//    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
//    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
//    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
//    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
//    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
//    SDL_DisplayMode current;
//    SDL_GetCurrentDisplayMode(0, &current);
//    SDL_Window *window = SDL_CreateWindow("ImGui SDL2+OpenGL example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
//    SDL_GLContext glcontext = SDL_GL_CreateContext(window);
//    
//    // Setup ImGui binding
//    ImGui_ImplSdl_Init(window);
//    
//    // Load Fonts
//    // (there is a default font, this is only if you want to change it. see extra_fonts/README.txt for more details)
//    //ImGuiIO& io = ImGui::GetIO();
//    //io.Fonts->AddFontDefault();
//    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/Cousine-Regular.ttf", 15.0f);
//    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/DroidSans.ttf", 16.0f);
//    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyClean.ttf", 13.0f);
//    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyTiny.ttf", 10.0f);
//    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
//    
//    bool show_test_window = true;
//    bool show_another_window = false;
//    ImVec4 clear_color = ImColor(114, 144, 154);
//    
//    // Main loop
//    bool done = false;
//    while (!done)
//    {
//        SDL_Event event;
//        while (SDL_PollEvent(&event))
//        {
//            ImGui_ImplSdl_ProcessEvent(&event);
//            if (event.type == SDL_QUIT)
//                done = true;
//        }
//        ImGui_ImplSdl_NewFrame(window);
//        
//        // 1. Show a simple window
//        // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
//        {
//            static float f = 0.0f;
//            ImGui::Text("Hello, world!");
//            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
//            ImGui::ColorEdit3("clear color", (float*)&clear_color);
//            if (ImGui::Button("Test Window")) show_test_window ^= 1;
//            if (ImGui::Button("Another Window")) show_another_window ^= 1;
//            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
//            
//            // Show image
//            dev >> img;
//            //cv::medianBlur(img, img, 7);
//            cv::inRange(img, centerColor - cv::Vec3b(20,20,20), centerColor + cv::Vec3b(20,20,20), binImg);
//            GLuint text = bgrImageToTexture(img.getMat(cv::ACCESS_READ).data, img.cols, img.rows);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//            ImGui::Image((ImTextureID) text, ImVec2(img.cols, img.rows));
//            
//        }
//        
//        // 2. Show another simple window, this time using an explicit Begin/End pair
//        if (show_another_window)
//        {
//            ImGui::SetNextWindowSize(ImVec2(200,100), ImGuiSetCond_FirstUseEver);
//            ImGui::Begin("Another Window", &show_another_window);
//            ImGui::Text("Hello");
//            ImGui::End();
//        }
//        
//        // 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
//        if (show_test_window)
//        {
//            ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
//            ImGui::ShowTestWindow(&show_test_window);
//        }
//        
//        // Rendering
//        glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
//        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
//        glClear(GL_COLOR_BUFFER_BIT);
//        ImGui::Render();
//        SDL_GL_SwapWindow(window);
//    }
//    
//    // Cleanup
//    ImGui_ImplSdl_Shutdown();
//    SDL_GL_DeleteContext(glcontext);
//    SDL_DestroyWindow(window);
//    SDL_Quit();
//    
//    return 0;
//}
