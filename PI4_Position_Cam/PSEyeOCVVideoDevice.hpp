//
//  PSEyeOCVVideoDevice.hpp
//  PI4_Position_Cam
//
//  Created by Cedric Leblond Menard on 17-02-18.
//  Copyright © 2017 AER4875. All rights reserved.
//

#ifndef PSEyeOCVVideoDevice_hpp
#define PSEyeOCVVideoDevice_hpp

#include <stdio.h>
#include "opencv2/core.hpp"
#include "ps3eye.h"
//#include <ctype>

class PSEyeOCVVideoDevice {
public:
    PSEyeOCVVideoDevice(unsigned int deviceIndex = 0);
    ~PSEyeOCVVideoDevice();
    void setExposure(uint8_t value);
    uint8_t getExposure();
    
private:
    ps3eye::PS3EYECam::PS3EYERef cam;
    cv::Mat lastFrame;
    friend PSEyeOCVVideoDevice &operator>>(PSEyeOCVVideoDevice &input, cv::InputOutputArray &output);
};

// Video stream operator from OpenCV
PSEyeOCVVideoDevice &operator>>(PSEyeOCVVideoDevice &input, cv::InputOutputArray &output);

#endif /* PSEyeOCVVideoDevice_hpp */
