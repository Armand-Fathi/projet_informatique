#pragma once

struct Chimney {
    double x0 = 0.5;
    double y0 = 0.5;

    double sigmaX = 0.04;
    /*“Largeur” (écart-type) de la source dans la direction x.
Plus sigmaX est grand, plus la fumée est injectée sur une zone large en x (source plus étalée)
     */
    double sigmaY = 0.04;
    //ces sigmas servent à définir une source gaussienne



    double amplitude = 100.0;
    /*Intensité maximale (facteur multiplicatif) de la source de fumée.
Plus amplitude est grand, plus on injecte de fumée.*/

    bool continuous = true;
    /*Indique si la cheminée émet en continu (true) ou non (false).

true : on ajoute de la fumée à chaque pas de temps (source permanente)

false : plutôt une émission ponctuelle / impulsion (selon ton code)*/
    double rate = 20.0; // continuous=true contrôle combien de fumée on injecte par seconde
};
