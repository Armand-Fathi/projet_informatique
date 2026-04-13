#pragma once
#include <QWidget>
#include <QImage>
#include <vector>
#include "chimney.h"

class HeatMapWidget : public QWidget {
    Q_OBJECT
public:
//Constructeur
    explicit HeatMapWidget(QWidget* parent=nullptr);

    void setImage(const QImage& img);

 
    void setChimneys(const std::vector<Chimney>& c) { chimneys_ = c; update(); }


    void setWind(double u, double w) { u_ = u; w_ = w; update(); }

    void setColorScale(double vmin, double vmax) { vmin_ = vmin; vmax_ = vmax; update(); }

protected:
    void paintEvent(QPaintEvent*) override;

private:
    QImage image_;
    std::vector<Chimney> chimneys_;
    double u_ = 0.0, w_ = 0.0;

   
    double vmin_ = 0.0;
    double vmax_ = 1.0;
};
