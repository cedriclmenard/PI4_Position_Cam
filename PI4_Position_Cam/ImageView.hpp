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
#include "pthread.h"

enum THREADSIG {
    STOP,
    CONTINUE
};

class ImageView{
public:
    ImageView(std::string windowName,unsigned int x, unsigned int y, unsigned int width, unsigned height);
    ~ImageView();
    void showBGR(unsigned char bgr[],unsigned int width, unsigned int height);
    void close();
    bool isValid();
    
    
private:
    unsigned int sizeX;
    unsigned int sizeY;
    SDL_Window *window;
    SDL_Renderer *renderer;
    //SDL_Surface *currentSurface;
    SDL_Texture *currentTexture;
    
    
    // Event thread
    pthread_t _eventThreadId;
    int eventThreadExitSignal = CONTINUE;
    friend void *eventThreadStart(void*);
    void (*_mouseDownCallback)(int x, int y) = nullptr;
    
};

#endif /* ImageView_hpp */
