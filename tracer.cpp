#include "tracer.h"
#include <omp.h>

PathTracer::PathTracer(Scene* scene_, Camera* cam_)
    { scene = scene_; cam = cam_; w = cam_->w; h = cam_->h; im = new Vec[w * h]; }


Vec PathTracer::radiance(const Ray& r, int dep, unsigned short* X)
{
    Hit h;
    if(!scene->intersect(r, h))
        return Vec();
    Vec f = h.m->c;
    double p = f.max();
    dep++;
    if(dep > 5) {
        if(erand48(X) < p) f /= p;
        else return h.m->e;
    }

    Vec x = r.point(h.t), nl = h.into ? h.n : h.n * -1.0;
    if(h.m->rt == DIFF) {
        double r1 = 2*M_PI*erand48(X), r2 = erand48(X), r2s = sqrt(r2);
        Vec w = nl;
        Vec u = (Vec::cross((fabs(w.x)>.1 ? Vec(0, 1) : Vec(1)), w)).norm();
        Vec v = Vec::cross(w, u);
        Vec d = (u*cos(r1)*r2s + v*sin(r1)*r2s + w*sqrt(1-r2)).norm();
        return h.m->e + f * radiance(Ray(x, d), dep, X);
    }
    else if(h.m->rt == SPEC) {
        return h.m->e + f * radiance(Ray(x, r.d-h.n*2*Vec::dot(h.n, r.d)), dep, X);
    }
    else if(h.m->rt == GLOS) {
        Vec w = nl;
        Vec u = (Vec::cross((fabs(w.x)>.1 ? Vec(0, 1) : Vec(1)), w)).norm();
        Vec v = Vec::cross(w, u);
        Vec rd = r.d - h.n * 2 * Vec::dot(h.n, r.d);
        if(Vec::dot(rd, u) < EPS) {
            u = u * -1;
            v = v * -1;
        }
        double r1 = 2 * erand48(X), r2 = 2 * erand48(X);
        double dx = r1<1 ? sqrt(r1)-1: 1-sqrt(2-r1);
        double dy = r2<1 ? sqrt(r2)-1: 1-sqrt(2-r2);
        double al = acos(Vec::dot(rd, w));
        double be = acos(Vec::dot(rd - w * al, v));
        al += dx * 0.2;
        al = std::max(0.0, al);
        be += dy * 0.2;
        be = std::max(0.0, std::min(be, M_PI));
        rd = (w * cos(al) + u * sin(al) * sin(be) + v * sin(al) * cos(be)).norm();
        return h.m->e + f * radiance(Ray(x, rd), dep, X);
    }
    else {
        Ray ref_ray(x, r.d-h.n*2*Vec::dot(h.n, r.d));
        double nnt = h.into?1.0/h.m->nt:h.m->nt;
        double ddn = Vec::dot(r.d, nl);
        double c2t = 1 - nnt * nnt * (1 - ddn * ddn);
        if(c2t < 0)
            return h.m->e + f * radiance(ref_ray, dep, X);
        Vec td = (r.d*nnt - h.n*(h.into?1:-1)*(ddn*nnt+sqrt(c2t))).norm();
        double a = h.m->nt-1, b = h.m->nt+1;
        double R0 = a*a/(b*b), c = 1-(h.into?-ddn:Vec::dot(td, h.n));
        double Re = R0+(1-R0)*c*c*c*c*c, Tr=1-Re, P=.25+.5*Re, RP=Re/P, TP=Tr/(1-P);
        return h.m->e + f * (dep>2 ? (erand48(X)<P ?
            radiance(ref_ray, dep, X)*RP : radiance(Ray(x, td), dep, X)*TP) :
            radiance(ref_ray, dep, X)*Re + radiance(Ray(x, td), dep, X)*Tr);
    }
}


void PathTracer::render()
{
    double start = omp_get_wtime();
    Vec cx = Vec(w*.5135/h), cy = Vec::cross(cx, cam->d).norm()*.5135;
    Vec r;
    #pragma omp parallel for schedule(dynamic, 1) private(r)
    for(int y = 0; y < h; y++) {
        fprintf(stderr, "\rRendering (%d spp) %5.2f%%", cam->spp*4, 100.*y/(h-1));
        unsigned short X[3] = {0, 0, (unsigned short)(y*y*y)};
        for(int x = 0; x < w; x++) {
            int i = (h-y-1)*w+x;
            for(int sy = 0; sy < 2; sy++) {
                for(int sx = 0; sx < 2; sx++) {
                    r = Vec();
                    for(int s = 0; s < cam->spp; s++) {
                        double r1 = 2 * erand48(X), r2 = 2 * erand48(X);
                        double dx = r1<1 ? sqrt(r1)-1: 1-sqrt(2-r1);
                        double dy = r2<1 ? sqrt(r2)-1: 1-sqrt(2-r2);
                        Vec d = cx*( ( (sx+.5 + dx)/2 + x)/w -.5) +
                                cy*( ( (sy+.5 + dy)/2 + y)/h -.5) + cam->d;
                        r += radiance(Ray(cam->o+d*140, d.norm()), 0, X) * (1.0/cam->spp);
                    }
                    im[i] += r.clamp() * .25;
                }
            }
        }
    }
    double end = omp_get_wtime();
    int sep = end - start;
    fprintf(stderr, "\nUsing Time %d:%d:%d\n", sep/3600, (sep%3600)/60, sep%60);
}


void PathTracer::write(const char* path)
{
    FILE* f = fopen("image.ppm", "w");
    fprintf(f, "P3\n%d %d\n%d\n", w, h, 255);
    for(int i = 0; i < w*h; i++)
        fprintf(f, "%d %d %d ", TOINT(im[i].x), TOINT(im[i].y), TOINT(im[i].z));
}
