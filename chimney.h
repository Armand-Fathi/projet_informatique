#pragma once

struct Chimney {
    double x0 = 0.5;
    double y0 = 0.5;
    double sigmaX = 0.04;
    double sigmaY = 0.04;
    double amplitude = 1.0;

    bool continuous = true;
    double rate = 2.0; // continuous=true 时：每秒叠加的强度
};
