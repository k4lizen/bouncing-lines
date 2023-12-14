#ifndef CAMERA_H
#define CAMERA_H

#include "blines.h"

#include "color.h"
#include "hittable.h"
#include "material.h"

#include <iostream>
#include <fstream>
#include <string>

class camera {
public:
    double aspect_ratio = 1.0;
    int image_width = 100;
    int samples_per_pixel = 10;
    int max_depth = 10;
    color background;

    double vfov = 90; // has to be 0 < vfov < 180 
    point3 lookfrom = point3(0, 0, 0);
    point3 lookat = point3(0, 0, -1);
    vec3 vup = vec3(0, 1, 0);

    double defocus_angle = 0;
    double focus_dist = 10;

    camera(std::string _filename) : filename(_filename) {}
    camera() : camera("images\\image.ppm") {}

    void render(const hittable& world){
        initialize();

        std::ofstream image_file(filename);

        image_file << "P3\n" << image_width << " " << image_height << "\n255\n";
        for(int i = 0; i < image_height; ++i){
            std::clog << "\rScanlines remaining: " << (image_height - i) << " ";
            for(int j = 0; j < image_width; ++j){
                color pixel_color(0, 0, 0);
                for(int sample = 0; sample < samples_per_pixel; ++sample){
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth, world);
                }
                write_color(image_file, pixel_color, samples_per_pixel);
            }
        }
        std::clog << "\rDone                                  \n";
        image_file.close();
    }

private:
    std::string filename = "images\\_image.ppm";
    int image_height;
    point3 center;
    point3 pixel00_loc;
    vec3 pixel_delta_right;
    vec3 pixel_delta_down;
    vec3 u, v, w; // camera frame basis vectors (back, up, right)
    vec3 defocus_disk_u, defocus_disk_v;

    void initialize(){
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        center = lookfrom;

        double theta = degrees_to_radians(vfov);
        double h = tan(theta / 2);
        double viewport_height = 2 * h * focus_dist;
        double viewport_width = viewport_height * image_width / image_height;

        w = unit_vector(lookfrom - lookat); // back
        u = unit_vector(cross(vup, w)); // right
        v = cross(w, u); // up

        vec3 viewport_right = viewport_width * u;
        vec3 viewport_down = viewport_height * -v;
        
        pixel_delta_right = viewport_right / image_width;
        pixel_delta_down = viewport_down / image_height;

        point3 viewport_upper_left = center - (focus_dist * w) - viewport_right / 2 - viewport_down / 2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_right + pixel_delta_down);
    
        double defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    color ray_color(const ray& r, int depth, const hittable& world){
        if(depth <= 0){
            return color(0, 0, 0);
        }

        static double acne_eps = 0.0000001; 

        hit_record rec;
        bool hasHit = world.hit(r, interval(0.0 + acne_eps, infinity), rec);
        if(!hasHit){
            return background;
        }

        ray scattered;
        color attenuation;
        color color_from_emmision = rec.mat->emmited(rec.u, rec.v, rec.p);

        bool hasScattered = rec.mat->scatter(r, rec, attenuation, scattered);
        if(!hasScattered){
            return color_from_emmision;
        }

        double scattering_pdf = rec.mat->scattering_pdf(r, rec, scattered);
        double pdf = 1/(2*pi);
        color color_from_scatter = 
            (attenuation * scattering_pdf * ray_color(scattered, depth - 1, world)) / pdf;

        return color_from_emmision + color_from_scatter;
    }

    ray get_ray(int i, int j) const {
        point3 pixel_center = pixel00_loc + (i * pixel_delta_down) + (j * pixel_delta_right);
        point3 pixel_sample = pixel_center + point_sample_square();

        point3 ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        point3 ray_direction = pixel_sample - ray_origin;
        double ray_time = random_double();
        return ray(ray_origin, ray_direction, ray_time);
    }

    vec3 defocus_disk_sample() const {
        point3 p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    vec3 point_sample_square() const {
        double px = -0.5 + random_double();
        double py = -0.5 + random_double();
        return (px * pixel_delta_right) + (py * pixel_delta_down);
    }
};


#endif