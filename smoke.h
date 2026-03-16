#pragma once
#include <vector>
#include "chimney.h"
using namespace std;


class Smoke
{
private:


    vector<double> q_; //en mémoire, q est un simple vecteur
    vector<Chimney> chimneys_;
    double l_=0, h_=0;
    int nx_=0, ny_=0;
    double dx_= 0 ;
    double dy_= 0;

    int idx(int i, int j) const // 2d -> 1d
    {
        return i*ny_ + j;
    }

public:
    void initialize(int nx1, int ny1, double l1,double h1);
    void reset();
    void addGaussian(const Chimney& c);
    void emitContinuous(double dt);

    void setChimneys(const std::vector<Chimney>& cs) { chimneys_ = cs; }
    const std::vector<Chimney>& chimneys() const { return chimneys_; }

    std::vector<double>& field() { return q_; }
    const std::vector<double>& field() const { return q_; }

    int nx() const { return nx_; }
    int ny() const { return ny_; }
    double dx() const { return dx_; }
    double dy() const { return dy_; }


};
