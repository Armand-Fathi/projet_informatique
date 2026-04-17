#include "heatmaprenderer.h"
#include <algorithm>
#include <cmath>



//Ça évite des couleurs “hors palette” si la normalisation dépasse [0,1].

static  double clamp01(double x) {
    if (x < 0.0) return 0.0;
    if (x > 1.0) return 1.0;
    return x;
}



//convertir une valeur normalisée en couleur
QRgb HeatMapRenderer::mapToColor(double v01) const
{
    v01 = clamp01(v01);


    //Palette par “stop points”
    //p = position sur l’axe 0→1
    //r,g,b = couleur associée (en double 0→1, pas 0→255)
    struct Stop { double p, r, g, b; };
    static const Stop stops[] = {
        {0.00, 0.05, 0.03, 0.53}, // bleu
        {0.25, 0.36, 0.07, 0.67}, // violet
        {0.50, 0.66, 0.18, 0.47}, // magenta
        {0.75, 0.91, 0.47, 0.20}, // orange
        {1.00, 0.99, 0.94, 0.14}  // jaune
    };

    //Trouver dans quel intervalle tombe v01
    int k = 0;
    while (k < 4 && v01 > stops[k+1].p) k++;
    const Stop& a = stops[k]; //a est un alias en lecture seule de stops[k]
    const Stop& b = stops[k+1];

    // Interpolation linéaire entre 2 couleurs
    double t = (v01 - a.p) / (b.p - a.p);
    double r = a.r + t * (b.r - a.r);
    double g = a.g + t * (b.g - a.g);
    double bb = a.b + t * (b.b - a.b);


    //Convertir en 0..255 (format Qt) + clamp
    auto to255 = [](double x){
        int v = (int)std::lround(x * 255.0); //arrondi au plus proche.
        return std::max(0, std::min(255, v));
    };

    return qRgb(to255(r), to255(g), to255(bb));
}




//créer l’image heatmap
QImage HeatMapRenderer::render(const std::vector<double>& q, int nx, int ny, bool autoScale)
{
    double qmin = 0.0;
    double qmax = 1.0;

    // recalculer qmax à partir des données
    if (autoScale && !q.empty()) {
        qmax = 0.0;
        for (double v : q) qmax = std::max(qmax, v);
        if (qmax < 1e-12) qmax = 1.0;
    }
   /*Si autoScale=true, on prend qmax = max(q)

Si toutes les valeurs sont presque 0, on force qmax=1 pour éviter division par 0*/
    lastMin_ = qmin;
    lastMax_ = qmax;

    // Créer l’image
    QImage img(ny, nx, QImage::Format_RGB32);
//ils mettent width = ny et height = nx.



    for (int i = 0; i < nx; ++i) 
    {
        for (int j = 0; j < ny; ++j) 
        {
            double val = q[i*ny + j];
            
            double v01 = (val - qmin) / (qmax - qmin); //normalisation
            img.setPixel(j, i, mapToColor(v01));
        }
    }
    return img;
}
