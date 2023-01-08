#include "ray.h"

Ray::Ray(const Vec& o_, const Vec& d_) { o = o_; d = d_; }
Ray::Ray(const Ray& r) { o = r.o; d = r.d; }
Vec Ray::point(double t) const { return o + d * t; }

Hit::Hit() { n = Vec(); t = INF; m = nullptr;  }
Hit::Hit(const Vec& n_, double t_, Material* m_, bool into_)
    { n = n_; t = t_; m = m_; into = into_; }

Camera::Camera(const Vec& o_, const Vec& d_, int w_, int h_, int spp_)
    { o = o_; d = d_; w = w_; h = h_; spp = spp_; }
