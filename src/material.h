#ifndef MATERIAL_H
#define MATERIAL_H

#include "blines_general.h"
#include "color.h"
#include "hittable.h"

class material{
public:
    virtual ~material() = default;

    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const = 0; 
};

class lambertian : material {
public:
    lambertian(const color& a) : albedo(a) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
        vec3 scatter_dir = rec.normal + random_unit_vector();
        scattered = ray(rec.p, scatter_dir);
        attenuation = albedo;
        return true;
    } 

private:
    color albedo; 
};

#endif
