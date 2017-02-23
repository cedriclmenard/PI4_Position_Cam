//
//  Calibrator.cpp
//  PI4_Position_Cam
//
//  Created by Cedric Leblond Menard on 17-02-21.
//  Copyright © 2017 AER4875. All rights reserved.
//

#include "Calibrator.hpp"

// MARK: Supporting functions

std::vector<cv::Point3f> Create3DChessboardCorners(cv::Size boardSize, float squareSize)
{
    // This function creates the 3D points of your chessboard in its own coordinate system
    
    std::vector<cv::Point3f> corners;
    
    for( int i = 0; i < boardSize.height; i++ )
    {
        for( int j = 0; j < boardSize.width; j++ )
        {
            corners.push_back(cv::Point3f(float(j*squareSize),
                                          float(i*squareSize), 0));
        }
    }
    
    return corners;
}

template <class VideoDevice>
Calibrator<VideoDevice>::Calibrator(VideoDevice dev) :
_dev(dev)
{
    
}

template <class VideoDevice>
void Calibrator<VideoDevice>::calibrate(unsigned int numberOfFrames, int boardNumOfSquaresInWidth, int boardNumOfSquaresInHeight, float squareSizeInM)
{
    
    cv::Size boardSize = cv::Size(boardNumOfSquaresInWidth, boardNumOfSquaresInHeight);
    
    std::vector<std::vector<cv::Point3f> > objectPoints;
    std::vector<std::vector<cv::Point2f> > imagePoints;
    std::vector<cv::Point2f> corners;
    
    std::vector<cv::Point3f> obj;
    
    obj = Create3DChessboardCorners(boardSize, squareSizeInM);
    
    cv::Mat img, gray;
    
    int success = 0, k = 0;
    bool found = false;
    
    while (success < numberOfFrames)
    {
        _dev >> img;
        
        cv::cvtColor(img, gray, CV_BGR2GRAY);
        
        found = cv::findChessboardCorners(img, boardSize, corners, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FILTER_QUADS | cv::CALIB_CB_NORMALIZE_IMAGE);
        
        if (found)
        {
            cornerSubPix(gray, corners, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
            drawChessboardCorners(img, boardSize, corners, found);
        }
        
        imshow("Calibration Image", img);
        
        
        k = cv::waitKey(10);
        if (found)
        {
            k = cv::waitKey(0);
        }
        if (k == 27)
        {
            break;
        }
        if (k == ' ' && found !=0)
        {
            imagePoints.push_back(corners);
            objectPoints.push_back(obj);
            success++;
            
            if (success >= numberOfFrames)
            {
                break;
            }
        }
    }
    
    cv::destroyAllWindows();
    
    cv::Mat CM = cv::Mat(3, 3, CV_64FC1);
    cv::Mat D;
    cv::Mat R, T;
    
    // MARK: Calibrate Camera
    calibrateCamera(objectPoints, imagePoints, img.size(), CM, D, R, T, CV_CALIB_RATIONAL_MODEL);
    
    cv::FileStorage fs1("calibration.yml", cv::FileStorage::WRITE);
    fs1 << "CM" << CM;
    fs1 << "D" << D;
    fs1 << "R" << R;
    fs1 << "T" << T;
    
    fs1.release();

}


