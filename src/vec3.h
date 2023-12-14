#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

using std::sqrt;

class vec3{
public:
    double e[3];
    
    vec3() : e{0, 0, 0} {}
    vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

    double x() const {
        return e[0];
    }
    double y() const {
        return e[1];
    }
    double z() const {
        return e[2];
    }

    vec3 operator-() const {
        return vec3(-e[0], -e[1], -e[2]);
    }
    double operator[](int i) const {
        return e[i];
    }
    double& operator[](int i) {
        return e[i];
    }

    vec3& operator+=(const vec3& other){
        e[0] += other.e[0];
        e[1] += other.e[1];
        e[2] += other.e[2];
        return *this;
    }
    vec3& operator-=(const vec3& other){
        e[0] -= other.e[0];
        e[1] -= other.e[1];
        e[2] -= other.e[2];
        return *this;
    }
    vec3& operator*=(double t){
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }
    vec3& operator/=(double t) {
        return (*this) *= 1/t;
    }
    double length() const {
        return sqrt(length_squared());
    }
    double length_squared() const {
        return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
    }

    bool near_zero() const {
        double eps = 1e-8;
        return (fabs(e[0]) < eps) && (fabs(e[1]) < eps) && (fabs(e[2]) < eps);
    }

    static vec3 random(){
        return vec3(random_double(), random_double(), random_double());
    }

    static vec3 random(double min, double max){
        return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
    }
};

using point3 = vec3;

inline std::ostream& operator<<(std::ostream &out, const vec3& v){
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3& v, const vec3& u){
    return vec3(v.e[0] + u.e[0], v.e[1] + u.e[1], v.e[2] + u.e[2]);
}

inline vec3 operator-(const vec3& v, const vec3& u){
    return vec3(v.e[0] - u.e[0], v.e[1] - u.e[1], v.e[2] - u.e[2]);
}

// not really mathematical
inline vec3 operator*(const vec3& v, const vec3& u){
    return vec3(v.e[0] * u.e[0], v.e[1] * u.e[1], v.e[2] * u.e[2]);
}

inline vec3 operator*(const vec3& v, const double t){
    return vec3(v.e[0] * t, v.e[1] * t, v.e[2] * t);
}

inline vec3 operator*(const double t, const vec3& v){
    return v * t;
}

inline vec3 operator/(const vec3& v, double t){
    return (1/t) * v;
}

inline double dot(const vec3& v, const vec3& u){
    return v.e[0] * u.e[0] + v.e[1] * u.e[1] + v.e[2] * u.e[2]; 
}

inline vec3 cross(const vec3& v, const vec3& u){
    return vec3(
        v.e[1] * u.e[2] - v.e[2] * u.e[1],
        v.e[2] * u.e[0] - v.e[0] * u.e[2],
        v.e[0] * u.e[1] - v.e[1] * u.e[0]
    );
}

inline vec3 unit_vector(const vec3& v){
    return v / v.length();
}

inline vec3 random_in_unit_disk(){
    while(true){
        vec3 p = vec3(random_double(-1, 1), random_double(-1, 1), 0);
        if(p.length_squared() < 1){
            return p;
        }
    }
}

inline vec3 random_in_unit_sphere(){
    while(true){
        vec3 p = vec3::random(-1, 1);
        if(p.length_squared() < 1)
            return p;
    }
}

inline vec3 random_unit_vector(){
    return unit_vector(random_in_unit_sphere());
}

inline vec3 random_on_hemisphere(const vec3& normal){
    vec3 on_unit_sphere = random_unit_vector();
    return dot(normal, on_unit_sphere) > 0.0 ? on_unit_sphere : -on_unit_sphere;
}

// n has to be a unit vector
inline vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2 * dot(v, n) * n;
}

// v and n must be unit vectors
inline vec3 refract(const vec3& v, const vec3 n, double etai_over_etat){
    double cos_theta = fmin(dot(-v, n), 1.0);
    vec3 r_out_perp = etai_over_etat * (v + cos_theta * n);
    vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_parallel + r_out_perp;
}

inline vec3 random_cosine_direction(){
    double r1 = random_double();
    double r2 = random_double();

    double phi = 2 * pi * r1;
    double x = cos(phi) * sqrt(r2);
    double y = sin(phi) * sqrt(r2);
    double z = sqrt(1 - r2);
    
    return vec3(x, y, z);
}

#endif