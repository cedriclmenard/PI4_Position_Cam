//
//  ImageView.hpp
//  PI4_Position_Cam
//
//  Created by Cedric Leblond Menard on 17-02-22.
//  Copyright © 2017 AER4875. All rights reserved.
//
// Ref: http://www.willusher.io/sdl2%20tutorials/2013/08/17/lesson-1-hello-world

#ifndef ImageView_hpp
#define ImageView_hpp

#include <stdio.h>
#include <iostream>
#include "SDL2/SDL.h"
#include "Views.hpp"

typedef int KeyPress;

class ImageView{
public:
    ImageView(std::string windowName,unsigned int x, unsigned int y, unsigned int width, unsigned height);
    ~ImageView();
    void showBGR(unsigned char bgr[],unsigned int width, unsigned int height);
    void showGrayscale(unsigned char *gray, unsigned int width, unsigned int height);
    void close();
    KeyPress waitKeyAndProcessEvent(unsigned int waitTimeMS);
    void setMouseDownCallback(void (*mouseDownCallback)(int x, int y, void* userData), void* userData);
    
    
    
private:
    unsigned int _sizeX;
    unsigned int _sizeY;
    SDL_Window *_window;
    SDL_Renderer *_renderer;
    //SDL_Surface *currentSurface;
    SDL_Texture *_currentTexture;
    
    // Mouse callback
    void (*_mouseDownCallback)(int, int, void*) = NULL;
    void* _mouseDownCallbackUserData;
    
};

#endif /* ImageView_hpp */
