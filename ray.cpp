#include "ray.h"

Ray::Ray(const Vec& o_, const Vec& d_) { o = o_; d = d_; }
Vec Ray::point(double t) { return o + d * t; }

Hit::Hit() { n = Vec(); t = INF; into = false; }
Hit::Hit(const Vec& n_, double t_, bool into_) { n = n_; t = t_; into = into_; }
