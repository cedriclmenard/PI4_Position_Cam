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
#include "BaseVideoDevice.hpp"
//#include <ctype>

class PSEyeOCVVideoDevice {
public:
    PSEyeOCVVideoDevice(unsigned int deviceIndex);
    PSEyeOCVVideoDevice();
    ~PSEyeOCVVideoDevice();
    
    // Options
    void setExposure(uint8_t value);
    uint8_t getExposure();
    
    void setGain(uint8_t value);
    uint8_t getGain();
    
    // Set to device (later)
    bool setDeviceIndexNotInit(unsigned int deviceIndex);
    
    static unsigned int getNumberOfAvailableDevices();
    ps3eye::PS3EYECam::PS3EYERef cam;
private:
    
    //cv::Mat lastFrame;
    friend PSEyeOCVVideoDevice &operator>>(PSEyeOCVVideoDevice &input, cv::OutputArray &output);
    bool camWasInit = false;
    //virtual void getImage(cv::OutputArray img);
    
    
};

// Video stream operator from OpenCV
PSEyeOCVVideoDevice &operator>>(PSEyeOCVVideoDevice &input, cv::OutputArray &output);

#endif /* PSEyeOCVVideoDevice_hpp */
