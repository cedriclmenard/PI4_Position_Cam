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

class ImageView{
public:
    ImageView();
    ~ImageView();
    void show(unsigned char rbg[],unsigned int sizeX, unsigned int sizeY);
    
    
private:
    unsigned int sizeX;
    unsigned int sizeY;
};

#endif /* ImageView_hpp */
