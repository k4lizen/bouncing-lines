#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include "interval.h"
#include <iostream>

using color = vec3;

inline double linear_to_gamma(double linear_component){
    return sqrt(linear_component);
}

void write_color(std::ostream& out, color pixel_color, int samples_per_pixel){
    double r = pixel_color.x();
    double g = pixel_color.y();
    double b = pixel_color.z();

    if(r != r) r = 0;
    if(g != g) g = 0;
    if(b != b) b = 0;

    double scale = 1.0 / samples_per_pixel; // only one division instead of 4
    r *= scale;
    g *= scale;
    b *= scale;

    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    static const interval intensity(0.000, 0.999);
    out << static_cast<int>(256 * intensity.clamp(r)) << ' '
        << static_cast<int>(256 * intensity.clamp(g)) << ' '
        << static_cast<int>(256 * intensity.clamp(b)) << '\n';
}

#endif