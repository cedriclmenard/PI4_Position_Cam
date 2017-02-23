//
//  Calibrator.hpp
//  PI4_Position_Cam
//
//  Created by Cedric Leblond Menard on 17-02-21.
//  Copyright © 2017 AER4875. All rights reserved.
//

#ifndef Calibrator_hpp
#define Calibrator_hpp

#include <stdio.h>
#include "opencv2/opencv.hpp"

template <class VideoDevice>
class Calibrator{
    VideoDevice _dev;
public:
    Calibrator(VideoDevice dev);
    void calibrate(unsigned int numberOfFrames, int boardNumOfSquaresInWidth, int boardNumOfSquaresInHeight, float squareSizeInM);
    
private:
    
    
};

#endif /* Calibrator_hpp */
