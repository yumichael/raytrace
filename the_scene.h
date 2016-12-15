
#include "material.h"
#include "light_source.h"
#include <cmath>
#include <iostream>
#include <ctime>


Colour mandelbrot(const Point3D& param);
Colour julia(const Point3D& param);

class TheScene {
public:
	int screen_width = 340;
	int screen_height = 280;
	int fov = 60;
	int aa_pixel = 2;
	int ray_hp = 2;

	// Camera parameters
	Point3D eye[9];
	Vector3D view[9];

	Raytracer rt;
	void configure_rendering();
	void add_views();

	TheScene();
	void take(int i);
};