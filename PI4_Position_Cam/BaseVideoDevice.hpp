//
//  BaseVideoDevice.hpp
//  PI4_Position_Cam
//
//  Created by Cedric Leblond Menard on 17-03-16.
//  Copyright © 2017 AER4875. All rights reserved.
//

#ifndef BaseVideoDevice_hpp
#define BaseVideoDevice_hpp

#include <stdio.h>
#include "opencv2/opencv.hpp"

class BaseVideoDevice {
    virtual void getImage(cv::OutputArray img) = 0;
    friend BaseVideoDevice &operator>>(BaseVideoDevice &input, cv::InputOutputArray &output);
public:
    
};


BaseVideoDevice &operator>>(BaseVideoDevice &input, cv::InputOutputArray &output);

#endif /* BaseVideoDevice_hpp */
