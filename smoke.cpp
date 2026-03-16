#include "smoke.h"
#include <cmath>
using namespace std;

void Smoke::initialize( int nx1, int ny1, double l1,double h1)
{
    nx_=nx1;
    ny_=ny1;
    l_=l1;
    h_=h1;

    dx_=l_/(nx_-1);
    dy_=h_/(ny_-1);

    q_.assign(nx_*ny_, 0);
}

void Smoke::reset()
{
    fill(q_.begin(), q_.end(), 0.0);
}

void Smoke::addGaussian(const Chimney & c)
{
    static constexpr double pi=3.14;

    for(int i=0; i < nx_; i++)
    {
        double x=i*dx_;
        for( int j=0; j <ny_; j++)
        {
            double y=j*dy_;

            q_[idx(i,j)] += c.amplitude *(1/ (c.sigmaX * sqrt(2* pi)))  *
                            (1/ (c.sigmaY * sqrt(2* pi))) *
                            exp(-((x-c.x0)*(x-c.x0) / (2*c.sigmaX * c.sigmaX )))*
                            exp(-((y-c.y0)*(y-c.y0) / (2*c.sigmaY * c.sigmaY )));
        }
    }


}

void Smoke::emitContinuous(double dt)
{
    for (const auto& c : chimneys_) {
        if (!c.continuous || c.rate <= 0.0) continue;

        Chimney injected = c;
        injected.amplitude = c.rate * dt;
        addGaussian(injected);
    }
}
