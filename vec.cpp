#include "vec.h"

Vec::Vec(double x_, double y_, double z_) { x=x_; y=y_; z=z_; }
Vec::Vec(const Vec& v) { x=v.x; y=v.y; z=v.z; }

Vec Vec::operator+(const Vec& v) const { return Vec(x+v.x, y+v.y, z+v.z); }
Vec Vec::operator-(const Vec& v) const { return Vec(x-v.x, y-v.y, z-v.z); }
Vec Vec::operator*(const Vec& v) const { return Vec(x*v.x, y*v.y, z*v.z); }

Vec Vec::operator+(double s) const { return Vec(x+s, y+s, z+s); }
Vec Vec::operator-(double s) const { return Vec(x-s, y-s, z-s); }
Vec Vec::operator*(double s) const { return Vec(x*s, y*s, z*s); }
Vec Vec::operator/(double s) const { return Vec(x/s, y/s, z/s); }

Vec& Vec::operator+=(const Vec& v) { return *this = *this + v; }
Vec& Vec::operator-=(const Vec& v) { return *this = *this - v; }
Vec& Vec::operator+=(double s) { return *this = *this + s; }
Vec& Vec::operator-=(double s) { return *this = *this - s; }
Vec& Vec::operator*=(double s) { return *this = *this * s; }
Vec& Vec::operator/=(double s) { return *this = *this / s; }

double Vec::max() const { return (x>y&&x>z) ? x : (y>z ? y : z); }

double Vec::len() const { return sqrt(x*x + y*y + z*z); }
double Vec::len2() const { return x*x + y*y + z*z; }
Vec& Vec::norm() { return *this = *this / this->len(); }

double Vec::operator[](int i) const { return (i==0) ? x : (i==1 ? y : z); }

Vec& Vec::clamp() { x = CLP(x); y = CLP(y); z = CLP(z); return *this; }

double Vec::dot(const Vec& a, const Vec& b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
Vec Vec::cross(const Vec& a, const Vec& b) { return Vec(a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x); }

