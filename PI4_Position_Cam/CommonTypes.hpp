//
//  CommonTypes.hpp
//  PI4_Position_Cam
//
//  Created by Cedric Leblond Menard on 17-03-24.
//  Copyright © 2017 AER4875. All rights reserved.
//

#ifndef CommonTypes_hpp
#define CommonTypes_hpp

#include <stdio.h>
#include <opencv2/core/core.hpp>
#include "SDL2/SDL_opengl.h"

struct CalibrationParameters {
    
    int boardNumOfSquaresInWidth = 9;
    int boardNumOfSquaresInHeight = 6;
    float squareSizeInM = 0.0225;
    cv::Size imageSize = cv::Size(480,640);
    
};

struct SyncThreadsParameters {
    // MARK: Set by image processing thread
    bool newImagesAvailable = false;
    unsigned char* bgrPtr = NULL;
    unsigned char* grayPtr = NULL;
    
    bool newResultsAreAvailable = false;
    std::vector<float> result;
    
    // For calibration
    bool lastFrameIsValid = false;
    unsigned char* calibImgPtr = NULL;
    
    // MARK: Set by GUI processing thread (main)
    bool startCalibration = false;
    bool useLastFrame = false;
    bool finalizeCalibration = false;
    bool stopProgram = false;
    
    // For calibration
    CalibrationParameters calibParams;
    
    // Calib parameters
    std::string savePath;
    std::string loadPath;
    bool toSave = false;
    bool toLoad = false;
    
    
    
    
    // For processing
    int bracketSize = 20;
    
    
    
};



#ifdef DEBUG

//std::string debugResult(std::vector<std::tuple<unsigned int, float>> &result) {
//    std::ostringstream oss;
//
//    for (auto iter = result.begin(); iter < result.end() - 1; iter++) {
//        oss << std::get<0>(*iter) << "," << std::get<1>(*iter) << ",";
//    }
//    oss << std::get<0>(*result.end()) << "," << std::get<1>(*result.end());
//
//    return oss.str();
//
//};

#endif



#endif /* CommonTypes_hpp */

