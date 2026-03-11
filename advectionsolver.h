#pragma once
#include <vector>

class AdvectionSolver
{
<<<<<<< HEAD
public:
    void initialize(int nx, int ny, double dx, double dy);


    // 核心：upwind 推进一步（输入 q，输出到内部 tmp 再 swap 回 q）
    void stepUpwind(std::vector<double>& q,
                    int nx, int ny,
                    double dx, double dy,
                    double u, double w,
                    double dt);

private:
    std::vector<double> tmp_;
    inline int idx(int i, int j, int ny) const { return i * ny + j; }
=======
public: //accessible depuis l’extérieur de la classe

    void initialize(int nx, int ny, double dx, double dy);

    /* nx, ny : nombre de cellules (ou points) dans les directions x et y

dx, dy : pas d’espace (taille d’une cellule) en x et y */


    // Calcul du pas de temps dt avec CFL  Courant-Friedrichs-Lewy. garantir la stabilité d'une simulation
    double computeDtByCFL(double u, double w, double safety = 0.8) const;
    /*u : vitesse selon x       w : vitesse selon y (souvent notée v, ici w)
safety = 0.8 : valeur par défaut (facteur de sécurité CFL, < 1)
const à la fin signifie : cette fonction ne modifie pas l’objet (ne modifie pas les champs de la classe).*/


    //Un pas d’advection upwind
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
>>>>>>> 24c4fa1 (a stable version with ui and diffusion)
};
