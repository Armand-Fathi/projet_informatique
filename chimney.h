#ifndef CHIMNEY_H
#define CHIMNEY_H

typedef struct
{
    double x0=0; double y0=0;

    double sigmaX= 0; double sigmaY= 0; //étalement

    double amplitude= 0; //quantité de fumée injectée

    bool continuous =true;
    double rate=0.2; //débit d’émission
}Chimney;


#endif