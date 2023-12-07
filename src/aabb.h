#ifndef AABB_H
#define AABB_H

#include "blines_general.h"

class aabb{
public:
    interval x, y, z;

    aabb() {}

    aabb(const interval& ix, const interval& iy, const interval& iz)
        : x(ix), y(iy), z(iz) {}

    aabb(const point3& a, const point3& b){
        x = interval(fmin(a[0], b[0]), fmax(a[0], b[0]));
        y = interval(fmin(a[1], b[1]), fmax(a[1], b[1]));
        z = interval(fmin(a[2], b[2]), fmax(a[2], b[2]));
    }

    const interval& axis(int n) const {
        if(n == 1) return y;
        if(n == 2) return z;
        return x;
    }

    bool hit(const ray& r, interval& ray_t) const {
        for(int a = 0; a < 3; ++a){
            double h1 = (axis(a).min - r.origin()[a]) / r.direction()[a];
            double h2 = (axis(a).max - r.origin()[a]) / r.direction()[a]; 
            double t0 = fmin(h1, h2);
            double t1 = fmax(h1, h2);

            ray_t.min = fmax(t0, ray_t.min);
            ray_t.max = fmin(t1, ray_t.min);

            if(ray_t.max <= ray_t.min)
                return false;
        }
        return true;
    }
};

#endif