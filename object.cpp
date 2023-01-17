#include "object.h"
#include <fstream>
#include <string>

double erand() { return rand() / double(RAND_MAX); }

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

pvd Sphere::sample(const Vec& o)
{
    Vec po = o - p;
    Vec w = po.norm();
    Vec u = (Vec::cross((fabs(w.x)>.1 ? Vec(0, 1) : Vec(1)), w)).norm();
    Vec v = Vec::cross(w, u);
    double t = acos(rad / po.len());
    double r1 = erand() * t, r2 = erand() * 2 * M_PI;
    double area = 2 * M_PI * rad * rad * (1 - cos(t));
    Vec x = (w * cos(r1) + u * sin(r1) * cos(r2) + v * sin(r1) * cos(r2)).norm() * rad;
    return pvd(x, area);
}

int Sphere::type() { return SPH; }

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

pvd Triangle::sample(const Vec& o)
{
    double a = erand(), b = erand(), c = 1 - a - b;
    Vec x = v[0] * a + v[1] * b + v[2] * c;
    double area = Vec::cross(v[1]-v[0], v[2]-v[0]).len();
    return pvd(x, area);
}

int Triangle::type() { return TRI; }
void Triangle::print() { fprintf(stderr, "[TRIANGLE]\n"); }

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

pvd TriangleMesh::sample(const Vec& o)
{
    int id = int(erand() * (faces.size() - .1));
    double area = 0;
    for(auto f : faces) {
        const Vec& v0 = vertex[f[0]];
        const Vec& v1 = vertex[f[1]];
        const Vec& v2 = vertex[f[2]];
        area = Vec::cross(v1-v0, v2-v0).len();
    }
    double a = erand(), b = erand(), c = 1 - a - b;
    Vec x = vertex[faces[id][0]] * a + vertex[faces[id][1]] * b + vertex[faces[id][2]] * c;
    return pvd(x, area);
}

int TriangleMesh::type() { return MESH; }

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
    new Material(Vec(3, 3, 3), Vec(),       DIFF, 1.5), // low light
    new Material(Vec(15, 15, 15), Vec(),    DIFF, 1.5)  // high light
};

Object* objects[] = {
    // sphere walls 0-5
    new Sphere(1e5, Vec( 1e5+1,40.8,81.6),  new Material(Vec(), Vec(.65, .85, .92), DIFF, 1.5)),
    new Sphere(1e5, Vec(-1e5+99,40.8,81.6), new Material(Vec(), Vec(.98, .73, .83), DIFF, 1.5)),
    new Sphere(1e5, Vec(50,40.8, 1e5),      new Material(Vec(), Vec(.75, .75, .75), DIFF, 1.5)),
    new Sphere(1e5, Vec(50,40.8,-1e5+170),  new Material(Vec(), Vec(),              DIFF, 1.5)),
    new Sphere(1e5, Vec(50, 1e5, 81.6),     new Material(Vec(), Vec(.75, .75, .75), DIFF, 1.5)),
    new Sphere(1e5, Vec(50,-1e5+81.6,81.6), new Material(Vec(), Vec(.66, .58, .85), DIFF, 1.5)),

    // sphere light 6
    new Sphere(6, Vec(50,70.6,81.6),        new Material(Vec(15, 15, 15), Vec(),    DIFF, 1.5)),

    // mesh walls 7-12
    new TriangleMesh("./cornellbox/left.obj",   materials[2]),
    new TriangleMesh("./cornellbox/right.obj",  materials[3]),
    new TriangleMesh("./cornellbox/back.obj",   materials[6]),
    new TriangleMesh("./cornellbox/front.obj",  materials[5]),
    new TriangleMesh("./cornellbox/bottom.obj", materials[6]),
    new TriangleMesh("./cornellbox/top.obj",    materials[4]),

    // mesh light 13
    new TriangleMesh("./cornellbox/light.obj",  materials[8]),

    // scene 1 14-17
    new Sphere(16.5,Vec(27,16.5,47),        new Material(Vec(), Vec(.99, .99, .99)*.999, SPEC, 1.5)),
    new Sphere(16.5,Vec(73,16.5,78),        new Material(Vec(), Vec(.99, .99, .99), REFR, 1.5)),
    new Sphere(6,   Vec(38, 6, 90),         new Material(Vec(), Vec(.58, .88, .82), DIFF, 1.5)),
    new Sphere(9,   Vec(50, 9, 70),         new Material(Vec(), Vec(.99, .99, .82), REFR, 1.5)),

    // scene 2 18-20
    new Sphere(16.5,Vec(27,16.5,47),        new Material(Vec(), Vec(.58, .88, .82), DIFF, 1.5)),
    new Sphere(9,   Vec(40, 9, 75),         new Material(Vec(), Vec(.99, .99, .82), GLOS, 1.5)),
    new TriangleMesh("./cornellbox/tallbox.obj",  new Material(Vec(), Vec(.75, .75, .75), DIFF, 1.5)),

    // scene 3 21-22
    new TriangleMesh("./cornellbox/shortbox.obj", materials[2]),
    new TriangleMesh("./cornellbox/tallbox.obj",  new Material(Vec(), Vec(.75, .75, .75), SPEC, 1.5)),

};

Scene::Scene(int id)
{
    vector<int> obj_id;
    if(id == 1) {
        obj_id = { 0, 1, 2, 3, 4, 5, 14, 15, 16, 17, 6 };
    }
    else if(id == 2) {
        obj_id = { 0, 1, 2, 3, 4, 5, 18, 19, 20, 13 };
    }
    else {
        obj_id = { 0, 1, 2, 3, 4, 5, 21, 22, 13};
    }

    num = obj_id.size();
    for(int id : obj_id)
        objs.push_back(objects[id]);
}

bool Scene::intersect(const Ray& r, Hit& h)
{
    for(int i = 0; i < num; i++) {
        objs[i]->intersect(r, h);
    }
    return h.t < INF;
}
