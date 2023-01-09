#include "ray.h"
#include "object.h"
#include "tracer.h"
#include <iostream>

int main(int argc, char* argv[])
{
    int w = 1024, h = 768, spp = (argc == 1)? 100: atoi(argv[1])/4;
    Scene* scene = new Scene(2);
    Camera* cam = new Camera(Vec(50,52,295.6), Vec(0,-0.042612,-1).norm(), w, h, spp);
    PathTracer* tracer = new PathTracer(scene, cam);
    tracer->render();
    tracer->write("image.ppm");
    return 0;
}
