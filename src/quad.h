#ifndef QUAD_H
#define QUAD_H

#include "blines.h"
#include "hittable.h"

class quad : public hittable {
public:
    quad(const point3& _Q, const vec3& _u, const vec3& _v, shared_ptr<material> _mat)
        : Q(_Q), u(_u), v(_v), mat(_mat)
    {
        vec3 n = cross(u, v);
        normal = unit_vector(n);
        D = dot(normal, Q);
        w = n / dot(n, n);

        set_bounding_box();
    }

    virtual void set_bounding_box() {
        bbox = aabb(Q, Q + u + v).pad();
    }

    aabb bounding_box() const override { 
        return bbox; 
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        double denom = dot(normal, r.direction());
        
        if(fabs(denom) < 1e-8)
            return false;

        double t = (D - dot(normal, r.origin())) / denom;
        if(!ray_t.contains(t))
            return false;

        point3 intersection = r.at(t);
        vec3 planat_hit_pt_vector = intersection - Q;
        double alpha = dot(w, cross(planat_hit_pt_vector, v));
        double beta = dot(w, cross(u, planat_hit_pt_vector));
        
        if(!is_interior(alpha, beta, rec))
            return false;

        rec.t = t;
        rec.p = intersection;
        rec.mat = mat;
        rec.set_face_normal(r, normal);
    
        return true;
    }

    virtual bool is_interior(double a, double b, hit_record& rec) const {
        if((a < 0) || (a > 1) || (b < 0) || (b > 1))
            return false;

        rec.u = a;
        rec.v = b;
        return true;
    }

private:
    point3 Q;
    vec3 u, v;
    shared_ptr<material> mat;
    aabb bbox;
    vec3 normal;
    double D;
    vec3 w;
};

inline shared_ptr<hittable_list> box(const point3& a, const point3& b, shared_ptr<material> mat){
    auto sides = make_shared<hittable_list>();

    point3 min = point3(fmin(a.x(), b.x()), fmin(a.y(), b.y()), fmin(a.z(), b.z()));
    point3 max = point3(fmax(a.x(), b.x()), fmax(a.y(), b.y()), fmax(a.z(), b.z()));

    vec3 dx = vec3(max.x() - min.x(), 0, 0);
    vec3 dy = vec3(0, max.y() - min.y(), 0);
    vec3 dz = vec3(0, 0, max.z() - min.z());

    sides->add(make_shared<quad>(point3(min.x(), min.y(), max.z()), dx, dy, mat));
    sides->add(make_shared<quad>(point3(max.x(), min.y(), max.z()), -dz, dy, mat));
    sides->add(make_shared<quad>(point3(max.x(), min.y(), min.z()), -dx, dy, mat));
    sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()), dz, dy, mat)); 
    sides->add(make_shared<quad>(point3(min.x(), max.y(), max.z()), dx, -dz, mat));
    sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()), dx, dz, mat));

    return sides;
}

#endif