#include "advectionsolver.h"
#include <algorithm>
#include <cmath>

void AdvectionSolver::initialize(int nx, int ny, double /*dx*/, double /*dy*/)
{
<<<<<<< HEAD
    tmp_.assign(nx * ny, 0.0);
}

void AdvectionSolver::stepUpwind(std::vector<double>& q,
                                 int nx, int ny,
                                 double dx, double dy,
                                 double u, double w,
                                 double dt)
{
    if ((int)tmp_.size() != nx * ny) tmp_.assign(nx * ny, 0.0);
    std::fill(tmp_.begin(), tmp_.end(), 0.0); // 吸收边界：边界默认 0

    const double ax = std::abs(u) * dt / dx;
    const double ay = std::abs(w) * dt / dy;

    // 更新内部点
    for (int i = 1; i < nx - 1; ++i) {
        for (int j = 1; j < ny - 1; ++j) {
            const double qij = q[idx(i,j,ny)];

            const double q_up_x = (u >= 0.0) ? q[idx(i-1,j,ny)] : q[idx(i+1,j,ny)];
            const double q_up_y = (w >= 0.0) ? q[idx(i,j-1,ny)] : q[idx(i,j+1,ny)];

            double next =
                (1.0 - ax - ay) * qij
                + ax * q_up_x
                + ay * q_up_y;

            if (next < 0.0) next = 0.0; // 防止数值误差出现负浓度
            tmp_[idx(i,j,ny)] = next;
=======
    tmp_.assign(nx * ny, 0.0);/*Redimensionne tmp_ à nx*ny et met toutes les valeurs à 0.0.
(ça prépare le buffer temporaire pour une grille 2D aplatie)*/
    tmp2_.assign(nx * ny, 0.0); // tmp2_ (deuxième buffer temporaire, utilisé pour la diffusion).
}

double AdvectionSolver::computeDtByCFL(double u, double w, double safety) const //const : ne modifie pas l’objet
{

    (void)u; (void)w; (void)safety; //vitesse u (x), w (y), et facteur de sécurité safety
    return 0.0;
}


//Définition de la fonction qui applique un pas d’advection.
void AdvectionSolver::stepUpwind(std::vector<double>& q,  //q est passé par référence → on modifie directement le champ (densité).
                                 int nx, int ny, //Dimensions de la grille.
                                 double dx, double dy, //Pas spatiaux.
                                 double u, double w, //Vitesses (x et y).
                                 double dt) //Pas de temps.
{
    if ((int)tmp_.size() != nx * ny) tmp_.assign(nx * ny, 0.0); //Si tmp_ n’a pas la bonne taille, on le redimensionne et on met tout à 0
    std::fill(tmp_.begin(), tmp_.end(), 0.0); // Remplit tmp_ avec 0.0 partout.
    //Donc les bords restent à 0 → ça simule une “absorption” (fumée qui disparaît aux bords)

    const double ax = std::abs(u) * dt / dx; //Calcule le coefficient d’advection
    const double ay = std::abs(w) * dt / dy;

    // Boucles sur les points internes
    for (int i = 1; i < nx - 1; ++i) { //Boucle sur i, en évitant les bords
        for (int j = 1; j < ny - 1; ++j) {
            const double qij = q[idx(i,j,ny)];
            /*Récupère la valeur actuelle au point (i,j).
idx(i,j,ny) convertit (i,j) → index 1D.*/

            const double q_up_x = (u >= 0.0) ? q[idx(i-1,j,ny)] : q[idx(i+1,j,ny)];
            /*Si u >= 0, le flux vient de la gauche (amont) → on prend (i-1,j).
Si u < 0, le flux vient de la droite → on prend (i+1,j).
C’est le principe upwind.*/

            const double q_up_y = (w >= 0.0) ? q[idx(i,j-1,ny)] : q[idx(i,j+1,ny)];
//si w >= 0 on prend (i,j-1), sinon (i,j+1).

            double next =
                (1.0 - ax - ay) * qij //Part “restée sur place” du point courant.
                + ax * q_up_x //Contribution venant de l’amont en x.
                + ay * q_up_y; // Contribution venant de l’amont en y

            if (next < 0.0) next = 0.0; // Si à cause d’erreurs numériques on obtient une valeur négative, on la remet à 0.
            tmp_[idx(i,j,ny)] = next; //Stocke le résultat dans tmp_ (pas directement dans q)
>>>>>>> 24c4fa1 (a stable version with ui and diffusion)
        }
    }

    q.swap(tmp_);
<<<<<<< HEAD
=======
    /*Échange les contenus de q et tmp_.
Résultat : q devient le nouveau champ calculé, et tmp_ récupère l’ancien champ.
C’est plus rapide que copier nx*ny valeurs.*/
}


void AdvectionSolver::stepDiffusion(std::vector<double>& q, //Fonction qui applique un pas de diffusion sur le champ q
                                    int nx, int ny, //Dimensions.
                                    double dx, double dy, //Pas spatial.
                                    double kappa, // Coefficient de diffusion k                                      κ
                                    double dt) //Pas de temps.
{
    if (kappa <= 0.0) return; //pas de diffusion → on sort immédiatement.

    if ((int)tmp2_.size() != nx * ny) tmp2_.assign(nx * ny, 0.0);//Vérifie la taille de tmp2_, sinon resize + mise à 0.
    std::fill(tmp2_.begin(), tmp2_.end(), 0.0); // Met tout à 0 : les bords restent 0 (absorbants).

    const double invdx2 = 1.0 / (dx * dx);
    const double invdy2 = 1.0 / (dy * dy);
    const double a = kappa * dt; //facteur devant le Laplacien

    for (int i = 1; i < nx - 1; ++i) {
        for (int j = 1; j < ny - 1; ++j) {
            const double qij = q[idx(i,j,ny)]; //Valeur au point

            const double lap = //Déclare le Laplacien.
                (q[idx(i+1,j,ny)] - 2.0*qij + q[idx(i-1,j,ny)]) * invdx2 +
                (q[idx(i,j+1,ny)] - 2.0*qij + q[idx(i,j-1,ny)]) * invdy2;


            //Mise à jour diffusion explicite
            double next = qij + a * lap;
            if (next < 0.0) next = 0.0;
            tmp2_[idx(i,j,ny)] = next; //Stocke le résultat dans tmp2_
        }
    }

    q.swap(tmp2_); //Remplace q par la nouvelle version diffusée, sans copie lourde.
>>>>>>> 24c4fa1 (a stable version with ui and diffusion)
}
