//
//  VerticalThinningAlgorithm.hpp
//  PI4_Position_Cam
//
//  Created by Cedric Leblond Menard on 17-03-18.
//  Copyright © 2017 AER4875. All rights reserved.
//

#ifndef VerticalThinningAlgorithm_hpp
#define VerticalThinningAlgorithm_hpp

#include <stdio.h>
#include <opencv2/opencv.hpp>

class VerticalThinningAlgorithm {
    cv::Mat _img;
    cv::Mat _returnImg;
    bool isComputed = false;
    
    std::vector<double> _points;
    
public:
    VerticalThinningAlgorithm(cv::Mat &binImg);
    void compute();
    std::vector<double> getResult();
};

#endif /* VerticalThinningAlgorithm_hpp */
