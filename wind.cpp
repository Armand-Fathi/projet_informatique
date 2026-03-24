#include "wind.h"

void Wind::set(double speed, double angleDeg)
{
    speed_ = speed;
    angleDeg_ = angleDeg;

    const double pi = 3.14;
    const double rad = angleDeg_ * pi / 180.0;

    horizontal = speed_ * std::cos(rad);
    vertical = speed_ * std::sin(rad);
}
