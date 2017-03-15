//
//  ImageView.cpp
//  PI4_Position_Cam
//
//  Created by Cedric Leblond Menard on 17-02-22.
//  Copyright © 2017 AER4875. All rights reserved.
//

#include "ImageView.hpp"

// Compile time supporting functions

// Grayscale
static SDL_Color grayscalePalette[256];
static bool grayscalePaletteBuilt = false;


// Declaration

bool SDLisInit = false;

ImageView::ImageView(std::string windowName,unsigned int x, unsigned int y, unsigned int width, unsigned height) {
    // Initialize SDL if it is not already
    if (~SDLisInit) {
        if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0){
            std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        }
    }
    
    if (~grayscalePaletteBuilt) {
        for (int i = 0; i<256; i++) {
            grayscalePalette[i].b = grayscalePalette[i].g = grayscalePalette[i].r = i;
        }
    }
    
    
    // Create window and store pointer
    window = SDL_CreateWindow(windowName.c_str(), x, y, width, height, SDL_WINDOW_SHOWN);
    if (window == nullptr){
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
    }
    
    // Create rendered
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr){
        SDL_DestroyWindow(window);
        std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
    }
}

ImageView::~ImageView() {
    SDL_DestroyTexture(currentTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void ImageView::showBGR(unsigned char *bgr, unsigned int width, unsigned int height){
    SDL_Surface* currentSurface = SDL_CreateRGBSurfaceFrom(bgr,
                                              width,
                                              height,
                                              24,
                                              3*width,
                                              0x0000FF,
                                              0x00FF00,
                                              0xFF0000,
                                              0);
    if (currentSurface == nullptr){
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        std::cout << "SDL_CreateRGBSurfaceFrom Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
    }
    
    currentTexture = SDL_CreateTextureFromSurface(renderer, currentSurface);
    SDL_FreeSurface(currentSurface);
    if (currentTexture == nullptr){
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
    }
    
    // Invalidate backbuffer (double buffering)
    SDL_RenderClear(renderer);
    // Draw the texture
    SDL_RenderCopy(renderer, currentTexture, NULL, NULL);
    // Update the screen
    SDL_RenderPresent(renderer);
}

void ImageView::showGrayscale(unsigned char *gray, unsigned int width, unsigned int height){
    SDL_Surface* currentSurface = SDL_CreateRGBSurfaceFrom(gray,
                                                           width,
                                                           height,
                                                           8,
                                                           width,
                                                           0,
                                                           0,
                                                           0,
                                                           0);
    SDL_SetPaletteColors(currentSurface->format->palette, grayscalePalette, 0, 256);
    if (currentSurface == nullptr){
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        std::cout << "SDL_CreateRGBSurfaceFrom Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
    }
    
    currentTexture = SDL_CreateTextureFromSurface(renderer, currentSurface);
    SDL_FreeSurface(currentSurface);
    if (currentTexture == nullptr){
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
    }
    
    // Invalidate backbuffer (double buffering)
    SDL_RenderClear(renderer);
    // Draw the texture
    SDL_RenderCopy(renderer, currentTexture, NULL, NULL);
    // Update the screen
    SDL_RenderPresent(renderer);
}

void ImageView::close(){
    SDL_DestroyTexture(currentTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

KeyPress ImageView::waitKey(unsigned int waitTimeMS){
    SDL_Event e;
//    unsigned int t1 = SDL_GetTicks();
//    unsigned int dTime = 0;
//    unsigned int t2 = 0;
//    while (dTime < waitTimeMS) {
//        SDL_Delay(10);
//        if(SDL_PollEvent(&e)) {
//            if (e.type == SDL_KEYDOWN) {
//                return e.key.keysym.sym;
//            }
//            t2 = SDL_GetTicks();
//            dTime = t2-t1;
//            t1 = t2;
//        }
//    }
    SDL_Delay(waitTimeMS);
    if(SDL_PollEvent(&e)) {
        if (e.type == SDL_KEYDOWN) {
            return e.key.keysym.sym;
        } else if (e.type == SDL_MOUSEBUTTONDOWN) {
            if (_mouseDownCallback != NULL) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                _mouseDownCallback(x,y,_mouseDownCallbackUserData);
            }
        }
    }
    return -1;
}

void ImageView::setMouseDownCallback(void (*mouseDownCallback)(int x, int y, void* userData), void* userData){
    _mouseDownCallbackUserData = userData;
    _mouseDownCallback = mouseDownCallback;
}

