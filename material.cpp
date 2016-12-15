/***********************************************************
     Shading functions

***********************************************************/


#include "raytracer.h"
#include "material.h"
#include <cmath>
#include <iostream>

void Material::setup(Raytracer* rt, Ray3D& ray) {
	// PRECONDITION: normal and ray are normalized already
	Intersection &it = ray.intersection;
	it.cosine = -ray.dir.dot(it.normal);
	// note reflect is normalized for free
	it.reflect = ray.dir + 2 * it.cosine * it.normal;
}

Colour PhongMaterial::shader(Raytracer* rt, Ray3D& ray, LightListNode *ll, SceneDagNode* objnode) {
	// NEEDS normal normalized, cosine precalculated, and reflect vector precalculated
	Colour sumcol = colour_0;
	Intersection &it = ray.intersection;
	for (;ll;) {
		Colour col = colour_0;

		// Implementing shadows here
		int numSampled = 0;
		ll->light->beginSampling();
		while (ll->light->sample()) {
			++numSampled;
			Vector3D lightDir = ll->light->position - it.point;
			double lightDist = lightDir.normalize();
			Ray3D lightRay = Ray3D_ray(it.point, lightDir, ray.hp);
			lightRay.isShadow = true;
			rt->traverseScene(lightRay, ll->scenerep, objnode);
			// only colour if scene intersection is further from light sample point
			if (!lightRay.intersection || lightRay.intersection.t_value > lightDist) {
				Colour colour = ll->light->colour;

				// ATTENUATION
				colour /= 1 + ll->light->linAtten * lightDist + ll->light->quadAtten * lightDist;

				// DIFFUSE
				double lightCosine = lightDir.dot(it.normal);
				col += diffuse * (MAX(0, lightCosine) * colour);

				// SPECULAR
				double specularCosine = it.reflect.dot(lightDir);
				col += specular * (crudepow(MAX(0, specularCosine), specular_exp) * colour);

			}
		}
		col /= numSampled;

		sumcol += col;
		ll = ll->next;
	}
	// AMBIENT
	sumcol += ambient;
	return sumcol;
}


Colour PolishedMaterial::shader(Raytracer* rt, Ray3D& ray, LightListNode *ll, SceneDagNode* objnode) {
	// NEEDS ray and normal normalized, cosine precalculated, and reflect vector precalculated
	Intersection &it = ray.intersection;

	if (gloss_exp < 0) { // if parameter is set so we don't have to calc gloss
		Ray3D reflection = Ray3D_ray(it.point, it.reflect, ray.hp - 1);
		return reflectance * rt->shadeRay(reflection, objnode);
	}

	// orthogonal basis for plane perpendicular to reflection
	// need this to sample random offset for reflection vector
	Vector3D perp[2];
	double xypart = it.reflect[0] * it.reflect[0] + it.reflect[1] * it.reflect[1];
	if (xypart > 0) {
		perp[0] = the_Vector3D(
			-sqrt((it.reflect[1] * it.reflect[1]) / xypart),
			sqrt ((it.reflect[0] * it.reflect[0]) / xypart),
			0
		);
	}
	else {
		perp[0] = the_Vector3D(1, 0, 0);
	}
	//perp[0].normalize();
	perp[1] = it.reflect.cross(perp[0]);
	perp[1].normalize();

	Colour refcol = colour_0;

	Sampler sampler;
	sampler.beginSampling(SAMPLE_DISK, sampleDensity);
	int numSampled = 0;
	while (sampler.sample()) {
		// we sample a uniform distribution on the disk
		Vector3D sampled = sampler.getSampled().asVec();
		++numSampled;
		double powscale = sampled.normalize();
		// and then do some math so that the resulting
		// angle is sampled in the same way calculations
		// for specular lighting in the Phong model would
		// imply light ray bouncing off a glossy surface
		// must behave: cosine ^ exponent
		powscale = pow(powscale, 1.0 / gloss_exp);
		sampled *= sqrt(1 - powscale * powscale);
		sampled[2] = powscale;
		Ray3D reflection = Ray3D_ray(
			it.point,
			sampled[0] * perp[0] + sampled[1] * perp[1] + sampled[2] * it.reflect,
			ray.hp / 2
		);
		reflection.dir.normalize();
		refcol += rt->shadeRay(reflection, objnode);
	}
	return reflectance * refcol / numSampled;
}

Colour PolishedPhongMaterial::shader(Raytracer* rt, Ray3D& ray, LightListNode *ll, SceneDagNode* objnode) {
	return PhongMaterial::shader(rt, ray, ll, objnode) + PolishedMaterial::shader(rt, ray, ll, objnode);
}

Colour TexturedPhongMaterial::shader(Raytracer* rt, Ray3D& ray, LightListNode* ll, SceneDagNode* objnode) {
	Colour pcol = phong.shader(rt, ray, ll, objnode);
	return pcol + textureMap(ray.intersection.param);
}

Colour GlowingMaterial::shader(Raytracer* rt, Ray3D& ray, LightListNode* ll, SceneDagNode* objnode) {
	return glow;
}