
#include <QImage>
#include <vector>

class HeatMapRenderer {


    private:
    //transforme une valeur normalisée entre 0 et 1 en couleur
        QRgb mapToColor(double v01) const;

        double lastMin_ = 0.0;
        double lastMax_ = 1.0;


    public:
        //fabriquer une image heatmap à partir de q.
                //q :tableau 1D qui contient les valeurs (taille attendue : nx * ny)

        QImage render(const std::vector<double>& q, int nx, int ny, bool autoScale = true);





        double lastMin() const { return lastMin_; }
        double lastMax() const { return lastMax_; }





};
