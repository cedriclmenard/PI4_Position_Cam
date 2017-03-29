//
//  PSEyeOCVVideoDevice.cpp
//  PI4_Position_Cam
//
//  Created by Cedric Leblond Menard on 17-02-18.
//  Copyright © 2017 AER4875. All rights reserved.
//

#include "PSEyeOCVVideoDevice.hpp"

PSEyeOCVVideoDevice::PSEyeOCVVideoDevice(unsigned int deviceIndex){
    // Initialize OpenCV frame matrix
    //lastFrame.create(480, 640, CV_8UC3);
    
    // Initialize video feed from PSEye
    std::vector<ps3eye::PS3EYECam::PS3EYERef> devices( ps3eye::PS3EYECam::getDevices() );
    cam = devices.at(deviceIndex);
    assert(cam->init(640,480,60));  // BGR output format, as per OpenCV mat format
    cam->start();
    camWasInit = true;
    
}

PSEyeOCVVideoDevice::PSEyeOCVVideoDevice() {
    //lastFrame.create(480, 640, CV_8UC3);
}

bool PSEyeOCVVideoDevice::setDeviceIndexNotInit(unsigned int deviceIndex) {
    if (!camWasInit) {
        std::vector<ps3eye::PS3EYECam::PS3EYERef> devices( ps3eye::PS3EYECam::getDevices() );
        cam = devices.at(deviceIndex);
        assert(cam->init(640,480,60));  // BGR output format, as per OpenCV mat format
        cam->start();
        camWasInit = true;
        return true;
    } else {
        return false;
    }
}

PSEyeOCVVideoDevice::~PSEyeOCVVideoDevice(){
    if (camWasInit)
        cam->stop();
}

void PSEyeOCVVideoDevice::setExposure(uint8_t value){
    cam->setExposure(value);
}

uint8_t PSEyeOCVVideoDevice::getExposure(){
    return cam->getExposure();
}

void PSEyeOCVVideoDevice::setGain(uint8_t value){
    cam->setGain(value);
}

uint8_t PSEyeOCVVideoDevice::getGain(){
    return cam->getGain();
}

// This function may need some error checking/type checking on "output" Mat, but this way it is faster. Use with care. Might not too, because of copy-constructor implementation of cv::Mat
PSEyeOCVVideoDevice &operator>>(PSEyeOCVVideoDevice &input, cv::OutputArray output){
    uint8_t* data;
    if (output.isMat()) {
        output.create(input.cam->getHeight(), input.cam->getWidth(), CV_8UC3);
        data = output.getMatRef().data;
        input.cam->getFrame(data);
    } else if (output.isUMat()) {
        output.create(input.cam->getHeight(), input.cam->getWidth(), CV_8UC3);
        data = output.getUMatRef().getMat(cv::ACCESS_WRITE).data;
        input.cam->getFrame(data);
    }
    
    return input;
}

unsigned int PSEyeOCVVideoDevice::getNumberOfAvailableDevices() {
    std::vector<ps3eye::PS3EYECam::PS3EYERef> devices( ps3eye::PS3EYECam::getDevices(true) );
    return devices.size();
}

//void PSEyeOCVVideoDevice::getImage(cv::OutputArray img) {
//    cam->getFrame(lastFrame.data);
//    img.assign(lastFrame);
//}

// TODO: Add functions to change camera settings
