#pragma once
#include <vector>

class AdvectionSolver
{
public:
    void initialize(int nx, int ny, double dx, double dy);


    // 核心：upwind 推进一步（输入 q，输出到内部 tmp 再 swap 回 q）
    void stepUpwind(std::vector<double>& q,
                    int nx, int ny,
                    double dx, double dy,
                    double u, double w,
                    double dt);

private:
    std::vector<double> tmp_;
    inline int idx(int i, int j, int ny) const { return i * ny + j; }
};
