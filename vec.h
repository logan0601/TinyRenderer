#ifndef VEC_H
#define VEC_H

#include <cmath>

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

    // Length Op
    double len() const;
    double len2() const;
    Vec& norm();

    // Max op
    double max() const;

    // Index Op
    double operator[](int i) const;

    // Product Op
    static double dot(const Vec& a, const Vec& b);
    static Vec cross(const Vec& a, const Vec& b);
};

#endif // VEC_H
