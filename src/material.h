#ifndef MATERIAL_H
#define MATERIAL_H

#include "blines.h"
#include "color.h"
#include "hittable.h"
#include "texture.h"
#include "onb.h"

class material{
public:
    virtual ~material() = default;

    virtual color emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p) const {
        return color(0, 0, 0);
    }

    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered, double& pdf) const = 0;

    virtual double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const {
        return 0;
    }
};

class lambertian : public material {
public:
    lambertian(const color& a) : albedo(make_shared<solid_color>(a)) {}

    lambertian(shared_ptr<texture> a) : albedo(a) {}


    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered, double& pdf) const override {
        onb uvw;
        uvw.build_from_w(rec.normal);
        vec3 scatter_dir = uvw.local(random_cosine_direction());
        
        if(scatter_dir.near_zero()){ // degenerate case
            scatter_dir = rec.normal;
        }
        
        scattered = ray(rec.p, scatter_dir, r_in.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        pdf = dot(uvw.w(), scattered.direction()) / pi;
        return true;
    }

    double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const {
        double cos_theta = dot(rec.normal, unit_vector(scattered.direction()));
        return cos_theta < 0 ? 0 : cos_theta / pi;
    }

private:
    shared_ptr<texture> albedo; 
};

class metal : public material {
public:
    metal(const color& a, double f)  : albedo(a), fuzz(f < 1 ? f : 1) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered, double& pdf) const override {
        vec3 reflected_dir = reflect(r_in.direction(), rec.normal);
        scattered = ray(rec.p, reflected_dir + fuzz * random_unit_vector(), r_in.time());
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0); // can be false only if fuzzing put it below the surface
    }

private:
    color albedo;
    double fuzz;
};

class dielectric : public material {
public:
    dielectric(double index_of_refraction) : ir(index_of_refraction){}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered, double& pdf) const override {
        attenuation = color(1.0, 1.0, 1.0);
        double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;
        
        vec3 unit_direction = unit_vector(r_in.direction());
        double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        vec3 direction;

        if(cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double()){
            direction = reflect(unit_direction, rec.normal);
        }else{
            direction = refract(unit_direction, rec.normal, refraction_ratio);
        }

        scattered = ray(rec.p, direction, r_in.time());
        return true;
    }
private:
    double ir;

    static double reflectance(double cosine, double ref_index){
        // Schlick's approximation for reflectance.
        double r0 = (1 - ref_index) / (1 + ref_index);
        r0 *= r0;
        return r0 + (1- r0) * pow(1 - cosine, 5);
    }
};

class diffuse_light : public material {
public:
    diffuse_light(shared_ptr<texture> a) : emit(a) {}
    diffuse_light(color c) : emit(make_shared<solid_color>(c)) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered, double& pdf) const override {
        return false;
    }

    color emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p) const override {
        if(!rec.front_face)
            return color(0, 0, 0);
        return emit->value(u, v, p);
    }

private:
    shared_ptr<texture> emit;
};

class isotropic : public material {
public:
    isotropic(color c) : albedo(make_shared<solid_color>(c)) {}
    isotropic(shared_ptr<texture> a) : albedo(a) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered, double& pdf) const override {
        scattered = ray(rec.p, random_unit_vector(), r_in.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        pdf = 1 / (4 * pi);
        return true;
    }

    double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override {
        return 1 / (4 * pi);
    }

private:
    shared_ptr<texture> albedo;
}; 

#endif
