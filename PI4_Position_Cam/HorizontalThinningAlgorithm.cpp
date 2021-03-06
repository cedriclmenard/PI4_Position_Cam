//
//  HorizontalThinningAlgorithm.cpp
//  PI4_Position_Cam
//
//  Created by Cedric Leblond Menard on 17-03-18.
//  Copyright © 2017 AER4875. All rights reserved.
//

#include "HorizontalThinningAlgorithm.hpp"

// MARK: Supporting functions

std::vector<std::tuple<unsigned int, float>> nonZeroElementsOfVectorResultTuple(std::vector<float> &result) {
    std::vector<std::tuple<unsigned int, float>> resultNonNull;
    for (auto iter = result.begin(); iter < result.end(); iter++) {
        if ((*iter) != 0) {
            resultNonNull.emplace_back((*iter), iter - result.begin());
        }
    }
    return resultNonNull;
}

std::vector<cv::Point2f> nonZeroElementsOfVectorResult(std::vector<float> &result) {
    std::vector<cv::Point2f> resultNonNull;
    for (auto iter = result.begin(); iter < result.end(); iter++) {
        if ((*iter) != 0) {
            resultNonNull.emplace_back((*iter), iter - result.begin());
        }
    }
    return resultNonNull;
}


// MARK: Class definitions

struct ParallelHorizontalThinning : public cv::ParallelLoopBody {
    cv::Mat _binImg;
    size_t _stepSize;
    size_t _cols;
    unsigned char* _data;
    //std::shared_ptr<double> _points;
    float* _points;
    
    ParallelHorizontalThinning(cv::Mat binImg, float *points) :
    _binImg(binImg), _stepSize(binImg.step1(0)), _cols(binImg.size[1]), _data(binImg.data), _points(points)
    {
        //_points.reset(new double[binImg.cols], std::default_delete<double[]>());
        //_points = new float[binImg.rows];  // Non-managed, because it is automatically managed when creating
        // creating a vector from this pointer (see HorizontalThinningAlgorithm::compute() )
    }
    
    ~ParallelHorizontalThinning() {
        
    }
    
    virtual void operator()(const cv::Range& range) const {
        // Range max is number of rows
        for (auto i = range.start; i <= range.end; i++) {
            unsigned char* start = _data + i*_stepSize;
            unsigned int j = 0;
            
            float avgPosition = 0;
            unsigned int avgCount = 0;
            while (j < _cols) {
                while (start[j] == 0) {
                    j++;
                    if (j >= _cols) {
                        break;
                    }
                }
                if (j < _cols) {
                    while (start[j] != 0 && j < _cols) {
                        avgPosition += j;
                        avgCount++;
                        j++;
                    }
                }
                if (avgCount != 0) {
                    avgPosition /= avgCount;
                } else {
                    avgPosition = 0;
                }
                break;
            }
            
            // Merge back from all threads. Should not need any mutexes, as no single address
            // space is written at the same time
            //std::mutex mtx;
            //mtx.lock();
            
            //_points.get()[i] = avgPosition;
            _points[i] = avgPosition;
            
            //mtx.unlock();
        }
    }
};

HorizontalThinningAlgorithm::HorizontalThinningAlgorithm(cv::Mat binImg) :
_img(binImg)
{
    
}

void HorizontalThinningAlgorithm::compute() {
    float points[_img.rows];
    ParallelHorizontalThinning paraHorizThin(_img, points);
    cv::Range range = cv::Range(0, _img.rows-1);
    cv::parallel_for_(range, paraHorizThin);
    _points.assign(paraHorizThin._points, paraHorizThin._points + _img.rows);
}

std::vector<float> HorizontalThinningAlgorithm::getResult() {
    //std::vector<double> result(_points.get(), _points.get() + _img.cols);
    
    return _points;
}

std::vector<std::tuple<unsigned int, float>> HorizontalThinningAlgorithm::getResultNonZeroTuple() {
    return nonZeroElementsOfVectorResultTuple(_points);
}

std::vector<cv::Point2f> HorizontalThinningAlgorithm::getResultNonZero() {
    return nonZeroElementsOfVectorResult(_points);
}

