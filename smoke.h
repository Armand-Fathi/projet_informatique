#pragma once
#include <vector>
#include "chimney.h"

class Smoke {
public:
    void initialize(int nx, int ny, double L, double H);
    void reset();

    // 添加一次性高斯源（初始化或手动触发）
    void addGaussian(const Chimney& c);

    // 持续排放：每帧按 dt 叠加（可选功能）
    void emitContinuous(double dt);

    const std::vector<double>& field() const { return q_; }
    std::vector<double>& field() { return q_; }

    int nx() const { return nx_; }
    int ny() const { return ny_; }
    double dx() const { return dx_; }
    double dy() const { return dy_; }

    void setChimneys(const std::vector<Chimney>& list) { chimneys_ = list; }
    const std::vector<Chimney>& chimneys() const { return chimneys_; }

private:
    int nx_ = 0, ny_ = 0;
    double L_ = 1.0, H_ = 1.0;
    double dx_ = 1.0, dy_ = 1.0;

    std::vector<double> q_; // q(i,j) 用一维存
    std::vector<Chimney> chimneys_;

    inline int idx(int i, int j) const { return i * ny_ + j; }
};
