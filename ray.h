#ifndef RAY_H
#define RAY_H

#include "vec.h"

class Ray
{
public:
    Vec o, d;
    Ray() = delete;
    Ray(const Vec& o_, const Vec& d_);
    Vec point(double t);

};

class Hit
{
public:
    Vec n;
    double t;
    
    bool into;
    Hit();
    Hit(const Vec& n_, double t_, bool into_);

};


#endif // RAY_H
