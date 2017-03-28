//
//  ImgProcUtilities.cpp
//  PI4_Position_Cam
//
//  Created by Cedric Leblond Menard on 17-03-26.
//  Copyright © 2017 AER4875. All rights reserved.
//

#include "ImgProcUtilities.hpp"

void BackprojectTransformation::initComputeReference(std::vector<cv::Point2f> &imagePoints, float wingSize, cv::InputArray cameraMatrix) {
    _cameraMatrix = cameraMatrix.getMat();
    _R.create(3, 3, CV_32F);
    cv::Mat rvec;
    computePNPReferenceTransformation(imagePoints, wingSize, _cameraMatrix, rvec, _T);
    cv::Rodrigues(rvec, _R);
}

void BackprojectTransformation::backproject2Dto3D(std::vector<cv::Point2f> &imagePoints, std::vector<cv::Point3f> &outputPoints) {
    backproject2Dto3DFixedZ(imagePoints, outputPoints, _cameraMatrix, _R, _T);
}






// MARK: Supporting functions

void computePNPReferenceTransformation(std::vector<cv::Point2f> &imagePoints, float wingSize, cv::InputArray cameraMatrix, cv::OutputArray rvec, cv::OutputArray tvec) {
    // For camera perspective : x is going left, y is going up and z is going toward the wing
    size_t imagePointsSize = imagePoints.size();
    float increment = wingSize/(imagePointsSize-1);
    std::vector<cv::Point3f> objectPoints;
    objectPoints.reserve(imagePointsSize);
    
    
    // Build object points vector
    for (int i = imagePointsSize - 1; i >= 0; i--) {
        objectPoints.emplace_back(0, i*increment,0);
    }
    
    cv::solvePnP(objectPoints, imagePoints, cameraMatrix, NULL, rvec, tvec);
    
    
    
}

void backproject2Dto3DFixedZ(std::vector<cv::Point2f> &imagePoints, std::vector<cv::Point3f> &outputPoints, float Z, cv::InputArray cameraMatrix, cv::InputArray R, cv::InputArray tvec) {
    
    outputPoints.reserve(imagePoints.size());
    
    //cv::Mat R(3,3,CV_32F);
    //cv::Rodrigues(rvec, R);
    cv::Mat z_inter_mat = R.getMat().row(3) * Z + tvec.getMat().at<float>(3);
    float z_inter = *z_inter_mat.data;
    float fx = cameraMatrix.getMat().at<float>(1,1);
    float fy = cameraMatrix.getMat().at<float>(2,2);
    float cx = cameraMatrix.getMat().at<float>(1,3);
    float cy = cameraMatrix.getMat().at<float>(2,3);
    
    // TODO: Could use some more low-level and parallelism, but hey, I'm short on time.
    
    for (auto iter = imagePoints.begin(); iter < imagePoints.end(); iter++) {
        cv::Point3f xyz = cv::Point3f((iter->x - cx)/fx*z_inter,
                                      (iter->y - cy)/fy*z_inter,
                                      z_inter);
        cv::Mat XYZ = R.getMat().t() * cv::Mat(xyz) - tvec.getMat();
        outputPoints.emplace_back(XYZ);
    }
    
}

void backproject2Dto3DFixedZ(std::vector<cv::Point2f> &imagePoints, std::vector<cv::Point3f> &outputPoints, cv::InputArray cameraMatrix, cv::InputArray R, cv::InputArray tvec) {
    
    outputPoints.reserve(imagePoints.size());
    
    
    float z_inter = tvec.getMat().at<float>(3);
    float fx = cameraMatrix.getMat().at<float>(1,1);
    float fy = cameraMatrix.getMat().at<float>(2,2);
    float cx = cameraMatrix.getMat().at<float>(1,3);
    float cy = cameraMatrix.getMat().at<float>(2,3);
    
    // TODO: Could use some more low-level and parallelism, but hey, I'm short on time.
    
    for (auto iter = imagePoints.begin(); iter < imagePoints.end(); iter++) {
        cv::Point3f xyz = cv::Point3f((iter->x - cx)/fx*z_inter,
                                      (iter->y - cy)/fy*z_inter,
                                      z_inter);
        cv::Mat XYZ = R.getMat().t() * cv::Mat(xyz) - tvec.getMat();
        outputPoints.emplace_back(XYZ);
    }
    
}
