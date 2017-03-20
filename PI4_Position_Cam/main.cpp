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
#include "ImageView.hpp"

cv::Vec3b centerColor = cv::Vec3b(155,155,155);

//void CallBackMouse(int event, int x, int y,  int flags, void* userdata) {
//    if (event == CV_EVENT_LBUTTONDOWN) {
//        centerColor = (*((cv::UMat*)userdata)).getMat(cv::ACCESS_READ).at<cv::Vec3b>(y,x);
//        std::cout << "BGR is : " << +centerColor[0] << ", " << +centerColor[1] << ", " << +centerColor[2] << std::endl;
//    }
//}
void mouseCallback(int x, int y, void* userData) {
    centerColor = (*((cv::UMat*)userData)).getMat(cv::ACCESS_READ).at<cv::Vec3b>(y,x);
    std::cout << "BGR is : " << +centerColor[0] << ", " << +centerColor[1] << ", " << +centerColor[2] << std::endl;
}

int main(int argc, const char * argv[]) {
    
    
//    //PSEyeOCVVideoDevice * dev = new PSEyeOCVVideoDevice(0);
//    PSEyeOCVVideoDevice dev = PSEyeOCVVideoDevice(0);
//    cv::UMat img;
//    cv::UMat binImg;
//    dev.setExposure(100);
//    cv::namedWindow("test");
//    cv::setMouseCallback("test", CallBackMouse,&img);
//    
//    int key = -1;
//    while (key) {
//        dev >> img;
//        cv::medianBlur(img, img, 7);
//        cv::inRange(img, centerColor - cv::Vec3b(20,20,20), centerColor + cv::Vec3b(20,20,20), binImg);
//        cv::imshow("test",img);
//        cv::imshow("binImg", binImg);
//        key = cv::waitKey(17);
//    }
//    
//    //cv::waitKey();
//    
//    //delete dev;
    
    PSEyeOCVVideoDevice dev = PSEyeOCVVideoDevice(0);
    cv::UMat img;
    cv::UMat binImg;
    dev.setExposure(150);
    dev.setGain(63);
    
    ImageView view = ImageView("test", 100, 100, 640, 480);
    ImageView view2 = ImageView("test Binary",100,580,640,480);
    view.setMouseDownCallback(mouseCallback,&img);
    
    
    
    while (view.waitKeyAndProcessEvent(10) < 0) {
        dev >> img;
        cv::medianBlur(img, img, 7);
        cv::inRange(img, centerColor - cv::Vec3b(20,20,20), centerColor + cv::Vec3b(20,20,20), binImg);
        //cv::imshow("test",img);
        //cv::imshow("binImg", binImg);
        view.showBGR(img.getMat(cv::ACCESS_READ).data, 640, 480);
        view2.showGrayscale(binImg.getMat(cv::ACCESS_READ).data, 640, 480);
    }
    
    
    return 0;
}
