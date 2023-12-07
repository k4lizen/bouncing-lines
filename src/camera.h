#ifndef CAMERA_H
#define CAMERA_H

#include "blines_general.h"

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

    camera(std::string _filename) : filename(_filename) {}
    camera() : camera("image.ppm") {}

    void render(const hittable& world){
        initialize();

        std::ofstream image_file("image.ppm");

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
    std::string filename = "image.ppm";
    int image_height;
    point3 center;
    point3 pixel00_loc;
    vec3 pixel_delta_right;
    vec3 pixel_delta_down;

    void initialize(){
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        double viewport_height = 2.0;
        double viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);

        double focal_lenght = 1.0;
        center = point3(0, 0, 0);

        vec3 viewport_right = vec3(viewport_width, 0, 0);
        vec3 viewport_down = vec3(0, -viewport_height, 0);
        
        pixel_delta_right = viewport_right / image_width;
        pixel_delta_down = viewport_down / image_height;

        point3 viewport_upper_left = center - vec3(0, 0, focal_lenght) - viewport_right / 2 - viewport_down / 2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_right + pixel_delta_down);
    }

    color ray_color(const ray& r, int depth, const hittable& world){
        if(depth <= 0){
            return color(0, 0, 0);
        }

        hit_record rec;
        static double acne_eps = 0.0000001; 
        if(world.hit(r, interval(0.0 + acne_eps, infinity), rec)){
            ray scattered;
            color attenuation;
            if(rec.mat->scatter(r, rec, attenuation, scattered)){
                return attenuation * ray_color(scattered, depth - 1, world);
            }
            return color(0, 0, 0);
        }

        // sky
        vec3 unit_direction = unit_vector(r.direction());
        double a = 0.5 * (unit_direction.y() + 1.0); // from [-1, 1] to [0, 1]
        return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
    }

    ray get_ray(int i, int j) const {
        point3 pixel_center = pixel00_loc + (i * pixel_delta_down) + (j * pixel_delta_right);
        point3 pixel_sample = pixel_center + point_sample_square();

        point3 ray_origin = center;
        point3 ray_direction = pixel_sample - ray_origin;
        return ray(ray_origin, ray_direction);
    }

    vec3 point_sample_square() const {
        double px = -0.5 + random_double();
        double py = -0.5 + random_double();
        return (px * pixel_delta_right) + (py * pixel_delta_down);
    }
};


#endif