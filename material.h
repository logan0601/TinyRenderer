#ifndef MATERIAL_H
#define MATERIAL_H

#include "vec.h"

enum Refl_t { DIFF, SPEC, REFR };

class Material
{
public:
    Vec e, c;
    Refl_t rt;
    double nt;
    Material(const Vec& e_, const Vec& c_, Refl_t rt_, double nt_);

};

#endif // MATERIAL_H
