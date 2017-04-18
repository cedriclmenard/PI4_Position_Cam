//
//  ImgProcUtilities.hpp
//  PI4_Position_Cam
//
//  Created by Cedric Leblond Menard on 17-03-26.
//  Copyright © 2017 AER4875. All rights reserved.
//

#ifndef ImgProcUtilities_hpp
#define ImgProcUtilities_hpp

#include <stdio.h>
#include <opencv2/opencv.hpp>

class BackprojectTransformation {
    cv::Mat _R, _T, _cameraMatrix;
    double _cameraDistance = 0.0;
    
public:
    void initComputeReference(cv::InputArray cameraMatrix, float cameraAngleFromXZPlaneRad, float cameraDistance);
    void backproject2Dto3D(std::vector<cv::Point2f> &imagePoints, std::vector<cv::Point3f> &outputPoints);
};

void computePNPReferenceTransformation(std::vector<cv::Point2f> &imagePoints, float wingSize, cv::InputArray cameraMatrix, cv::OutputArray rvec, cv::OutputArray tvec);

void computePoseFromDistanceAndCameraAngle(double cameraAngleFromXZPlaneRad, double cameraDistance, cv::OutputArray rvec, cv::OutputArray tvec);

void backproject2Dto3DFixedZ(std::vector<cv::Point2f> &imagePoints, std::vector<cv::Point3f> &outputPoints, float Z, cv::InputArray cameraMatrix, cv::InputArray rvec, cv::InputArray tvec);

void backproject2Dto3DFixedZ(std::vector<cv::Point2f> &imagePoints, std::vector<cv::Point3f> &outputPoints, cv::InputArray cameraMatrix, cv::InputArray rvec, cv::InputArray tvec);

void backproject2Dto3DFixedDistance(std::vector<cv::Point2f> &imagePoints, std::vector<cv::Point3f> &outputPoints, cv::InputArray _cameraMatrix, cv::InputArray _R, double cameraDistance);

#endif /* ImgProcUtilities_hpp */
