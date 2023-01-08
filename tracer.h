#ifndef TRACER_H
#define TRACER_H

#include "ray.h"
#include "object.h"

class PathTracer
{
public:
    Scene* scene;
    Camera* cam;
    Vec* im;
    int w, h, tot = 0;
    PathTracer(Scene* scene_, Camera* cam_);
    Vec radiance(const Ray& r, int dep, unsigned short* X);
    void render();
    void write(const char* path);

};

#endif // TRACER_H
