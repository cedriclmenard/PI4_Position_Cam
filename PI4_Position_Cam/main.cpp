//
//  main.cpp
//  PI4_Position_Cam
//
//  Created by Cedric Leblond Menard on 17-02-17.
//  Copyright © 2017 AER4875. All rights reserved.
//

#include <iostream>
#include "opencv2/core.hpp"
#include "ps3eye.h"


int main(int argc, const char * argv[]) {
    //PS3EYECam::PS3EYERef eye;
    const std::vector<ps3eye::PS3EYECam::PS3EYERef> devices = ps3eye::PS3EYECam::getDevices(false);
    // insert code here...
    std::cout << "Hello, World!\n";
    return 0;
}
