#ifndef RAY_H
#define RAY_H

#include "material.h"

class Ray
{
public:
    Vec o, d;
    Ray() = delete;
    Ray(const Vec& o_, const Vec& d_);
    Ray(const Ray& r);
    Vec point(double t) const;

};


class Hit
{
public:
    Vec n;
    double t;
    Material* m;
    bool into;
    Hit();
    Hit(const Vec& n_, double t_, Material* m_, bool into_);

};


class Camera
{
public:
    Vec o, d;
    int w, h, spp;
    Camera() = delete;
    Camera(const Vec& o_, const Vec& d_, int w_, int h_, int spp_=1);

};

#endif // RAY_H
