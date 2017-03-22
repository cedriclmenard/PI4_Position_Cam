//
//  CalibratedDevice.hpp
//  PI4_Position_Cam
//
//  Created by Cedric Leblond Menard on 17-02-21.
//  Copyright © 2017 AER4875. All rights reserved.
//

#ifndef CalibratedDevice_hpp
#define CalibratedDevice_hpp

#include <stdio.h>
#include "opencv2/opencv.hpp"
#include "ImageView.hpp"
#include "BaseVideoDevice.hpp"

template <class VideoDevice>
class CalibratedDevice : BaseVideoDevice {
    VideoDevice _dev;
    cv::Mat _CM, _D, _R, _T, _map1, _map2;
    double _alpha = 0;
    virtual void getImage(cv::OutputArray img);
    
public:
    CalibratedDevice(VideoDevice dev);
    void calibrate(ImageView &view, unsigned int numberOfFrames, int boardNumOfSquaresInWidth, int boardNumOfSquaresInHeight, float squareSizeInM);
    void saveParameters(std::string filename);
    void loadParameters(std::string filename);
    void correctImage(cv::InputArray &input, cv::OutputArray &output);
    void setAlpha(double alpha);
    
    // MARK: Updated calibration procedure
    void beginCalibration(int boardNumOfSquaresInWidth, int boardNumOfSquaresInHeight, float squareSizeInM, cv::Size imageSize);
    bool checkOneFrame(cv::InputOutputArray &inputOutput);
    //void getLastValidFrame(cv::OutputArray &output);
    void useLastValidFrameForCalibration();
    void finalizeCalibration();
    
    
};

#endif /* CalibratedDevice_hpp */
