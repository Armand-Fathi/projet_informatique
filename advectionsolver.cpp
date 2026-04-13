#include "advectionsolver.h"
#include <cmath>

void AdvectionSolver::initialize(int nx, int ny)
{
    tmp_.assign(nx * ny, 0.0);
    tmp2_.assign(nx * ny, 0.0);
}

void AdvectionSolver::stepUpwind(std::vector<double>& q,//柱密度
                                 int nx, int ny,

                                 double dx, double dy,
                                 double u, double w,
                                 double dt)
{
    if ((int)tmp_.size() != nx * ny) tmp_.assign(nx * ny, 0.0);

    const double ax = std::abs(u) * dt / dx;
    const double ay = std::abs(w) * dt / dy;

    for (int i = 1; i < nx - 1; ++i) { //从1到nx-2，边界(0和nx-1)交给边界限制函数处理
        for (int j = 1; j < ny - 1; ++j) {
            const double qij = q[idx(i,j,ny)];

            const double q_up_x = (u >= 0.0) ? q[idx(i-1,j,ny)] : q[idx(i+1,j,ny)];

            const double q_up_y = (w >= 0.0) ? q[idx(i,j-1,ny)] : q[idx(i,j+1,ny)];

            double next =
                (1.0 - ax - ay) * qij
                + ax * q_up_x
                + ay * q_up_y;

            if (next < 0.0) next = 0.0;
            tmp_[idx(i,j,ny)] = next;
        }
    }

    q.swap(tmp_);

}


void AdvectionSolver::stepDiffusion(std::vector<double>& q,
                                    int nx, int ny,
                                    double dx, double dy,
                                    double kappa,
                                    double dt)
{
    if (kappa <= 0.0) return;

    if ((int)tmp2_.size() != nx * ny) tmp2_.assign(nx * ny, 0.0);

    const double invdx2 = 1.0 / (dx * dx);
    const double invdy2 = 1.0 / (dy * dy);

    for (int i = 1; i < nx - 1; ++i) {
        for (int j = 1; j < ny - 1; ++j) {
            const double qij = q[idx(i,j,ny)];

            const double lap =
                (q[idx(i+1,j,ny)] - 2.0*qij + q[idx(i-1,j,ny)]) * invdx2 +
                (q[idx(i,j+1,ny)] - 2.0*qij + q[idx(i,j-1,ny)]) * invdy2;

            double next = qij + kappa * dt * lap;
            if (next < 0.0) next = 0.0;
            tmp2_[idx(i,j,ny)] = next;
        }
    }

    q.swap(tmp2_);
}
