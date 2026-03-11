#pragma once
#include <QImage>
#include <vector>

class HeatMapRenderer {
public:
    // 渲染函数
    QImage render(const std::vector<double>& q, int nx, int ny, bool autoScale = true);

    // [新增] 获取上一次渲染时使用的数值范围（用于给 UI 画刻度）
    double lastMin() const { return lastMin_; }
    double lastMax() const { return lastMax_; }

private:
    QRgb mapToColor(double v01) const;

    // [新增] 记录范围
    double lastMin_ = 0.0;
    double lastMax_ = 1.0;
};
