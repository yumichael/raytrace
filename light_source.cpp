
#include <cmath>
#include "light_source.h"
#include "sampler.h"

/*******************************
         POINT LIGHT
********************************/
void PointLight::beginSampling() {
	go = true;
}

bool PointLight::sample() {
	if (go) {
		go = false;
		return true;
	}
	return false;
}

/*****************************
            OBJECT
*******************************/
ObjectLight::~ObjectLight() {
	delete pt_obj;
}

/*******************************
SPHERE LIGHT
********************************/
SphereLight::SphereLight(Point3D centre, double radius, int sample_n, Colour colour,
	double linAtten, double quadAtten, Material* mat) :
	centre(centre), radius(radius), sample_n(sample_n),
	ObjectLight(centre, colour, mat, linAtten, quadAtten)
{
	pt_obj = new UnitSphere();
	translate(centre.asVec());
	scale(point3D_0, { { radius, radius, radius } });
	objmat = mat;
}

void SphereLight::beginSampling() {
	s.beginSampling(SAMPLE_BALL, sample_n);
}

bool SphereLight::sample() {
	if (s.sample()) {
		position = trans * s.getSampled();
		return true;
	}
	return false;
}

/*******************************
SQUARE LIGHT
********************************/
SquareLight::SquareLight(Point3D centre, double halfdiam, int sample_n, Colour colour,
	double linAtten, double quadAtten, Material* mat) :
	centre(centre), radius(halfdiam), sample_n(sample_n),
	ObjectLight(centre, colour, mat, linAtten, quadAtten)
{
	pt_obj = new UnitSquare();
	translate(centre.asVec());
	scale(point3D_0, { { 2 * radius, 2 * radius, 2 * radius} });
	objmat = mat;
}

void SquareLight::beginSampling() {
	s.beginSampling(SAMPLE_SQUARE, sample_n);
}

bool SquareLight::sample() {
	if (s.sample()) {
		Vector3D offset = the_Vector3D(0.5, 0.5, 0);
		position = trans * (s.getSampled() - offset);
		return true;
	}
	return false;
}





/*******************************************************
Duplicated code for matrix transformations from RayTracer
Was impossible to refactor so code is reused, so meh
********************************************************/

void ObjectLight::rotate(char axis, double angle) {
	Matrix4x4 rotation = matrix4x4_id;
	double toRadian = 2 * M_PI / 360.0;
	int i;

	for (i = 0; i < 2; i++) {
		switch (axis) {
		case 'x':
			rotation(0, 0) = 1;
			rotation(1, 1) = cos(angle*toRadian);
			rotation(1, 2) = -sin(angle*toRadian);
			rotation(2, 1) = sin(angle*toRadian);
			rotation(2, 2) = cos(angle*toRadian);
			rotation(3, 3) = 1;
			break;
		case 'y':
			rotation(0, 0) = cos(angle*toRadian);
			rotation(0, 2) = sin(angle*toRadian);
			rotation(1, 1) = 1;
			rotation(2, 0) = -sin(angle*toRadian);
			rotation(2, 2) = cos(angle*toRadian);
			rotation(3, 3) = 1;
			break;
		case 'z':
			rotation(0, 0) = cos(angle*toRadian);
			rotation(0, 1) = -sin(angle*toRadian);
			rotation(1, 0) = sin(angle*toRadian);
			rotation(1, 1) = cos(angle*toRadian);
			rotation(2, 2) = 1;
			rotation(3, 3) = 1;
			break;
		}
		if (i == 0) {
			trans = trans*rotation;
			angle = -angle;
		}
		else {
			invtrans = rotation*invtrans;
		}
	}
}

void ObjectLight::translate(Vector3D transl) {
	Matrix4x4 translation = matrix4x4_id;

	translation(0, 3) = transl[0];
	translation(1, 3) = transl[1];
	translation(2, 3) = transl[2];
	trans = trans*translation;
	translation(0, 3) = -transl[0];
	translation(1, 3) = -transl[1];
	translation(2, 3) = -transl[2];
	invtrans = translation*invtrans;
}

void ObjectLight::scale(Point3D origin, double factor[3]) {
	Matrix4x4 scale = matrix4x4_id;

	scale(0, 0) = factor[0];
	scale(0, 3) = origin[0] - factor[0] * origin[0];
	scale(1, 1) = factor[1];
	scale(1, 3) = origin[1] - factor[1] * origin[1];
	scale(2, 2) = factor[2];
	scale(2, 3) = origin[2] - factor[2] * origin[2];
	trans = trans*scale;
	scale(0, 0) = 1 / factor[0];
	scale(0, 3) = origin[0] - 1 / factor[0] * origin[0];
	scale(1, 1) = 1 / factor[1];
	scale(1, 3) = origin[1] - 1 / factor[1] * origin[1];
	scale(2, 2) = 1 / factor[2];
	scale(2, 3) = origin[2] - 1 / factor[2] * origin[2];
	invtrans = scale*invtrans;
}


void ObjectLight::scale(Point3D origin, Vector3D factor) {
	Matrix4x4 scale = matrix4x4_id;

	scale(0, 0) = factor[0];
	scale(0, 3) = origin[0] - factor[0] * origin[0];
	scale(1, 1) = factor[1];
	scale(1, 3) = origin[1] - factor[1] * origin[1];
	scale(2, 2) = factor[2];
	scale(2, 3) = origin[2] - factor[2] * origin[2];
	trans = trans*scale;
	scale(0, 0) = 1 / factor[0];
	scale(0, 3) = origin[0] - 1 / factor[0] * origin[0];
	scale(1, 1) = 1 / factor[1];
	scale(1, 3) = origin[1] - 1 / factor[1] * origin[1];
	scale(2, 2) = 1 / factor[2];
	scale(2, 3) = origin[2] - 1 / factor[2] * origin[2];
	invtrans = scale*invtrans;
}
