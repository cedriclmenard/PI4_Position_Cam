//
//  VerticalThinningAlgorithm.cpp
//  PI4_Position_Cam
//
//  Created by Cedric Leblond Menard on 17-03-18.
//  Copyright © 2017 AER4875. All rights reserved.
//

#include "VerticalThinningAlgorithm.hpp"


struct ParallelVerticalThinning : public cv::ParallelLoopBody {
    cv::Mat _binImg;
    size_t _stepSize;
    size_t _ySize;
    unsigned char* _data;
    //std::shared_ptr<double> _points;
    float* _points;
    
    ParallelVerticalThinning(cv::Mat &binImg) :
    _binImg(binImg), _stepSize(binImg.step1()), _ySize(binImg.size[0]*binImg.step1()), _data(binImg.data)
    {
        //_points.reset(new double[binImg.cols], std::default_delete<double[]>());
        _points = new float[binImg.cols];  // Non-managed, because it is automatically managed when creating
        // creating a vector from this pointer (see VerticalThinningAlgorithm::compute() )
    }
    
    ~ParallelVerticalThinning() {
        
    }
    
    virtual void operator()(const cv::Range& range) const {
        // Range max is number of cols
        for (auto i = range.start; i < range.end; i++) {
            unsigned char* start = _data + i;
            unsigned int j = 0;
            
            float avgPosition = 0;
            unsigned int avgCount = 0;
            while (j*_stepSize < _ySize) {
                while (start[j*_stepSize] == 0) {
                    if (j*_stepSize >= (_ySize-_stepSize)) {
                        break;
                    } else {
                        j++;
                    }
                }
                while (start[j*_stepSize] != 0 && j*_stepSize < _ySize) {
                    avgPosition += j;
                    avgCount++;
                    j++;
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

VerticalThinningAlgorithm::VerticalThinningAlgorithm(cv::Mat &binImg) :
_img(binImg)
{
    
}

void VerticalThinningAlgorithm::compute() {
    ParallelVerticalThinning paraVertThin(_img);
    cv::Range range = cv::Range(0, _img.cols-1);
    cv::parallel_for_(range, paraVertThin);
    _points.assign(paraVertThin._points, paraVertThin._points + _img.cols);
}

std::vector<float> VerticalThinningAlgorithm::getResult() {
    //std::vector<double> result(_points.get(), _points.get() + _img.cols);
    
    return _points;
}
