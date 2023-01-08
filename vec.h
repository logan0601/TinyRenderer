#ifndef VEC_H
#define VEC_H

#include <cmath>
#include <iostream>

#define INF 1e20
#define EPS 1e-4
#define CLP(x) x<0?0:(x>1?1:x)
#define TOINT(x) int(pow(CLP(x), 1/2.2)*255+.5)

class Vec
{
public:
    double x, y, z;

    // Init
    Vec(double x_=0, double y_=0, double z_=0);
    Vec(const Vec& v);

    // Vector Op
    Vec operator+(const Vec& v) const;
    Vec operator-(const Vec& v) const;
    Vec operator*(const Vec& v) const;

    // Scalar Op
    Vec operator+(double s) const;
    Vec operator-(double s) const;
    Vec operator*(double s) const;
    Vec operator/(double s) const;

    // Assign Op
    Vec& operator+=(const Vec& v);
    Vec& operator-=(const Vec& v);
    Vec& operator+=(double s);
    Vec& operator-=(double s);
    Vec& operator*=(double s);
    Vec& operator/=(double s);

    // Max Op
    double max() const;

    // Length Op
    double len() const;
    double len2() const;
    Vec& norm();

    // Index Op
    double operator[](int i) const;

    // Clamp Op
    Vec& clamp();

    // Product Op
    static double dot(const Vec& a, const Vec& b);
    static Vec cross(const Vec& a, const Vec& b);

};


#endif // VEC_H
