//
//  Views.cpp
//  PI4_Position_Cam
//
//  Created by Cedric Leblond Menard on 17-03-20.
//  Copyright © 2017 AER4875. All rights reserved.
//

#include "Views.hpp"

static bool sdlIsInit = false;
static unsigned int currentNumberOfSDLOGLUsers = 0;

void InitializeSDLOGLForViews() {
    if (~sdlIsInit) {
        // Setup SDL and OpenGL
        if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0){
            std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        }
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        sdlIsInit = true;
    }
    currentNumberOfSDLOGLUsers++;
}

void QuitSDLOGLForViews() {
    currentNumberOfSDLOGLUsers--;
    if (sdlIsInit && currentNumberOfSDLOGLUsers == 0) {
        SDL_Quit();
        sdlIsInit = false;
    }
}




// Grayscale
static SDL_Color grayscalePalette[256];
static bool grayscalePaletteBuilt = false;




GLuint bgrImageToTexture(unsigned char *bgr, unsigned int width, unsigned int height) {
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(bgr,
                                                           width,
                                                           height,
                                                           24,
                                                           3*width,
                                                           0x0000FF,
                                                           0x00FF00,
                                                           0xFF0000,
                                                           0);
    if (surface == nullptr){
        std::cout << "SDL_CreateRGBSurfaceFrom Error: " << SDL_GetError() << std::endl;
    }
    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, surface->w, surface->h, 0, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return texture;
}

GLuint grayscaleImageToTexture(unsigned char *gray, unsigned int width, unsigned int height) {
    // Setup palettes
//    if (~grayscalePaletteBuilt) {
//        for (int i = 0; i<256; i++) {
//            grayscalePalette[i].b = grayscalePalette[i].g = grayscalePalette[i].r = i;
//        }
//    }
    
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(gray,
                                                           width,
                                                           height,
                                                           8,
                                                           width,
                                                           0,
                                                           0,
                                                           0,
                                                           0);
    //SDL_SetPaletteColors(surface->format->palette, grayscalePalette, 0, 256);
    if (surface == nullptr){
        std::cout << "SDL_CreateRGBSurfaceFrom Error: " << SDL_GetError() << std::endl;
    }
    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, surface->w, surface->h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, surface->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return texture;
}
