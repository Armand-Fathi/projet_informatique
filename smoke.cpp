#include "smoke.h"
#include <algorithm>
#include <cmath>

void Smoke::initialize(int nx, int ny, double L, double H)
{
    nx_ = nx; ny_ = ny;
    //Stocke les dimensions de la grille dans les attributs de l’objet (nx_, ny_).
    L_ = L;  H_ = H;
    //Stocke les dimensions physiques du domaine : largeur L_ et hauteur H_


    dx_ = L_ / (nx_ - 1);//Calcule le pas spatial en x.
    dy_ = H_ / (ny_ - 1);//Calcule le pas spatial en y

    q_.assign(nx_ * ny_, 0.0); //Crée/resize le vecteur q_ à nx_*ny_ cases et met tout à 0.0
}


void Smoke::reset()
{
    std::fill(q_.begin(), q_.end(), 0.0); //Met toutes les valeurs de q_ à zéro → supprime toute la fumée
}

void Smoke::addGaussian(const Chimney& c) //passé par référence constante → on ne modifie pas c
{
    // q += A * exp(-(x-x0)^2/(2*sx^2)) * exp(-(y-y0)^2/(2*sy^2))
    for (int i = 0; i < nx_; ++i) {
        const double x = i * dx_; //Convertit l’indice i en coordonnée physique x
        const double ex = std::exp(- (x - c.x0) * (x - c.x0) / (2.0 * c.sigmaX * c.sigmaX));
        for (int j = 0; j < ny_; ++j) {
            const double y = j * dy_;  //Convertit j en coordonnée physique y
            const double ey = std::exp(- (y - c.y0) * (y - c.y0) / (2.0 * c.sigmaY * c.sigmaY));
            q_[idx(i,j)] += c.amplitude * ex * ey;
            /*Ajoute au champ q_ la contribution de la gaussienne au point (i,j).

idx(i,j) transforme (i,j) → index 1D dans q_

c.amplitude est l’intensité globale

ex*ey donne la forme 2D.*/


        }
    }
}

void Smoke::emitContinuous(double dt)
{
    for (const auto& c : chimneys_) {
        if (!c.continuous || c.rate <= 0.0) continue;
        Chimney injected = c;
        injected.amplitude = c.rate * dt; // 每秒 rate，乘dt得到本帧叠加量
        addGaussian(injected);
    }
}
