#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "onb.h"

class sphere : public hittable{
public:
    sphere(point3 _center, double _radius, shared_ptr<material> _material)
        : center1(_center), radius(_radius), mat(_material), is_moving(false) 
    {
        vec3 rvec = vec3(radius, radius, radius);
        bbox = aabb(center1 - rvec, center1 + rvec);
    }

    sphere(point3 _center1, point3 _center2, double _radius, shared_ptr<material> _material)
        : center1(_center1), radius(_radius), mat(_material), is_moving(true) 
        {
            vec3 rvec = vec3(radius, radius, radius);
            aabb box1(_center1 - rvec, _center1 + rvec);
            aabb box2(_center2 - rvec, _center2 + rvec);
            bbox = aabb(box1, box2); // cover whole motion instead of in the moment, is this better?

            center_vec = _center2 - _center1;
        }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        point3 center = is_moving ? sphere_center(r.time()) : center1; 
        vec3 oc = r.origin() - center;
        double a = r.direction().length_squared();
        double half_b = dot(oc, r.direction());
        double c = oc.length_squared() - radius * radius;
        double discriminant = half_b * half_b - a * c;

        if(discriminant < 0){;
            return false;
        }

        double sqrtd = sqrt(discriminant);
        double root = (-half_b - sqrtd) / a;
        if(!ray_t.surrounds(root)){
            root = (-half_b + sqrtd) / a;
            if(!ray_t.surrounds(root))
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);
        get_sphere_uv(outward_normal, rec.u, rec.v);
        rec.mat = mat;

        return true;
    }

    aabb bounding_box() const override {
        return bbox;
    }

    double pdf_value(const point3& o, const vec3& v) const override {
        // Only for stationary

        hit_record rec;
        if(!this->hit(ray(o, v), interval(0.0001, infinity), rec)){
            return 0;
        }

        double cos_theta_max = sqrt(1 - radius * radius / (center1 - o).length_squared());
        double solid_angle = 2 * pi * (1 - cos_theta_max);

        return 1 / solid_angle;
    }

    vec3 random(const point3& o) const override {
        vec3 direction = center1 - o;
        double distance_squared = direction.length_squared();
        onb uvw;
        uvw.build_from_w(direction);
        return uvw.local(random_to_sphere(radius, distance_squared));
    }

private:
    point3 center1;
    double radius;
    shared_ptr<material> mat;
    bool is_moving;
    vec3 center_vec;
    aabb bbox;

    point3 sphere_center(double time) const {
        return center1 + time*center_vec;
    }

    static void get_sphere_uv(const point3& p, double& u, double& v){
        double theta = acos(-p.y());
        double phi = atan2(-p.z(), p.x()) + pi;
        u = phi / (2 * pi);
        v = theta / pi;
    }

    static vec3 random_to_sphere(double radius, double distance_squared){
        double r1 = random_double();
        double r2 = random_double();
        double z = 1 + r2 * (sqrt(1 - radius * radius / distance_squared) - 1);

        double phi = 2 * pi * r1;
        double x = cos(phi) * sqrt(1 - z * z);
        double y = sin(phi) * sqrt(1 - z * z);

        return vec3(x, y, z);
    }
};

#endif
