#ifndef WIND_H
#define WIND_H

#include <cmath>

class Wind {


    private:


        double speed_ = 0;
        double angleDeg_ = 0;
        double horizontal = 0;
        double vertical = 0;


    public:

        void set(double speed, double angleDeg);



        //accesseurs

        double u() const { return horizontal; }
        double w() const { return vertical; }
        double speed() const { return speed_; }
        double angleDeg() const { return angleDeg_; }


};


#endif