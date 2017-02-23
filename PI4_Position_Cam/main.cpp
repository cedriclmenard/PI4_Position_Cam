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



void CallBackMouse(int event, int x, int y,  int flags, void* userdata) {
    if (event == CV_EVENT_LBUTTONDOWN) {
        cv::Vec3b s = (*((cv::Mat*)userdata)).at<cv::Vec3b>(y,x);
        std::cout << "BGR is : " << +s[0] << ", " << +s[1] << ", " << +s[2] << std::endl;
    }
}

int main(int argc, const char * argv[]) {
    
    
    //PSEyeOCVVideoDevice * dev = new PSEyeOCVVideoDevice(0);
    PSEyeOCVVideoDevice dev = PSEyeOCVVideoDevice(0);
    cv::UMat img;
    cv::UMat binImg;
    dev.setExposure(220);
    cv::namedWindow("test");
    cv::setMouseCallback("test", CallBackMouse,&img);
    
    while (cv::waitKey(17)) {
        dev >> img;
        //cv::medianBlur(img, img, 7);
        cv::inRange(img, cv::Scalar(0,0,125), cv::Scalar(100,100,255), binImg);
        cv::imshow("test",img);
        cv::imshow("binImg", binImg);
    }
    
    //cv::waitKey();
    
    //delete dev;
    
    
    
    return 0;
}
