//
//  main.cpp
//  PI4_Position_Cam
//
//  Created by Cedric Leblond Menard on 17-02-17.
//  Copyright © 2017 AER4875. All rights reserved.
//

#include <iostream>
#include "opencv2/core.hpp"
#include "ps3eye.h"
#include "PSEyeOCVVideoDevice.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "SDL2/SDL.h"
#include "SettingsView.hpp"
#include "HorizontalThinningAlgorithm.hpp"
#include "CalibratedDevice.hpp"
#include "CommonTypes.hpp"

cv::Vec3b centerColor = cv::Vec3b(155,155,155);

// Forward declaration
static int main_imageprocessing(void* data);

SyncThreadsParameters sync;


void mouseCallback(int x, int y, void* userData) {
    centerColor = (*((cv::UMat*)userData)).getMat(cv::ACCESS_READ).at<cv::Vec3b>(y,x);
    std::cout << "BGR is : " << +centerColor[0] << ", " << +centerColor[1] << ", " << +centerColor[2] << std::endl;
}

void loadCalibrationCallback(std::string* filePath, void* userData) {
    CalibratedDevice<PSEyeOCVVideoDevice>* calibDev = (CalibratedDevice<PSEyeOCVVideoDevice>*) userData;
    (*calibDev).loadParameters(*filePath);
}
void saveCalibrationCallback(std::string* filePath, void* userData) {
    CalibratedDevice<PSEyeOCVVideoDevice>* calibDev = (CalibratedDevice<PSEyeOCVVideoDevice>*) userData;
    if ((*calibDev).checkIfCalibrated() ) {
        (*calibDev).saveParameters(*filePath);
    }
    std::cout << "Unable to save, not calibrated" << std::endl;
}

int main(int argc, const char * argv[]) {
    
    
    
    // Setup input device
    //PSEyeOCVVideoDevice dev = PSEyeOCVVideoDevice(0);

    
    
    //ImageView view = ImageView("test", 100, 100, 640, 480);
    //ImageView view2 = ImageView("test Binary",100,580,640,480);
    //view.setMouseDownCallback(mouseCallback,&img);
    
    SettingsView settView = SettingsView("This is a test", 0, 0, 1280, 720, &sync.bracketSize);
    
    SDL_Thread *improcThread = SDL_CreateThread(main_imageprocessing, "image_processing", (void*) &settView);
    
    while (settView.hasEnded()) {
        settView.runForThisFrame(640, 480, &sync);
    }
    sync.stopProgram = true;
    
    
    
    
    
    
    
    
    return 0;
}


static int main_imageprocessing(void* data) {
    
    static PSEyeOCVVideoDevice dev = PSEyeOCVVideoDevice();
    static CalibratedDevice<PSEyeOCVVideoDevice> calibDev(dev);
    cv::UMat img;
    cv::UMat binImg;
    SettingsView* settView = (SettingsView*) data;
    
    unsigned char* imgData = NULL;
    unsigned char* binData = NULL;
    
    (*settView).setMouseCallbackOnBGRImage(mouseCallback, &img);
    (*settView).setLoadCalibrationCallback(loadCalibrationCallback,&calibDev);
    (*settView).setSaveCalibrationCallback(saveCalibrationCallback,&calibDev);
    
    std::vector<float> result;
    bool cameraNotSet = true;
    size_t numberOfAvailableDevices = 0;
    
    while (!sync.stopProgram) {
        if (cameraNotSet) {
            numberOfAvailableDevices = PSEyeOCVVideoDevice::getNumberOfAvailableDevices();
        }
        if (numberOfAvailableDevices != 0 && !sync.startCalibration) {
            if (cameraNotSet) {
                cameraNotSet = false;
                dev.setDeviceIndexNotInit(0);
                //dev.setExposure(150);
                //dev.setGain(63);
                dev.cam->setAutogain(true);
                dev.cam->setExposure(150);
            }
            dev >> img;
            cv::medianBlur(img, img, 7);
            cv::inRange(img, centerColor - cv::Vec3b(sync.bracketSize,sync.bracketSize,sync.bracketSize), centerColor + cv::Vec3b(sync.bracketSize,sync.bracketSize,sync.bracketSize), binImg);
            //cv::Mat img2 = binImg.getMat(cv::ACCESS_READ);
            cv::morphologyEx(binImg, binImg, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(7,7)));
            cv::morphologyEx(binImg, binImg, cv::MORPH_CLOSE, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15,15)));
            cv::Mat img2 = binImg.getMat(cv::ACCESS_READ);
            
            
            
            HorizontalThinningAlgorithm algo = HorizontalThinningAlgorithm(img2);
            algo.compute();
            sync.result = algo.getResult();
            imgData = img.getMat(cv::ACCESS_READ).data;
            binData = binImg.getMat(cv::ACCESS_READ).data;
            sync.bgrTex = bgrImageToTexture(imgData, 640, 480);
            sync.grayTex = grayscaleImageToTexture(binData, 640, 480);
            
        } else if (sync.startCalibration) {
            static bool beganCalibration = false;
            if (beganCalibration) {
                calibDev.beginCalibration(sync.calibParams.boardNumOfSquaresInWidth, sync.calibParams.boardNumOfSquaresInHeight, sync.calibParams.squareSizeInM, sync.calibParams.imageSize);
                beganCalibration = true;
            }
            if (sync.lastFrameIsValid) {
                if (sync.useLastFrame) {
                    calibDev.useLastValidFrameForCalibration();
                }
            } else {
                calibDev >> img;
                sync.lastFrameIsValid = calibDev.checkOneFrame(img);
                sync.calibImgTex = bgrImageToTexture(img.getMat(cv::ACCESS_READ).data, 640, 480);
            }
            
            if (sync.finalizeCalibration) {
                calibDev.finalizeCalibration();
                sync.finalizeCalibration = false;
                sync.startCalibration = false;
            }
            
        }
        
        
        //        std::ostringstream oss;
        //
        //        if (!result.empty())
        //        {
        //            // Convert all but the last element to avoid a trailing ","
        //            std::copy(result.begin(), result.end()-1,
        //                      std::ostream_iterator<int>(oss, ","));
        //
        //            // Now add the last element with no delimiter
        //            oss << result.back();
        //        }
        //
        //        const char* str = oss.str().c_str();
        
        //SDL_Event e;
        //SDL_PollEvent(&e);
        //settView.runForThisFrame(e, img.getMat(cv::ACCESS_READ).data, binImg.getMat(cv::ACCESS_READ).data, 640, 480);
        
        
        
    }
    
    return 0;
}

