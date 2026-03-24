#include "heatmapwidget.h"
#include <QPainter>
#include <cmath>
#include <algorithm>

/*Dans le widget, ils doivent dessiner la barre de couleur (dégradé vertical).

Ils veulent la même palette que l’image heatmap, sinon la légende serait fausse*/
static QRgb plasmaColor(double v01) //couleur RGB
{
    if (v01 < 0.0) v01 = 0.0;
    if (v01 > 1.0) v01 = 1.0;

    struct Stop { double p, r, g, b; }; //p -> position
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



//Constructeur
HeatMapWidget::HeatMapWidget(QWidget* parent) : QWidget(parent)
{
    setMinimumSize(600, 550);  
  
  
}

void HeatMapWidget::setImage(const QImage& img)
{
    image_ = img;
    update(); //“redessine le widget” → ça déclenche paintEvent().
}

void HeatMapWidget::paintEvent(QPaintEvent*)
{
    //Créer le painter + fond blanc


    QPainter p(this); //QPainter contient toutes les fonctions de dessin



    //p.fillRect(rect(), Qt::green);  //fond du widget
    p.fillRect(rect(), QColor(255, 0, 0, 128));
   
    int leftMargin = 60;   
    int bottomMargin = 40;
    int topMargin = 20;    
    int rightMargin = 70; 




    // Définir la mise en page (les rectangles)
    QRect plotRect(leftMargin, topMargin,
                   width() - leftMargin - rightMargin,
                   height() - topMargin - bottomMargin);

    QRect barRect(plotRect.right() + 15, plotRect.top(), 30, plotRect.height());




    //Dessiner la heatmap (l’image)
    if (!image_.isNull()) {
        p.drawImage(plotRect, image_);
    } else {
        p.fillRect(plotRect, Qt::black);
    }


    p.setPen(Qt::red);
    p.drawRect(plotRect); 
    p.setFont(QFont("Arial", 8));

    


    int numTicks = 10;
    // Axe vertical (à gauche)
    for (int i = 0; i <= numTicks; ++i) 
    {
        double val = (double)i / numTicks;
        int sy = plotRect.bottom() - (int)(val * plotRect.height());

        p.drawLine(plotRect.left() - 5, sy, plotRect.left(), sy);

        
        QString text = QString::number(val, 'f', 1);
        p.drawText(QRect(0, sy - 10, leftMargin - 7, 20), Qt::AlignRight | Qt::AlignVCenter, text);
    }
    



    p.save();
    p.translate(15, plotRect.center().y());
    p.rotate(-90);
    p.drawText(QRect(-100, -10, 200, 20), Qt::AlignCenter, "Y");
    p.restore();






    //Axe horizontal (en bas)
    for (int i = 0; i <= numTicks; ++i) {
        double val = (double)i / numTicks;
        int sx = plotRect.left() + (int)(val * plotRect.width());

        p.drawLine(sx, plotRect.bottom(), sx, plotRect.bottom() + 5);

        QString text = QString::number(val, 'f', 1);
        p.drawText(QRect(sx - 15, plotRect.bottom() + 5, 30, 20), Qt::AlignHCenter | Qt::AlignTop, text);
    }
    p.drawText(QRect(plotRect.left(), height() - 20, plotRect.width(), 20), Qt::AlignCenter, "X");




    //Dessiner la barre de couleur (legend)
    QImage barImg(1, 256, QImage::Format_RGBX64);
    for (int y = 0; y < 256; ++y) {
       
        double v01 = 1.0 - (double)y / 255.0;
        QRgb c = plasmaColor(v01);
        barImg.setPixel(0, y, c);
    }
    p.drawImage(barRect, barImg);
    p.setPen(Qt::black);
    p.drawRect(barRect);

    int barTicks = 5;
    for (int i = 0; i <= barTicks; ++i) {
        double ratio = (double)i / barTicks; // 0.0 ~ 1.0
        double val = vmin_ + ratio * (vmax_ - vmin_);

        int y = barRect.bottom() - (int)(ratio * barRect.height());

        p.drawLine(barRect.right(), y, barRect.right() + 5, y);

        QString text = QString::number(val, 'f', 1);
        p.drawText(barRect.right() + 8, y + 5, text);
    }
    
    p.drawText(QRect(barRect.right() - 30, barRect.top() - 20, 60, 20), Qt::AlignLeft, "Density");



    //Superposer les “chimneys” (points)
    p.setPen(Qt::red);
    p.setBrush(Qt::gray);
    for (const auto& c : chimneys_) {

        // x physique -> droite écran
        int screenX = plotRect.left() + (int)(c.x0 * plotRect.width());

        // y physique -> haut écran
        int screenY = plotRect.bottom() - (int)(c.y0 * plotRect.height());

        p.drawEllipse(QPoint(screenX, screenY), 30, 30);
    }


    //flèche du vent
    {
        QPoint origin(plotRect.left() + 70, plotRect.top() + 70);

        const double scale = 120.0; // longueur visuelle de la flèche

        QPoint tip(
            origin.x() + (int)(scale * u_),   // u = horizontal
            origin.y() - (int)(scale * w_)    // w = vertical, signe inversé écran
            );

        p.setPen(QPen(Qt::red, 4));
        p.drawLine(origin, tip);



        // petite pointe de flèche
        const double dx = tip.x() - origin.x();
        const double dy = tip.y() - origin.y();
        const double len = std::sqrt(dx*dx + dy*dy);

        if (len > 1e-6) {
            const double ux = dx / len;
            const double uy = dy / len;

            QPoint left(
                tip.x() - (int)(12 * ux - 6 * uy),
                tip.y() - (int)(12 * uy + 6 * ux)
                );
            QPoint right(
                tip.x() - (int)(12 * ux + 6 * uy),
                tip.y() - (int)(12 * uy - 6 * ux)
                );

            p.drawLine(tip, left);
            p.drawLine(tip, right);
        }

        p.setBrush(Qt::black);
        p.drawEllipse(origin, 4, 4);
    }
}
