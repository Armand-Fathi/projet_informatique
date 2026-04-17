#pragma once
#include <vector>

class AdvectionSolver
{
public:

    void initialize(int nx, int ny);

    void stepUpwind(std::vector<double>& q, //q est un vecteur passé par référence (&) donc on modifie directement le tableau du champ (ex : densité de fumée)
                    int nx, int ny, //Les dimensions de la grille passées à la fonction
                    double dx, double dy, //Les pas spatiaux.
                    double u, double w, //Les vitesses d’advection (transport) en x et y.
                    double dt); //Le pas de temps à utiliser.

    // Un pas de diffusion (Laplacien)
    void stepDiffusion(std::vector<double>& q, //Méthode de diffusion : elle modifie q (référence).
                       int nx, int ny, //Dimensions de la grille.
                       double dx, double dy, //Pas spatial.
                       double kappa, //Coefficient de diffusion (souvent noté κ). Plus κ est grand, plus ça “étale” vite.
                       double dt); //Pas de temps.

private: //interne à la classe : accessible uniquement depuis ses méthodes.
    std::vector<double> tmp_;//Tableau temporaire interne utilisé pour stocker un résultat intermédiaire (ex : résultat de stepUpwind).
    std::vector<double> tmp2_;   // Deuxième tableau temporaire interne (ajouté), souvent utile pour diffusion (ou pour éviter d’écraser des valeurs pendant le calcul).

    inline int idx(int i, int j, int ny) const { return i * ny + j; }
    /*Elle convertit un index 2D (i, j) en index 1D pour accéder au std::vector.

i : indice en x

j : indice en y

ny : taille en y
     */
};
