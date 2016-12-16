
#include "material.h"
#include "light_source.h"
#include <cmath>
#include <iostream>
#include <ctime>
#include "the_scene.h"


Colour mandelbrot(const Point3D& param);
Colour julia(const Point3D& param);

PhongMaterial drywall(the_Colour(0.3, 0.3, 0.3), the_Colour(0.3, 0.3, 0.3), the_Colour(0.3, 0.3, 0.3), 2);
PhongMaterial drywallb(the_Colour(0.25, 0.25, 0.35), the_Colour(0.25, 0.25, 0.35), the_Colour(0.3, 0.3, 0.3), 2);
PhongMaterial drywallg(the_Colour(0.25, 0.35, 0.25), the_Colour(0.25, 0.35, 0.25), the_Colour(0.3, 0.3, 0.3), 2);
PhongMaterial drywallr(the_Colour(0.35, 0.25, 0.25), the_Colour(0.35, 0.25, 0.25), the_Colour(0.3, 0.3, 0.3), 2);
TexturedPhongMaterial drywallf(the_Colour(0.2, 0.2, 0.2), the_Colour(0.2, 0.2, 0.2), the_Colour(0.3, 0.3, 0.3), 2, julia);
PolishedMaterial gloss({ { 1, 1, 1 } }, { { 0, 0, 0 } }, 50, 4);
GlowingMaterial heat(the_Colour(1, 0, 0), 0);


PhongMaterial gold (the_Colour(0.3, 0.1, 0.1), the_Colour(0.75164, 0.60648, 0.22648), the_Colour(0.628281, 0.555802, 0.366065), 51.2);
PhongMaterial jade (the_Colour(0.13, 0.2, 0.13), the_Colour(0.54, 0.89, 0.63), the_Colour(0.316228, 0.316228, 0.316228), 12.8);
PhongMaterial aqua (the_Colour(0.15, 0.23, 0.23), the_Colour(0.24, 0.49, 0.93), the_Colour(0.116228, 0.416228, 0.366228), 32.8);
GlowingMaterial white (the_Colour(1, 1, 1), 0);

TexturedPhongMaterial pattern(.8 * aqua.ambient, aqua.diffuse / 4, aqua.diffuse / 4, 2, mandelbrot);
PolishedPhongMaterial metal = PolishedPhongMaterial(gold, the_Colour(.7, .4, .4), colour_0, 200, 3);
PolishedPhongMaterial shiningmirror = PolishedPhongMaterial(colour_0, colour_0, colour_1, 100, colour_1, colour_0, -1, -1);
PolishedMaterial perfectmirror = PolishedMaterial(colour_1, colour_0, -1, -1);


GlowingMaterial bright({ { 1, 1, 1 } }, 0);

PointLight light0 = PointLight(the_Point3D(8, 8, 15), the_Colour(0.8, 0.8, 0.8));
SphereLight light1 = SphereLight(the_Point3D(2, 3, 8), .7, 3, the_Colour(0.9, 0.9, 0.9), .04, 0.004, &white);
SphereLight light2 = SphereLight(the_Point3D(1, 3, 4), .4, 3, the_Colour(12, 12, 12), 0, 9, &white);

SquareLight toplight({ { 8, 8, 15.5 } }, 1.3, 8, { { .9, .9, .9 } }, 0, 0.01, &bright);
SphereLight balllight = SphereLight({ { 9, 7, 2.8 } }, .8, 4, colour_1, 0, .25, &bright);

/******************************************************************************************************************************
             CHANGE SETTINGS RIGHT BELOW
*******************************************************************************************************************************/

TheScene::TheScene() :
screen_width(192),
screen_height(108),
fov(75),
aa_pixel(2), // points per side on pixel to sample for anti aliasing
ray_hp(2) // ray "hit points", i.e. recursion depth control

