#include "heatmaprenderer.h"
#include <algorithm>
#include <cmath>

static inline double clamp01(double x) {
    if (x < 0.0) return 0.0;
    if (x > 1.0) return 1.0;
    return x;
}

// 连续色阶 (Plasma-like)
QRgb HeatMapRenderer::mapToColor(double v01) const
{
    v01 = clamp01(v01);

    // 定义关键帧颜色 (Stop points)
    struct Stop { double p, r, g, b; };
    static const Stop stops[] = {
        {0.00, 0.05, 0.03, 0.53}, // 深蓝
        {0.25, 0.36, 0.07, 0.67}, // 紫
        {0.50, 0.66, 0.18, 0.47}, // 紫红
        {0.75, 0.91, 0.47, 0.20}, // 橙
        {1.00, 0.99, 0.94, 0.14}  // 黄
    };

    // 找到当前值所在的区间 [a, b]
    int k = 0;
    while (k < 4 && v01 > stops[k+1].p) k++;

    const Stop& a = stops[k];
    const Stop& b = stops[k+1];

    // 线性插值
    double t = (v01 - a.p) / (b.p - a.p);
    double r = a.r + t * (b.r - a.r);
    double g = a.g + t * (b.g - a.g);
    double bb = a.b + t * (b.b - a.b);

    auto to255 = [](double x){
        int v = (int)std::lround(x * 255.0);
        return std::max(0, std::min(255, v));
    };

    return qRgb(to255(r), to255(g), to255(bb));
}

QImage HeatMapRenderer::render(const std::vector<double>& q, int nx, int ny, bool autoScale)
{
    double qmin = 0.0;
    double qmax = 1.0;

    // 如果启用了自动缩放，则寻找最大值（通常不建议用于物理分析，因为颜色会闪烁）
    if (autoScale && !q.empty()) {
        qmax = 0.0;
        for (double v : q) qmax = std::max(qmax, v);
        if (qmax < 1e-12) qmax = 1.0;
    }
    // 如果 autoScale == false，则强制使用 [0.0, 1.0] 的绝对范围
    // 这样浓度低就是暗色，浓度高就是亮色，不会随时间变化

    // 保存范围供 Widget 绘图例使用
    lastMin_ = qmin;
    lastMax_ = qmax;

    // 生成图像
    QImage img(ny, nx, QImage::Format_RGB32);

    for (int i = 0; i < nx; ++i) {
        for (int j = 0; j < ny; ++j) {
            double val = q[i*ny + j];
            // 归一化到 0..1 用于查表
            double v01 = (val - qmin) / (qmax - qmin);
            img.setPixel(j, i, mapToColor(v01));
        }
    }
    return img;
}
