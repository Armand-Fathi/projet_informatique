#include "advectionsolver.h"
#include <algorithm>
#include <cmath>

void AdvectionSolver::initialize(int nx, int ny, double /*dx*/, double /*dy*/)
{
    tmp_.assign(nx * ny, 0.0);
}

void AdvectionSolver::stepUpwind(std::vector<double>& q,
                                 int nx, int ny,
                                 double dx, double dy,
                                 double u, double w,
                                 double dt)
{
    if ((int)tmp_.size() != nx * ny) tmp_.assign(nx * ny, 0.0);
    std::fill(tmp_.begin(), tmp_.end(), 0.0); // 吸收边界：边界默认 0

    const double ax = std::abs(u) * dt / dx;
    const double ay = std::abs(w) * dt / dy;

    // 更新内部点
    for (int i = 1; i < nx - 1; ++i) {
        for (int j = 1; j < ny - 1; ++j) {
            const double qij = q[idx(i,j,ny)];

            const double q_up_x = (u >= 0.0) ? q[idx(i-1,j,ny)] : q[idx(i+1,j,ny)];
            const double q_up_y = (w >= 0.0) ? q[idx(i,j-1,ny)] : q[idx(i,j+1,ny)];

            double next =
                (1.0 - ax - ay) * qij
                + ax * q_up_x
                + ay * q_up_y;

            if (next < 0.0) next = 0.0; // 防止数值误差出现负浓度
            tmp_[idx(i,j,ny)] = next;
        }
    }

    q.swap(tmp_);
}