// the current settings above lets the scene render in a few seconds on Release settings
{
	configure_rendering();
	add_views();


	// Add walls
	SceneDagNode* walls = rt.addObject(NULL, NULL);
	rt.scale(walls, point3D_0, { { 16, 16, 16 } });
	SceneDagNode* wback = rt.addObject(walls, new XYPlane(), &drywallr);
	rt.rotate(wback, 'x', 90);
	rt.translate(wback, { { 0, 0, -1 } });
	SceneDagNode* wleft = rt.addObject(walls, new XYPlane(), &drywallg);
	rt.rotate(wleft, 'y', 90);
	rt.translate(wleft, { { -1, 0, 0 } });
	SceneDagNode* wright = rt.addObject(walls, new XYPlane(), &drywallb);
	rt.rotate(wright, 'y', -90);
	rt.translate(wright, { { 0, 0, -1 } });
	SceneDagNode* floor = rt.addObject(walls, new XYPlane(), &drywallf);
	SceneDagNode* ceiling = rt.addObject(walls, new XYPlane(), &perfectmirror);
	rt.rotate(ceiling, 'x', -180);
	rt.translate(ceiling, { { 0, -1, -1 } });
	SceneDagNode* wfront = rt.addObject(walls, new XYPlane(), &drywall);
	rt.rotate(wfront, 'x', -90);
	rt.translate(wfront, { { 0, -1, 0 } });

	// Add light source.
	//rt.addLightSource(&light0);
	toplight.rotate('x', 180);
	//rt.addLightSource(&light0);
	rt.addObjectLight(&toplight);
	//rt.addObjectLight(&light2);

	// Add objects of interest
	SceneDagNode* cylr = rt.addObject(new UnitCylinder(), &metal);
	rt.translate(cylr, { { 14, 9, 0 } });
	rt.scale(cylr, point3D_0, { { 1.75, 1.75, 6 } });
	SceneDagNode* cylg = rt.addObject(new UnitCylinder(), &jade);
	rt.translate(cylg, { { 8, 12, 0 } });
	rt.scale(cylg, point3D_0, { { 1.5, 1.5, 3 } });
	SceneDagNode* cylb = rt.addObject(new UnitCylinder(), &pattern);
	rt.translate(cylb, { { 9, 7, 0 } });
	rt.scale(cylb, point3D_0, { { 2, 2, 2 } });

	SceneDagNode* ballr = rt.addObject(new UnitSphere(), &gloss);
	rt.translate(ballr, { { 14, 9, 7.75 } });
	rt.scale(ballr, point3D_0, { { 1.75, 1.75, 1.75 } });
	SceneDagNode* ballg = rt.addObject(new UnitSphere(), &shiningmirror);
	rt.translate(ballg, { { 8, 12, 5 } });
	rt.scale(ballg, point3D_0, { { 2, 2, 2 } });
	
	rt.addObjectLight(&balllight);
	
}

void TheScene::take(int i) {
	// the "takes" for renering each snapshot
	switch (i) {
	case 1:
		rt.render(screen_width, screen_height, eye[1], view[1], view[0], fov, "view1.bmp");
		break;
	case 2:
		//rt.render(screen_width, screen_height, eye[2], view[2], view[0], fov, "view2.bmp");
		break;
	case 3:
		//rt.render(screen_width, screen_height, eye[3], view[3], view[0], fov, "view3.bmp");
		break;
	}
}


void TheScene::add_views() {
	eye[1] = the_Point3D(3.5, 1, 8);
	view[1] = the_Vector3D(4.3, 5, -3);
	view[0] = the_Vector3D(0, 0, 1);

	eye[2] = the_Point3D(10, 1, 10);
	view[2] = the_Vector3D(-2, 4, -1);

	eye[3] = the_Point3D(8, 1, 5);
	view[3] = the_Vector3D(0, 5, 1);
}

void TheScene::configure_rendering() {
	rt.aa_pixel = aa_pixel;
	rt.ray_hp = ray_hp;
}

// these fractal functions below were coded by me but with strong help from the internet
Colour mandelbrot(const Point3D& param) {
	double escape = 64;
	int max_iter = 256;
	double reZ = 0;
	double imZ = 0;
	double reZ2 = 0;
	double imZ2 = 0;
	double reC = (param[0]) * 2 * param[2];
	double imC = (param[1]) * 2 * param[2];
	int i;
	for (i = 0; i < max_iter; ++i) {
		imZ = 2 * reZ * imZ + imC;
		reZ = reZ2 - imZ2 + reC;
		reZ2 = reZ * reZ;
		imZ2 = imZ * imZ;
		if (reZ2 + imZ2 > escape) {
			break;
		}
	}
	double modulus = sqrt(reZ2 + imZ2);
	if (i == max_iter || modulus <= 1) {
		return colour_0;
	}
	double mu = 1 + i - (log(log(sqrt(modulus)))) / log(2.0);
	// some fancy try and see math stuff for getting cool colours
	Colour col = the_Colour(1 - 2 / mu - sin(mu) / mu, 1 - 3 / mu + cos(mu) / mu, 1 / (mu + 1) + atan((mu - 50) / 50) / M_PI / 1.5 + .5);
	col *= .9;
	col.clamp();
	return col;
	//atan((mu - 20) / 10) / M_PI * .7 + .5,
	//atan((mu - 40) / 10) / M_PI * .7 + .5,
	//atan((mu / 5) / 10) / M_PI * .7 + .5
	//);
}


Colour julia(const Point3D& param) {
	const int n = 256;
	double radius = 64;
	const double phi = (1 + sqrt(5)) / 2;
	const double Re_c = phi - 2;
	const double Im_c = phi - 1;
	double Re_z = (param[0] - .5) * 1.5;
	double Im_z = (param[1] - .5) * 1.5;
	double Re_z2;
	double Im_z2;
	double modulus;
	double s = 0;
	int i;
	for (i = 0; i < n; ++i) {
		Re_z2 = Re_z * Re_z;
		Im_z2 = Im_z * Im_z;
		modulus = sqrt(Re_z2 + Im_z2);
		if (modulus > radius) {
			break;
		}
		s += exp(-modulus);
		Im_z = 2 * Re_z * Im_z + Im_c;
		Re_z = Re_z2 - Im_z2 + Re_c;
	}
	s = s / n;
	s = tan(s * M_PI / 2);
	// again colouring formula was just try and see
	Colour col = the_Colour(.5 - .5 * sin(5 * s), .5 + .5 * cos(12 * s), .5 + .5 * cos(9 * s));
	col *= 1 - 1 / pow(s + 1, 2);
	return col;

}
