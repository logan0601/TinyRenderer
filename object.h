#ifndef OBJECT_H
#define OBJECT_H

#include "ray.h"
#include "material.h"
#include <vector>

using std::vector;

class Object
{
public:
    Material* m;
    Object() = delete;
    Object(Material* m_);
    virtual bool intersect(const Ray& r, Hit& h) = 0;
    virtual void print() = 0;

};

class Sphere : public Object
{
public:
    Vec p;
    double rad;
    Sphere() = delete;
    Sphere(double r_, const Vec& p_, Material* m_);
    bool intersect(const Ray& r, Hit& h);
    void print();

};

class Triangle : public Object
{
public:
    Vec v[3], n;
    Triangle() = delete;
    Triangle(const Vec& a, const Vec& b, const Vec& c, Material* m_);
    bool intersect(const Ray& r, Hit& h);
    void print();

};

class TriangleMesh : public Object
{
public:
    vector<Vec> vertex;
    vector<vector<int>> faces;
    TriangleMesh() = delete;
    TriangleMesh(const char* path, Material* m_);
    bool intersect(const Ray& r, Hit& h);
    void print();

};


class Scene
{
public:
    int num;
    vector<Object*> objs;
    Scene(int id);
    bool intersect(const Ray& r, Hit& h);

};

#endif // OBJECT_H
