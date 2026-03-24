#ifndef HEATMAPWIDGET_H
#define HEATMAPWIDGET_H


#include <QWidget>
#include <QImage>
#include <vector>
#include "chimney.h"

class HeatMapWidget : public QWidget
 {
    protected:
    void paintEvent(QPaintEvent*) override;


    
    private:

        QImage image_;
        std::vector<Chimney> chimneys_;
        double u_ = 0.0, w_ = 0.0;

    
        double vmin_ = 0.0;
        double vmax_ = 1.0;


    public:

    //Constructeur
        explicit HeatMapWidget(QWidget* parent=nullptr);

        void setImage(const QImage& img);

    
        void setChimneys(const std::vector<Chimney>& c) //copie
        { chimneys_ = c; update(); }


        void setWind(double horizontal, double vertical) 
        { u_ = horizontal; w_ = vertical; update(); }

        void setColorScale(double valeurmin, double valeurmax) 
        { vmin_ = valeurmin; vmax_ = valeurmax; update(); }


};


#endif HEATMAPWIDGET_H