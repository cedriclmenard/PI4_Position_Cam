//
//  CalibratedDevice.cpp
//  PI4_Position_Cam
//
//  Created by Cedric Leblond Menard on 17-02-21.
//  Copyright © 2017 AER4875. All rights reserved.
//

#include "CalibratedDevice.hpp"

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
CalibratedDevice<VideoDevice>::CalibratedDevice(VideoDevice dev) :
_dev(dev)
{
    
}

template <class VideoDevice>
void CalibratedDevice<VideoDevice>::calibrate(ImageView &view, unsigned int numberOfFrames, int boardNumOfSquaresInWidth, int boardNumOfSquaresInHeight, float squareSizeInM)
{
    // TODO: Change waitKey
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
        
        view.showBGR(img.data, img.cols, img.rows);
        //imshow("Calibration Image", img);
        
        
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
    
    _CM = cv::Mat(3, 3, CV_64FC1);
    
    // MARK: Calibrate Camera
    calibrateCamera(objectPoints, imagePoints, img.size(), _CM, _D, _R, _T, CV_CALIB_RATIONAL_MODEL);
    
    // Get distortion correction map for speed
    cv::initUndistortRectifyMap(_CM, _D, cv::Mat(),
                                cv::getOptimalNewCameraMatrix(_CM, _D, img.size(), 1, img.size(), 0),
                                img.size(), CV_16SC2, _map1, _map2);

}

template<class VideoDevice>
void CalibratedDevice<VideoDevice>::saveParameters(std::string filename){
    cv::FileStorage fs1(filename, cv::FileStorage::WRITE);
    fs1 << "CM" << _CM;
    fs1 << "D" << _D;
    fs1 << "R" << _R;
    fs1 << "T" << _T;
    
    fs1.release();
}

template<class VideoDevice>
void CalibratedDevice<VideoDevice>::loadParameters(std::string filename){
    cv::FileStorage fs1(filename, cv::FileStorage::READ);
    fs1["CM"] >> _CM;
    fs1["D"] >> _D;
    fs1["R"] >> _R;
    fs1["T"] >> _T;
    
    fs1.release();
}

template<class VideoDevice>
void CalibratedDevice<VideoDevice>::correctImage(cv::InputArray &input, cv::OutputArray &output){
    cv::remap(input, output, _map1, _map2, cv::INTER_LINEAR);
}

template<class VideoDevice>
void CalibratedDevice<VideoDevice>::getImage(cv::OutputArray img) {
    cv::Mat _img;
    _dev >> _img;
    correctImage(_img,img);
}

// MARK: Updated calibration procedure
struct CalibrationData {
    cv::Size boardSize;
    std::vector<std::vector<cv::Point3f> > objectPoints;
    std::vector<std::vector<cv::Point2f> > imagePoints;
    std::vector<cv::Point2f> corners;
    std::vector<cv::Point3f> obj;
    cv::Size imageSize;
} calibrationData;

template<class VideoDevice>
void CalibratedDevice<VideoDevice>::beginCalibration(int boardNumOfSquaresInWidth, int boardNumOfSquaresInHeight, float squareSizeInM, cv::Size imageSize) {
    calibrationData.boardSize = cv::Size(boardNumOfSquaresInWidth, boardNumOfSquaresInHeight);
    calibrationData.obj = Create3DChessboardCorners(calibrationData.boardSize, squareSizeInM);
    calibrationData.imageSize = imageSize;
}

template<class VideoDevice>
bool CalibratedDevice<VideoDevice>::checkOneFrame(cv::InputOutputArray &input) {
    cv::Mat gray;
    cv::cvtColor(input, gray, CV_BGR2GRAY);
    
    bool found = cv::findChessboardCorners(input, calibrationData.boardSize, calibrationData.corners, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FILTER_QUADS | cv::CALIB_CB_NORMALIZE_IMAGE);
    
    if (found)
    {
        cv::cornerSubPix(gray, calibrationData.corners, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
        cv::drawChessboardCorners(input, calibrationData.boardSize, calibrationData.corners, found);
    }
    return found;
}

//template<class VideoDevice>
//void CalibratedDevice<VideoDevice>::getLastValidFrame(cv::OutputArray &output) {
//    
//}

template<class VideoDevice>
void CalibratedDevice<VideoDevice>::useLastValidFrameForCalibration() {
    calibrationData.imagePoints.push_back(calibrationData.corners);
    calibrationData.objectPoints.push_back(calibrationData.obj);
}

template<class VideoDevice>
void CalibratedDevice<VideoDevice>::finalizeCalibration() {
    _CM = cv::Mat(3, 3, CV_64FC1);
    
    // MARK: Calibrate Camera
    calibrateCamera(calibrationData.objectPoints, calibrationData.imagePoints, calibrationData.imageSize, _CM, _D, _R, _T, CV_CALIB_RATIONAL_MODEL);
    
    // Get distortion correction map for speed
    cv::initUndistortRectifyMap(_CM, _D, cv::Mat(),
                                cv::getOptimalNewCameraMatrix(_CM, _D, calibrationData.imageSize, 1, calibrationData.imageSize, 0),
                                calibrationData.imageSize, CV_16SC2, _map1, _map2);
}

