
#ifndef _SCENE_OBJECT_
#define _SCENE_OBJECT_

#include "util.h"

// All primitives should provide a intersection function.  
// To create more primitives, inherit from SceneObject.
// Namely, you can create, Sphere, Cylinder, etc... classes
// here.
class SceneObject {
public:
	// coordinates indicating local data at an intersection
	// to be reused to pass data for each intersection call
	Vector3D normal;
	// holds data for texture drawing, colour generation, etc
	Point3D param;
	// Returns true if an intersection occured, false otherwise.
	virtual bool intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld);
	// Return the t value for ray intersection in self coordinates.
	// Return negative number if no hit.
	// also fill in the normal and param at the intersection
	virtual double find_tModel(const Point3D& rayOrigin, const Vector3D& rayDir, double tMax) = 0;
};

class XYPlane : public SceneObject {
public:
	double find_tModel(const Point3D& rayOrigin, const Vector3D& rayDir, double tMax);
};

// Example primitive you can create, this is a unit square on the xy-plane with coordinates 0-1
class UnitSquare : public SceneObject {
public:
	double find_tModel(const Point3D& rayOrigin, const Vector3D& rayDir, double tMax);
	XYPlane plane;
};

class UnitDisk : public SceneObject {
public:
	double find_tModel(const Point3D& rayOrigin, const Vector3D& rayDir, double tMax);
	XYPlane plane;
};

class UnitSphere : public SceneObject {
public:
	double find_tModel(const Point3D& rayOrigin, const Vector3D& rayDir, double tMax);
};

class InfiniteCylinder : public SceneObject {
public:
	double find_tModel(const Point3D& rayOrigin, const Vector3D& rayDir, double tMax);
};

class UnitCylinder : public SceneObject {
	// Cylinder with height 1 sitting on xy plane
public:
	double find_tModel(const Point3D& rayOrigin, const Vector3D& rayDir, double tMax);
	InfiniteCylinder trunk;
	UnitDisk cap0;
	UnitDisk cap1;
};

#endif