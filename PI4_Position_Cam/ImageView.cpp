//
//  ImageView.cpp
//  PI4_Position_Cam
//
//  Created by Cedric Leblond Menard on 17-02-22.
//  Copyright © 2017 AER4875. All rights reserved.
//

#include "ImageView.hpp"

// Declaration
void *eventThreadStart(void*);

bool SDLisInit = false;

ImageView::ImageView(std::string windowName,unsigned int x, unsigned int y, unsigned int width, unsigned height) {
    // Initialize SDL if it is not already
    if (~SDLisInit) {
        if (SDL_Init(SDL_INIT_VIDEO) != 0){
            std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
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
    
    // Create event thread for this window
    if (pthread_create(&_eventThreadId, 0, eventThreadStart, (void*)this)) {
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        std::cout << "pthread_create Error" << std::endl;
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

void ImageView::close(){
    eventThreadExitSignal = STOP;
    pthread_join(_eventThreadId, NULL);
    SDL_DestroyTexture(currentTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

bool ImageView::isValid(){
    return (eventThreadExitSignal == CONTINUE) ? true : false;
}

// MARK: Multithreading

typedef ImageView* eventThreadParent;

void *eventThreadStart(void* arg){
    SDL_Event e;
    eventThreadParent parent = (eventThreadParent) arg;
    while (parent->eventThreadExitSignal == CONTINUE) {
        SDL_PollEvent(&e);
        switch (e.type) {
            case SDL_QUIT:
                parent->close();
                break;
            case SDL_KEYDOWN:
                parent->close();
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (parent->_mouseDownCallback != nullptr) {
                    parent->_mouseDownCallback(e.button.x,e.button.y);
                }
                break;
        }
    }
    pthread_exit(NULL);
}


