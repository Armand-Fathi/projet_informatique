#include "smoke.h"
#include <algorithm>
#include <cmath>

void Smoke::initialize(int nx, int ny, double L, double H)
{
    nx_ = nx; ny_ = ny;
    L_ = L;  H_ = H;

    dx_ = L_ / (nx_ - 1);
    dy_ = H_ / (ny_ - 1);

    q_.assign(nx_ * ny_, 0.0);
}

void Smoke::reset()
{
    std::fill(q_.begin(), q_.end(), 0.0);
}

void Smoke::addGaussian(const Chimney& c)
{
    // q += A * exp(-(x-x0)^2/(2*sx^2)) * exp(-(y-y0)^2/(2*sy^2))
    for (int i = 0; i < nx_; ++i) {
        const double x = i * dx_;
        const double ex = std::exp(- (x - c.x0) * (x - c.x0) / (2.0 * c.sigmaX * c.sigmaX));
        for (int j = 0; j < ny_; ++j) {
            const double y = j * dy_;
            const double ey = std::exp(- (y - c.y0) * (y - c.y0) / (2.0 * c.sigmaY * c.sigmaY));
            q_[idx(i,j)] += c.amplitude * ex * ey;
        }
    }
}

void Smoke::emitContinuous(double dt)
{
    for (const auto& c : chimneys_) {
        if (!c.continuous || c.rate <= 0.0) continue;
        Chimney injected = c;
        injected.amplitude = c.rate * dt; // 每秒 rate，乘dt得到本帧叠加量
        addGaussian(injected);
    }
}
