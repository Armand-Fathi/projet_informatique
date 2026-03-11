#pragma once
#include <cmath>

class Wind {
public:
    void set(double speed, double angleDeg);
    double u() const { return u_; }
    double w() const { return w_; }
    double speed() const { return speed_; }
    double angleDeg() const { return angleDeg_; }

private:
    double speed_ = 0.35;
    double angleDeg_ = 180.0;
    double u_ = 0.0;
    double w_ = 0.0;
};
