#include "object.h"
#include <fstream>
#include <string>

Object::Object(Material* m_) { m = m_; }

Sphere::Sphere(double r_, const Vec& p_, Material* m_) : Object(m_)
    { rad = r_; p = p_; }

bool Sphere::intersect(const Ray& r, Hit& h)
{
    Vec op = p - r.o;
    double t;
    double b = Vec::dot(op, r.d);
    double det = b * b - op.len2() + rad * rad;
    if(det < 0)
        return false;
    det = sqrt(det);
    t = (t=b-det)>EPS ? t : ((t=b+det)>EPS ? t : INF);
    if(t < h.t) {
        Vec x = r.point(t);
        h.n = (x - p).norm();
        h.t = t;
        h.m = m;
        h.into = Vec::dot(h.n, r.d) < 0;
        return true;
    }
    return false;
}

void Sphere::print()
{
    fprintf(stderr, "[SPHERE] Rad %.2lf, Center (%.2lf, %.2lf, %.2lf)\n", rad, p.x, p.y, p.z);
}

bool triangle_intersect(const Vec& v0, const Vec& v1, const Vec& v2, const Ray& r,
                        double& t, double& u, double& v)
{
    Vec E1 = v1 - v0;
    Vec E2 = v2 - v0;
    Vec pvec = Vec::cross(r.d, E2);
    double det = Vec::dot(E1, pvec);
    if(det <= 0)
        return false;
    
    Vec tvec = r.o - v0;
    u = Vec::dot(tvec, pvec);
    if(u < 0 || u > det)
        return false;
    
    Vec qvec = Vec::cross(tvec, E1);
    v = Vec::dot(r.d, qvec);
    if(v < 0 || u + v > det)
        return false;
    
    t = Vec::dot(E2, qvec) / det;
    return true;
}

Triangle::Triangle(const Vec& a, const Vec& b, const Vec& c, Material* m_) : Object(m_)
    { v[0] = a; v[1] = b; v[2] = c; n = Vec::cross(b-a, c-a).norm(); }

bool Triangle::intersect(const Ray& r, Hit& h)
{
    double t, a, b;
    if(!triangle_intersect(v[0], v[1], v[2], r, t, a, b))
        return false;
    if(0 < t && t < h.t) {
        h.n = n;
        h.t = t;
        h.m = m;
        h.into = true;
        return true;
    }
    return false;
}

void Triangle::print()
{
    fprintf(stderr, "[TRIANGLE]\n");
}

TriangleMesh::TriangleMesh(const char* path, Material* m_) : Object(m_)
{
    std::ifstream in;
    in.open(path, std::fstream::in);
    std::string line;
    int p;
    while(getline(in, line)) {
        if(line[0] == 'v') {
            double v[3];
            p = 0;
            for(int i = 0; i < 3; i++) {
                p = line.find(" ", p) + 1;
                v[i] = stod(line.substr(p));
            }
            vertex.push_back(Vec(v[0], v[1], v[2]));
        }
        if(line[0] == 'f') {
            vector<int> idx;
            p = 0;
            for(int i = 0; i < 3; i++) {
                p = line.find(" ", p) + 1;
                idx.push_back(stoi(line.substr(p)));
            }
            faces.push_back(idx);
        }
    }
    in.close();
}

bool TriangleMesh::intersect(const Ray& r, Hit& h)
{
    double t, a, b;
    for(auto f : faces) {
        const Vec& v0 = vertex[f[0]];
        const Vec& v1 = vertex[f[1]];
        const Vec& v2 = vertex[f[2]];
        if(triangle_intersect(v0, v1, v2, r, t, a, b)) {
            if(0 < t && t < h.t) {
                h.n = Vec::cross(v1-v0, v2-v0).norm();
                h.t = t;
                h.m = m;
                h.into = true;
            }
        }
    }
    return h.t < INF;
}

