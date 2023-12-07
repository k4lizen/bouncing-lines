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

class lambertian : public material {
public:
    lambertian(const color& a) : albedo(a) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
        vec3 scatter_dir = rec.normal + random_unit_vector();
        
        if(scatter_dir.near_zero()){ // degenerate case
            scatter_dir = rec.normal;
        }
        
        scattered = ray(rec.p, scatter_dir);
        attenuation = albedo;
        return true;
    } 

private:
    color albedo; 
};

class metal : public material {
public:
    metal(const color& a)  : albedo(a) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
        scattered = ray(rec.p, reflect(r_in.direction(), rec.normal)); // listing 60, why unit vector?
        attenuation = albedo;
        return true;
    }

private:
    color albedo;
};

#endif
