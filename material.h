
#ifndef _MATERIAL_
#define _MATERIAL_

#include "util.h"
#include "raytracer.h"
#include "light_source.h"

class Material {
public:
	// Precalculate things like cosines and normalize vectors
	virtual void setup(Raytracer* rt, Ray3D& ray);
	// PRECONDITION: setup has been called first
	virtual Colour shader(Raytracer* rt, Ray3D& ray, LightListNode* ll, SceneDagNode* objnode) = 0;
	virtual bool hasShadow() {
		return true;
	}
};

class PhongMaterial : public virtual Material {
public:
	PhongMaterial(Colour ambient, Colour diffuse, Colour specular, double exp) :
		ambient(ambient), diffuse(diffuse), specular(specular), specular_exp(exp) {}

	// Ambient components for Phong shading.
	Colour ambient;
	// Diffuse components for Phong shading.
	Colour diffuse;
	// Specular components for Phong shading.
	Colour specular;
	// Specular expoent.
	double specular_exp;
	// Called to add texture
	Colour shader(Raytracer* rt, Ray3D& ray, LightListNode* ll, SceneDagNode* objnode);
};

class PolishedMaterial : public virtual Material {
public:
	PolishedMaterial(Colour reflectance, Colour refractance, double gloss_exp, int sampleDensity) :
		reflectance(reflectance), refractance(refractance), gloss_exp(gloss_exp), sampleDensity(sampleDensity) {}
	Colour reflectance;
	Colour refractance;
	double gloss_exp;
	Colour shader(Raytracer* rt, Ray3D& ray, LightListNode* ll, SceneDagNode* objnode);

	int sampleDensity; // suggests number of times to sample for reflection and refraction
};

class PolishedPhongMaterial : public PhongMaterial, public PolishedMaterial {
public:
	PolishedPhongMaterial(Colour ambient, Colour diffuse, Colour specular, double exp,
		Colour reflectance, Colour refractance, double gloss_exp, int sampleDensity) :
		PhongMaterial(ambient, diffuse, specular, exp),
		PolishedMaterial(reflectance, refractance, gloss_exp, sampleDensity) {}
	PolishedPhongMaterial(PolishedMaterial polished,
		Colour ambient, Colour diffuse, Colour specular, double exp) :
		PhongMaterial(ambient, diffuse, specular, exp),
		PolishedMaterial(polished) {}
	PolishedPhongMaterial(PhongMaterial phong,
		Colour reflectance, Colour refractance, double gloss_exp, int sampleDensity) :
		PhongMaterial(phong),
		PolishedMaterial(reflectance, refractance, gloss_exp, sampleDensity) {}
	Colour shader(Raytracer* rt, Ray3D& ray, LightListNode* ll, SceneDagNode* objnode);
};

class TexturedMaterial : public virtual Material {
public:
	TexturedMaterial(Colour(&textureMap)(const Point3D& param)) : textureMap(textureMap) {}
	Colour (&textureMap)(const Point3D& param);
	//Colour shader(Raytracer* rt, Ray3D& ray, LightListNode* ll, SceneDagNode* objnode);
};

class TexturedPhongMaterial : public TexturedMaterial {
public:
	TexturedPhongMaterial(Colour ambient, Colour diffuse, Colour specular, double exp,
		Colour(&textureMap)(const Point3D& param)) : TexturedMaterial(textureMap),
		phong(ambient, diffuse, specular, exp) {}
	TexturedPhongMaterial(PhongMaterial phong, Colour(&textureMap)(const Point3D& param)) :
		phong(phong), TexturedMaterial(textureMap) {}
	Colour shader(Raytracer* rt, Ray3D& ray, LightListNode* ll, SceneDagNode* objnode);

	PhongMaterial phong;
};

class GlowingMaterial : public virtual Material {
public:
	GlowingMaterial(Colour glow, double spread) : glow(glow), spread(spread) {}
	Colour glow;
	double spread;
	Colour shader(Raytracer* rt, Ray3D& ray, LightListNode* ll, SceneDagNode* objnode);
	bool hasShadow() {
		return false;
	}
};

#endif