
#include <cmath>
#include <iostream>
#include "util.h"
#include "scene_object.h"


bool SceneObject::intersect(Ray3D& worldRay, const Matrix4x4& worldToModel,
	const Matrix4x4& modelToWorld) {
	const Point3D& rayOrigin = worldToModel * worldRay.origin;
	Vector3D rayDir = worldRay.dir;
	// model to world t scaling constant for the ray
	double mwtScale = worldToModel.applyNorm1(rayDir);
	Intersection& it = worldRay.intersection;
	double tModel = find_tModel(rayOrigin, rayDir, it ? it.t_value / mwtScale : -1);
	
	if (tModel < 0) {
		return false;
	}
	it.t_value = tModel * mwtScale;
	it.point = worldRay.origin + it.t_value * worldRay.dir;
	it.yes = true;
	it.normal = worldToModel.transNorm(normal);
	it.param = param;
	return true;
}

double UnitSquare::find_tModel(const Point3D& rayOrigin, const Vector3D& rayDir, double tMax) {
	double zRatio;
	// NOTE the square is invisible from behind
	// if we are looking down at the square
	if (rayOrigin[2] > 0 && rayDir[2] < 0) {
		zRatio = -rayOrigin[2] / rayDir[2];
		// and the square is in front of previous intersections
		if ((tMax < 0 || tMax > zRatio) // and is also in the square
			&& abs(rayOrigin[0] + rayDir[0] * zRatio) <= 0.5
			&& abs(rayOrigin[1] + rayDir[1] * zRatio) <= 0.5) {
			normal = the_Point3D(0, 0, 1);
			return zRatio;
		}
	}
	return -1;
}



double UnitSphere::find_tModel(const Point3D& rayOrigin, const Vector3D& rayDir, double tMax) {
	// We assume we look outside at the sphere; the sphere is invisible if camera is inside
	Vector3D originVector = rayOrigin.asVec();
	double origin_dot_dir = originVector.dot(rayDir);
	// Don't bother drawing if ray points away from centre
	if (origin_dot_dir > 0) {
		return -1;
	}
	// We solve the equation as 0=at^2+2bt+c
	// where a = norm^2(origin-0)
	// b = (origin-0).direction
	// c = norm^2(direction)-1
	// here b is called origin_dot_dir and direction is normalized so c=-1
	double tModel;
	double discriminant = origin_dot_dir * origin_dot_dir - originVector.length2() + 1;
	if (discriminant >= 0) {
		tModel = -origin_dot_dir - sqrt(discriminant);
		if (tModel > 0 && (tMax < 0 || tModel < tMax)) {
			normal = originVector + tModel * rayDir;
			return tModel;
		}
	}
	return -1;
}

double InfiniteCylinder::find_tModel(const Point3D& rayOrigin, const Vector3D& rayDir, double tMax) {
	// We assume we look outside at the cylinder; the sphere is invisible if camera is inside
	Vector3D originVector = rayOrigin.asVec();
	const double& x0proj = rayOrigin[0];
	const double& y0proj = rayOrigin[1];
	const double& dxproj = rayDir[0];
	const double& dyproj = rayDir[1];
	// We solve the equation on only the xy dimensions 0=at^2+2bt+c
	// where a = norm^2(origin-0)
	// b = (origin-0).direction
	// c = norm^2(direction)-1
	double a = dxproj * dxproj + dyproj * dyproj;
	if (a == 0) { // we are looking directly vertical
		return -1;
	}
	double b = dxproj * x0proj + dyproj * y0proj;
	double c = x0proj * x0proj + y0proj * y0proj - 1;
	// Don't bother drawing if ray points away from centre
	if (b > 0) {
		return -1;
	}
	double tModel;
	double discriminant = b * b - a * c;
	if (discriminant >= 0) {
		tModel = -(b + sqrt(discriminant)) / a;
		if (tModel > 0 && (tMax < 0 || tModel < tMax)) {
			normal = the_Vector3D(tModel * dxproj + x0proj, tModel * dyproj + y0proj, 0);
			param = rayOrigin + tModel * rayDir;
			return tModel;
		}
	}
	return -1;
}


double XYPlane::find_tModel(const Point3D& rayOrigin, const Vector3D& rayDir, double tMax) {
	double zRatio;
	// NOTE the plane is invisible from below
	// We can only see it if we are looking down at the plane
	if (rayOrigin[2] > 0 && rayDir[2] < 0) {
		zRatio = -rayOrigin[2] / rayDir[2];
		// and the square is in front of previous intersections
		if (tMax < 0 || tMax > zRatio) {
			normal = the_Point3D(0, 0, 1);
			param = the_Point3D(rayOrigin[0] + rayDir[0] * zRatio,
				rayOrigin[1] + rayDir[1] * zRatio, 0);
			return zRatio;
		}
	}
	return -1;
}

//// doesn't work idk why :( way too tired to figure it out meh I already got it working from before
//double UnitSquare::find_tModel(const Point3D& rayOrigin, const Vector3D& rayDir, double tMax) {
//	double t = plane.find_tModel(rayOrigin, rayDir, tMax);
//	if (0 <= param[0] && param[0] <= 1 && 0 <= param[1] && param[1] <= 1) {
//		normal = plane.normal;
//		return t;
//	}
//	return -1;
//}

double UnitDisk::find_tModel(const Point3D& rayOrigin, const Vector3D& rayDir, double tMax) {
	double t = plane.find_tModel(rayOrigin, rayDir, tMax);
	if (plane.param[0] * plane.param[0] + plane.param[1] * plane.param[1] <= 1) {
		normal = plane.normal;
		return t;
	}
	return -1;
}

double UnitCylinder::find_tModel(const Point3D& rayOrigin, const Vector3D& rayDir, double tMax) {
	double t;
	Point3D otherOrigin = rayOrigin;
	Vector3D otherDir = rayDir;
	if (1 <= rayOrigin[2] && rayDir[2] < 0) { // looking down from above top cap
		otherOrigin[2] -= 1;
		t = cap1.find_tModel(otherOrigin, otherDir, tMax);
		if (t > 0) {
			normal = cap1.normal;
			param = point3D_0;
			return t;
		}
	}
	else if (-1 >= rayOrigin[2] && rayDir[2] > 0) { // looking up from below bottom cap
		otherOrigin[2] *= -1;
		otherDir[2] *= -1;
		t = cap0.find_tModel(otherOrigin, otherDir, tMax);
		if (t > 0) {
			normal = -cap0.normal;
			param = point3D_0;
			return t;
		}
	}
	t = trunk.find_tModel(rayOrigin, rayDir, tMax);
	if (0 <= trunk.param[2] && trunk.param[2] <= 1) {
		normal = trunk.normal;
		param = trunk.param;
		return t;
	}
	return -1;
}