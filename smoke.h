#ifndef SMOKE_H
#define SMOKE_H

#include <vector>
#include "chimney.h"
using namespace std;


class Smoke
{
private:
    int nx_=0, ny_=0; // nombre de points lignes, colonnes
    double dx_= 0 ; double dy_= 0; // pas spatial lignes, colonnes

    vector<double> q_; //en mémoire, q est un simple vecteur 1D
    vector<Chimney> chimneys_; //liste pour avoir plusieurs plus tard
    


    int idx(int i, int j) const // 2d -> 1d
    {
        return i*ny_ + j;
    }

public:
 
    void initialize(int nx1, int ny1, double l1,double h1);
    void reset();
    void addGaussian(const Chimney& c);
    void emitContinuous(double dt);

    void setChimneys(const vector<Chimney>& cs) { chimneys_ = cs; } // pour simulationcontroller
    const vector<Chimney>& chimneys() const { return chimneys_; }// pour simulationcontroller

    vector<double>& field() { return q_; } // pour simulationcontroller

    int nx() const { return nx_; } // pour simulationcontroller
    int ny() const { return ny_; }
    double dx() const { return dx_; }
    double dy() const { return dy_; }


};


#endif