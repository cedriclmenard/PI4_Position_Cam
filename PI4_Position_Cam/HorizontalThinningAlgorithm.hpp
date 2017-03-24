//
//  HorizontalThinningAlgorithm.hpp
//  PI4_Position_Cam
//
//  Created by Cedric Leblond Menard on 17-03-18.
//  Copyright © 2017 AER4875. All rights reserved.
//

#ifndef HorizontalThinningAlgorithm_hpp
#define HorizontalThinningAlgorithm_hpp

#include <stdio.h>
#include <opencv2/opencv.hpp>

class HorizontalThinningAlgorithm {
    cv::Mat _img;
    cv::Mat _returnImg;
    bool isComputed = false;
    
    std::vector<float> _points;
    
public:
    HorizontalThinningAlgorithm(cv::Mat binImg);
    void compute();
    std::vector<float> getResult();
};

#endif /* HorizontalThinningAlgorithm_hpp */