void TriangleMesh::print()
{
    fprintf(stderr, "[MESH]\n");
    for(auto& v : vertex)
        fprintf(stderr, "Vertex (%.2lf, %.2lf, %.2lf)\n", v.x, v.y, v.z);
    for(auto& v : faces)
        fprintf(stderr, "Faces (%d, %d, %d)\n", v[0], v[1], v[2]);
}

Material* materials[] = {
    new Material(Vec(), Vec(.75,.25,.25),   DIFF, 1.5), // red
    new Material(Vec(), Vec(.25,.25,.75),   DIFF, 1.5), // blue
    new Material(Vec(), Vec(.65, .85, .92), DIFF, 1.5), // cyan
    new Material(Vec(), Vec(.98, .73, .83), DIFF, 1.5), // pink
    new Material(Vec(), Vec(.66, .58, .85), DIFF, 1.5), // purple
    new Material(Vec(), Vec(),              DIFF, 1.5), // black
    new Material(Vec(), Vec(.75, .75, .75), DIFF, 1.5), // gray
    new Material(Vec(), Vec(.58, .88, .82), DIFF, 1.5), // 
    new Material(Vec(3, 3, 3), Vec(),    DIFF, 1.5)  // light
};

Object* objects[] = {
    // sphere walls
    new Sphere(1e5, Vec( 1e5+1,40.8,81.6),  new Material(Vec(), Vec(.65, .85, .92), DIFF, 1.5)),
    new Sphere(1e5, Vec(-1e5+99,40.8,81.6), new Material(Vec(), Vec(.98, .73, .83), DIFF, 1.5)),
    new Sphere(1e5, Vec(50,40.8, 1e5),      new Material(Vec(), Vec(.75, .75, .75), DIFF, 1.5)),
    new Sphere(1e5, Vec(50,40.8,-1e5+170),  new Material(Vec(), Vec(),              DIFF, 1.5)),
    new Sphere(1e5, Vec(50, 1e5, 81.6),     new Material(Vec(), Vec(.75, .75, .75), DIFF, 1.5)),
    new Sphere(1e5, Vec(50,-1e5+81.6,81.6), new Material(Vec(), Vec(.66, .58, .85), DIFF, 1.5)),

    // sphere light
    new Sphere(600, Vec(50,681.6-.27,81.6), new Material(Vec(10, 10, 10), Vec(),    DIFF, 1.5)),

    // mesh walls
    new TriangleMesh("./cornellbox/left.obj",   materials[2]),
    new TriangleMesh("./cornellbox/right.obj",  materials[3]),
    new TriangleMesh("./cornellbox/back.obj",   materials[6]),
    new TriangleMesh("./cornellbox/front.obj",  materials[5]),
    new TriangleMesh("./cornellbox/bottom.obj", materials[6]),
    new TriangleMesh("./cornellbox/top.obj",    materials[4]),

    // mesh light
    new TriangleMesh("./cornellbox/light.obj",  materials[8]),

    // spheres
    new Sphere(16.5,Vec(27,16.5,47),        new Material(Vec(), Vec(.99, .99, .99)*.999, SPEC, 1.5)),
    new Sphere(16.5,Vec(73,16.5,78),        new Material(Vec(), Vec(.99, .99, .99), REFR, 1.5)),
    new Sphere(6,   Vec(38, 6, 90),         new Material(Vec(), Vec(.58, .88, .82), DIFF, 1.5)),
    new Sphere(9,   Vec(50, 9, 70),         new Material(Vec(), Vec(.99, .99, .82), REFR, 1.5)),
};

Scene::Scene(int id)
{
    vector<int> obj_id;
    if(id == 1) {
        obj_id = { 0, 1, 2, 3, 4, 5, 14, 15, 16, 17, 6 };
    }
    else {
        obj_id = { 7, 6 };
    }
    num = obj_id.size();
    for(int id : obj_id)
        objs.push_back(objects[id]);
}

bool Scene::intersect(const Ray& r, Hit& h)
{
    for(Object* obj : objs)
        obj->intersect(r, h);
    return h.t < INF;
}
