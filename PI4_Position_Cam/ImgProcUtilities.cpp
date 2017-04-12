//
//  ImgProcUtilities.cpp
//  PI4_Position_Cam
//
//  Created by Cedric Leblond Menard on 17-03-26.
//  Copyright © 2017 AER4875. All rights reserved.
//

#include "ImgProcUtilities.hpp"

// MARK: Debug functions

std::string type2str(int type) {
    std::string r;
    
    uchar depth = type & CV_MAT_DEPTH_MASK;
    uchar chans = 1 + (type >> CV_CN_SHIFT);
    
    switch ( depth ) {
        case CV_8U:  r = "8U"; break;
        case CV_8S:  r = "8S"; break;
        case CV_16U: r = "16U"; break;
        case CV_16S: r = "16S"; break;
        case CV_32S: r = "32S"; break;
        case CV_32F: r = "32F"; break;
        case CV_64F: r = "64F"; break;
        default:     r = "User"; break;
    }
    
    r += "C";
    r += (chans+'0');
    
    return r;
}

void BackprojectTransformation::initComputeReference(std::vector<cv::Point2f> &imagePoints, float wingSize, cv::InputArray cameraMatrix) {
    _cameraMatrix = cameraMatrix.getMat();
    _R.create(3, 3, CV_32F);
    cv::Mat rvec;
    computePNPReferenceTransformation(imagePoints, wingSize, _cameraMatrix, rvec, _T);
    cv::Rodrigues(rvec, _R);
}

void BackprojectTransformation::backproject2Dto3D(std::vector<cv::Point2f> &imagePoints, std::vector<cv::Point3f> &outputPoints) {
    outputPoints.clear();
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
    
    cv::solvePnP(objectPoints, imagePoints, cameraMatrix, cv::Mat(), rvec, tvec);
    
    
    
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

void backproject2Dto3DFixedZ(std::vector<cv::Point2f> &imagePoints, std::vector<cv::Point3f> &outputPoints, cv::InputArray _cameraMatrix, cv::InputArray _R, cv::InputArray _tvec) {
    
    outputPoints.reserve(imagePoints.size());
    
    cv::Mat R = _R.getMat();
    cv::Mat tvec = _tvec.getMat();
    cv::Mat CM = _cameraMatrix.getMat();
    
    float z_inter = tvec.at<double>(2);
    float fx = CM.at<double>(0,0);
    float fy = CM.at<double>(1,1);
    float cx = CM.at<double>(0,2);
    float cy = CM.at<double>(1,2);
    
    // TODO: Could use some more low-level and parallelism, but hey, I'm short on time.
    
    for (auto iter = imagePoints.begin(); iter < imagePoints.end(); iter++) {
        cv::Point3d xyz = cv::Point3d((iter->x - cx)/fx*z_inter,
                                      (iter->y - cy)/fy*z_inter,
                                      z_inter);
        cv::Mat XYZ = R.t() * cv::Mat(xyz).reshape(1,3) - tvec;
        outputPoints.emplace_back(XYZ);
    }
    
}
