
#ifndef _LIGHT_SOURCE_
#define _LIGHT_SOURCE_

#include "util.h"
#include "scene_object.h"
#include "sampler.h"
#include <ctime>


class LightSource {
public:
	LightSource(Point3D position, Colour colour) :
		position(position), colour(colour) {}
	LightSource(Point3D position, Colour colour, double linAtten, double quadAtten) :
		position(position), colour(colour), linAtten(linAtten), quadAtten(quadAtten) {}
	Point3D position;
	Colour colour; // I decided having a colour for each Phong parameter is kinda ...stupid
	double linAtten; // linear attenuation
	double quadAtten; // quadtratic attenuation
	virtual void beginSampling() = 0; // signals class to begin sampling point lights
	virtual bool sample() = 0; // sample a point light, being stored in position
};

class PointLight : public LightSource {
public:
	PointLight(Point3D position, Colour colour) :
		LightSource(position, colour) {}
	PointLight(Point3D position, Colour colour, double linAtten, double quadAtten) :
		LightSource(position, colour, linAtten, quadAtten) {}
	void beginSampling();
	bool sample();
private:
	bool go;
};

class ObjectLight : public LightSource {
	// represents a light that is also an object directly seeable
public:
	ObjectLight(Point3D position, Colour colour, Material* mat) :
		LightSource(position, colour), objmat(mat),
		trans(matrix4x4_id), invtrans(matrix4x4_id) {}
	ObjectLight(Point3D position, Colour colour, Material* mat,
		double linAtten, double quadAtten) :
		LightSource(position, colour, linAtten, quadAtten), objmat(mat),
		trans(matrix4x4_id), invtrans(matrix4x4_id) {}
	virtual ~ObjectLight();
	SceneObject* pt_obj;
	// we have matrices to store how the object is transformed from world
	Matrix4x4 trans;
	Matrix4x4 invtrans;
	// functions like in raytracer to transform object
	void rotate(char axis, double angle);
	void translate(Vector3D transl);
	void scale(Point3D origin, double factor[3]);
	void scale(Point3D origin, Vector3D factor);
	Material* objmat;
};


class SquareLight : public virtual ObjectLight {
public:
	SquareLight(Point3D centre, double halfdiam, int sample_n, Colour colour,
		double linAtten, double quadAtten, Material* mat);
	void beginSampling();
	bool sample();
private:
	Sampler s;
	int sample_n;
	Point3D centre;
	double radius;
};


class SphereLight : public virtual ObjectLight {
public:
	SphereLight(Point3D centre, double radius, int sample_n, Colour colour,
		double linAtten, double quadAtten, Material* mat);
	void beginSampling();
	bool sample();
private:
	Sampler s;
	int sample_n;
	Point3D centre;
	double radius;
};

#endif