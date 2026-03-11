#pragma once
#include <QWidget>
#include <QImage>
#include <vector>
#include "chimney.h"

class HeatMapWidget : public QWidget {
    Q_OBJECT
public:
    explicit HeatMapWidget(QWidget* parent=nullptr);

    void setImage(const QImage& img);

    // 设置烟囱数据（用于叠加显示白色圆点）
    void setChimneys(const std::vector<Chimney>& c) { chimneys_ = c; update(); }

    // 设置风速分量（用于绘制风向箭头）
    void setWind(double u, double w) { u_ = u; w_ = w; update(); }

    // [新增] 设置颜色条的数值范围（从 Controller 传入）
    void setColorScale(double vmin, double vmax) { vmin_ = vmin; vmax_ = vmax; update(); }

protected:
    void paintEvent(QPaintEvent*) override;

private:
    QImage image_;
    std::vector<Chimney> chimneys_;
    double u_ = 0.0, w_ = 0.0;

    // [新增] 记录颜色条对应的物理数值范围
    double vmin_ = 0.0;
    double vmax_ = 1.0;
};
