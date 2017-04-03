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
#include "ImgProcUtilities.hpp"

cv::Vec3b centerColor = cv::Vec3b(155,155,155);

// Forward declaration
static int main_imageprocessing(void* data);

SyncThreadsParameters sync;


void mouseCallback(int x, int y, void* userData) {
    centerColor = (*((cv::UMat*)userData)).getMat(cv::ACCESS_READ).at<cv::Vec3b>(y,x);
    //centerColor = (*(cv::Mat*)userData).at<cv::Vec3b>(y,x);
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
    
    SettingsView settView = SettingsView("AER8475 - Camera measurements", 0, 0, 1280, 720, &sync.bracketSize);
    
    SDL_Thread *improcThread = SDL_CreateThread(main_imageprocessing, "image_processing", (void*) &settView);
    
    while (!settView.hasEnded()) {
        settView.runForThisFrame(640, 480, &sync);
    }
    sync.stopProgram = true;
    
    
    
    // Exit so created thread doesn't get owned by the OS
    SDL_WaitThread(improcThread, NULL);
    
    
    
    
    
    return 0;
}


static int main_imageprocessing(void* data) {
    
    static PSEyeOCVVideoDevice dev = PSEyeOCVVideoDevice();
    static CalibratedDevice<PSEyeOCVVideoDevice> calibDev(dev);
    cv::UMat img, imgBlur, binImg;
    //cv::Mat img, imgBlur, binImg;
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
            
            
            
            calibDev >> img;
            
            cv::medianBlur(img, imgBlur, 7);
            cv::inRange(imgBlur, centerColor - cv::Vec3b(sync.bracketSize,sync.bracketSize,sync.bracketSize), centerColor + cv::Vec3b(sync.bracketSize,sync.bracketSize,sync.bracketSize), binImg);
            //cv::Mat img2 = binImg.getMat(cv::ACCESS_READ);
            cv::morphologyEx(binImg, binImg, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(7,7)));
            cv::morphologyEx(binImg, binImg, cv::MORPH_CLOSE, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15,15)));
            cv::Mat img2 = binImg.getMat(cv::ACCESS_READ);
            imgBlur.release();
            
            
            
            HorizontalThinningAlgorithm algo = HorizontalThinningAlgorithm(img2);
            //HorizontalThinningAlgorithm algo = HorizontalThinningAlgorithm(img);
            algo.compute();
            sync.newResultsAreAvailable = false;
            sync.result = algo.getResult();
            sync.resultPoint = algo.getResultNonZero();
            sync.newResultsAreAvailable = true;
            
            // This is a wannabe mutex (without all the fuzzy OS-thingy going on)
            sync.newImagesAvailable = false;
            imgData = img.getMat(cv::ACCESS_READ).data;
            binData = binImg.getMat(cv::ACCESS_READ).data;
            //imgData = img.data;
            //binData = binImg.data;
            
            
            sync.bgrPtr = imgData;
            sync.grayPtr = binData;
            
            sync.newImagesAvailable = true;
            
            
            
            
            // MARK: Result processing (PNP)
            static BackprojectTransformation bpTsf;
            if (sync.computeReferenceForPNP && calibDev.checkIfCalibrated()) {
                bpTsf.initComputeReference(sync.resultPoint, sync.wingSize, calibDev.getCameraMat());
                sync.backprojectionReferenceIsSet = true;
                sync.computeReferenceForPNP = false;
            } else {
                sync.computeReferenceForPNP = false;
            }
            if (sync.computeBackprojection && calibDev.checkIfCalibrated()) {
                sync.backprojectionResultsAreAvailable = false;
                bpTsf.backproject2Dto3D(sync.resultPoint, sync.backprojectionResult);
                sync.backprojectionResultsAreAvailable = true;
            } else {
                sync.computeBackprojection = false;
            }
            
            
        } else if (sync.startCalibration) {
            static bool beganCalibration = false;
            if (!beganCalibration) {
                calibDev.beginCalibration(sync.calibParams.boardNumOfSquaresInWidth, sync.calibParams.boardNumOfSquaresInHeight, sync.calibParams.squareSizeInM, sync.calibParams.imageSize);
                beganCalibration = true;
            }
            if (sync.lastFrameIsValid) {
                if (sync.useLastFrame) {
                    calibDev.useLastValidFrameForCalibration();
                    sync.useLastFrame = false;
                }
            } else {
                calibDev >> img;
                sync.lastFrameIsValid = calibDev.checkOneFrame(img);
                sync.newImagesAvailable = false;
                sync.calibImgPtr = img.getMat(cv::ACCESS_READ).data;
                //sync.calibImgPtr = img.data;
                sync.newImagesAvailable = true;
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



