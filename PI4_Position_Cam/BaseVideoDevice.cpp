//
//  BaseVideoDevice.cpp
//  PI4_Position_Cam
//
//  Created by Cedric Leblond Menard on 17-03-16.
//  Copyright © 2017 AER4875. All rights reserved.
//

#include "BaseVideoDevice.hpp"

BaseVideoDevice &operator>>(BaseVideoDevice &input, cv::InputOutputArray &output) {
    input.getImage(output);
    return input;
}
