#include "heatmapwidget.h"
#include <QPainter>
#include <cmath>
#include <algorithm>

// 静态辅助函数：用于在 Widget 里绘制颜色条时生成同样的颜色
// 保持与 Renderer 一致的配色逻辑
static QRgb plasmaColor(double v01)
{
    if (v01 < 0.0) v01 = 0.0;
    if (v01 > 1.0) v01 = 1.0;

    struct Stop { double p, r, g, b; };
    static const Stop stops[] = {
        {0.00, 0.05, 0.03, 0.53},
        {0.25, 0.36, 0.07, 0.67},
        {0.50, 0.66, 0.18, 0.47},
        {0.75, 0.91, 0.47, 0.20},
        {1.00, 0.99, 0.94, 0.14}
    };

    int k = 0;
    while (k < 4 && v01 > stops[k+1].p) k++;
    const Stop& a = stops[k];
    const Stop& b = stops[k+1];

    double t = (v01 - a.p) / (b.p - a.p);
    double r = a.r + t * (b.r - a.r);
    double g = a.g + t * (b.g - a.g);
    double bb = a.b + t * (b.b - a.b);

    auto to255 = [](double x){ return std::max(0, std::min(255, (int)std::lround(x * 255.0))); };
    return qRgb(to255(r), to255(g), to255(bb));
}

HeatMapWidget::HeatMapWidget(QWidget* parent) : QWidget(parent)
{
    setMinimumSize(600, 550); //稍微加宽一点给ColorBar
}

void HeatMapWidget::setImage(const QImage& img)
{
    image_ = img;
    update();
}

void HeatMapWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.fillRect(rect(), Qt::white); // 白色背景

    // --- 1. 布局定义 ---
    int leftMargin = 60;   // 左侧：物理X轴刻度
    int bottomMargin = 40; // 底部：物理Y轴刻度
    int topMargin = 20;    // 顶部：防止文字切断
    int rightMargin = 80;  // 右侧：留给颜色条 (Color Bar)

    // 热力图绘图区域
    QRect plotRect(leftMargin, topMargin,
                   width() - leftMargin - rightMargin,
                   height() - topMargin - bottomMargin);

    // 颜色条区域 (位于热力图右侧 15px 处，宽 20px)
    QRect barRect(plotRect.right() + 15, plotRect.top(), 20, plotRect.height());

    // --- 2. 绘制热力图 ---
    if (!image_.isNull()) {
        p.drawImage(plotRect, image_);
    } else {
        p.fillRect(plotRect, Qt::black);
    }
    p.setPen(Qt::black);
    p.drawRect(plotRect); // 热力图边框

    // --- 3. 绘制坐标轴 (物理坐标) ---
    p.setFont(QFont("Arial", 8));
    int numTicks = 10;

    // 垂直轴 (Screen Y = Physics X)
    for (int i = 0; i <= numTicks; ++i) {
        double val = (double)i / numTicks;
        int sy = plotRect.top() + (int)(val * plotRect.height());

        p.drawLine(plotRect.left() - 5, sy, plotRect.left(), sy);

        // 文字
        QString text = QString::number(val, 'f', 1);
        p.drawText(QRect(0, sy - 10, leftMargin - 7, 20), Qt::AlignRight | Qt::AlignVCenter, text);
    }
    // 轴标题
    p.save();
    p.translate(15, plotRect.center().y());
    p.rotate(-90);
    p.drawText(QRect(-100, -10, 200, 20), Qt::AlignCenter, "Physics X (u)");
    p.restore();

    // 水平轴 (Screen X = Physics Y)
    for (int i = 0; i <= numTicks; ++i) {
        double val = (double)i / numTicks;
        int sx = plotRect.left() + (int)(val * plotRect.width());

        p.drawLine(sx, plotRect.bottom(), sx, plotRect.bottom() + 5);

        QString text = QString::number(val, 'f', 1);
        p.drawText(QRect(sx - 15, plotRect.bottom() + 5, 30, 20), Qt::AlignHCenter | Qt::AlignTop, text);
    }
    p.drawText(QRect(plotRect.left(), height() - 20, plotRect.width(), 20), Qt::AlignCenter, "Physics Y (w)");

    // --- 4. 绘制右侧颜色条 (Legend) ---
    // 生成渐变条图像
    QImage barImg(1, 256, QImage::Format_RGB32);
    for (int y = 0; y < 256; ++y) {
        // 顶部是最大值(1.0)，底部是最小值(0.0) -> y=0对应1.0
        double v01 = 1.0 - (double)y / 255.0;
        QRgb c = plasmaColor(v01);
        barImg.setPixel(0, y, c);
    }
    p.drawImage(barRect, barImg); // 自动拉伸到 barRect 大小
    p.setPen(Qt::black);
    p.drawRect(barRect);

    // 颜色条刻度
    int barTicks = 5;
    for (int i = 0; i <= barTicks; ++i) {
        double ratio = (double)i / barTicks; // 0.0 ~ 1.0
        // 数值：底部是 vmin, 顶部是 vmax
        double val = vmin_ + ratio * (vmax_ - vmin_);

        // 屏幕Y坐标：ratio=0(底部) -> rect.bottom()
        int y = barRect.bottom() - (int)(ratio * barRect.height());

        p.drawLine(barRect.right(), y, barRect.right() + 5, y);

        QString text = QString::number(val, 'f', 1);
        p.drawText(barRect.right() + 8, y + 5, text); // 稍微向下偏移一点以便垂直居中
    }
    // 颜色条标题
    p.drawText(QRect(barRect.right() - 10, barRect.top() - 20, 60, 20), Qt::AlignLeft, "Density");


    // --- 5. 叠加层 (烟囱与风向) ---
    // 烟囱 (注意坐标映射)
    p.setPen(Qt::white);
    p.setBrush(Qt::white);
    for (const auto& c : chimneys_) {
        // Physical Y -> Screen X
        int screenX = plotRect.left() + (int)(c.y0 * plotRect.width());
        // Physical X -> Screen Y
        int screenY = plotRect.top() + (int)(c.x0 * plotRect.height());
        p.drawEllipse(QPoint(screenX, screenY), 5, 5);
    }

    // 风向箭头 (相对于 plotRect 左上角)
    {
        QPoint origin(plotRect.left() + 30, plotRect.top() + 30);
        QPoint tip(
            origin.x() + (int)(50 * w_),
            origin.y() + (int)(50 * u_)
            );
        p.setPen(QPen(Qt::white, 2));
        p.drawLine(origin, tip);
        p.setBrush(Qt::white);
        p.drawEllipse(origin, 2, 2);
    }
}
