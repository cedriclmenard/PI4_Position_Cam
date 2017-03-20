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
    double* _points;
    
    ParallelVerticalThinning(cv::Mat &binImg) :
    _binImg(binImg), _stepSize(binImg.step1()), _ySize(binImg.size[1]), _data(binImg.data)
    {
        //_points.reset(new double[binImg.cols], std::default_delete<double[]>());
        _points = new double[binImg.cols];  // Non-managed, because it is automatically managed when creating
        // creating a vector from this pointer (see VerticalThinningAlgorithm::compute() )
    }
    
    ~ParallelVerticalThinning() {
        
    }
    
    virtual void operator()(const cv::Range& range) const {
        // Range max is number of cols
        for (auto i = range.start; i < range.end; i++) {
            unsigned char* start = _data + i;
            unsigned int j = 0;
            
            double avgPosition = 0;
            unsigned int avgCount = 0;
            while (j < _ySize) {
                while (start[j] == 0) {
                    j++;
                }
                while (start[j] != 0) {
                    avgPosition += j;
                    avgCount++;
                }
                avgPosition /= avgCount;
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

std::vector<double> VerticalThinningAlgorithm::getResult() {
    //std::vector<double> result(_points.get(), _points.get() + _img.cols);
    
    return _points;
}