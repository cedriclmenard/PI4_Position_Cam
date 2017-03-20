//
//  Views.hpp
//  PI4_Position_Cam
//
//  Created by Cedric Leblond Menard on 17-03-20.
//  Copyright © 2017 AER4875. All rights reserved.
//

#ifndef Views_hpp
#define Views_hpp

#include <stdio.h>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>


/**
 Safe initialize SDL and OGL
 */
void InitializeSDLOGLForViews();

/**
 Safe deinit (with counting) SDL and OGL
 */
void QuitSDLOGLForViews();


GLuint bgrImageToTexture(unsigned char *bgr, unsigned int width, unsigned int height);

GLuint grayscaleImageToTexture(unsigned char *gray, unsigned int width, unsigned int height);



#endif /* Views_hpp */
