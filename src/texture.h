#ifndef TEXTURE_H
#define TEXTURE_H

#include "blines_general.h"
#include "rtw_stb_image.h"
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

class image_texture : public texture {
public:
    image_texture(const char* filename) : image(filename) {}

    color value(double u, double v, const point3& p) const override {
        if(image.height() <= 0) return color(0, 1, 1);

        u = interval(0, 1).clamp(u);
        v = 1.0 - interval(0, 1).clamp(v);

        int i = static_cast<int>(u * image.width());
        int j = static_cast<int>(v * image.height());
        const unsigned char* pixel = image.pixel_data(i, j);

        double color_scale = 1.0 / 255.0;
        return color_scale * color(pixel[0], pixel[1], pixel[2]);
    }
private:
    rtw_image image;
};

#endif