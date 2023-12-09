#ifndef TEXTURE_H
#define TEXTURE_H

#include "blines_general.h"
#include "color.h"

class texture{
public:
    virtual ~texture() = default;

    virtual color value(double u, double v, const point3& p) const = 0;
};

class solid_color : public texture{
public:
    solid_color(color c) : color_value(c) {}

    solid_color(double red, double green, double blue) : color_value(color(red, green, blue)) {} 

    color value(double u, double v, const point3& p) const override {
        return color_value;
    }

private:
    color color_value;
};

class checker_texture : public texture {
public:
    checker_texture(double _scale, shared_ptr<texture> _even, shared_ptr<texture> _odd)
        : inv_scale(1.0 / _scale), even(_even), odd(_odd) {}

    checker_texture(double _scale, color c1, color c2)
        : inv_scale(1.0 / _scale), 
        even(make_shared<solid_color>(c1)), 
        odd(make_shared<solid_color>(c2)) {}

    color value(double u, double v, const point3& p) const override {
        auto xint = static_cast<int>(std::floor(inv_scale * p.x()));
        auto yint = static_cast<int>(std::floor(inv_scale * p.y()));
        auto zint = static_cast<int>(std::floor(inv_scale * p.z()));

        bool is_odd = (xint + yint + zint) % 2;
    
        return is_odd ? odd->value(u, v, p) : even->value(u, v, p);
    }
private:
    double inv_scale;
    shared_ptr<texture> even;
    shared_ptr<texture> odd;
};

#endif