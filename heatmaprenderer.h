
#include <QImage>
#include <vector>

class HeatMapRenderer {
public:
    //fabriquer une image heatmap à partir de q.
    QImage render(const std::vector<double>& q, int nx, int ny, bool autoScale = true);
    //q :tableau 1D qui contient les valeurs (taille attendue : nx * ny)



    double lastMin() const { return lastMin_; }
    double lastMax() const { return lastMax_; }
//Ça sert à récupérer les valeurs min/max utilisées lors du dernier rendu.



private:
    QRgb mapToColor(double v01) const;

    double lastMin_ = 0.0;
    double lastMax_ = 1.0;
};
