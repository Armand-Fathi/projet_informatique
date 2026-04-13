#include "wind.h"

void Wind::set(double speed, double angleDeg)
{
    speed_ = speed;
    angleDeg_ = angleDeg;

    const double pi = 3.14159265358979323846;
    const double rad = angleDeg_ * pi / 180.0;

    u_ = speed_ * std::cos(rad);
    w_ = speed_ * std::sin(rad);
}
