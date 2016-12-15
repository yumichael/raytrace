/***********************************************************

(Starter code based on code from CGL, University of Waterloo)
		
***********************************************************/

#ifndef _UTIL_
#define _UTIL_

#include <iostream>
#include <cmath>

#ifndef M_PI
#define M_PI	3.14159265358979323846
#endif

#define SMALL_NUM 0.0000000000001
#define DIST_OFF  0.0000000000001


#ifndef MAX
#define MAX(X, Y) (X > Y ? X : Y)
#endif
#ifndef MIN
#define MIN(X, Y) ((X > Y) ? (Y) : (X))
#endif

#define SQUARE(X) ((X) * (X))

class Material;

// from http://stackoverflow.com/questions/1640258/need-a-fast-random-generator-for-c
static unsigned int __g_seed;
inline void fast_srand(int seed) {
	__g_seed = seed;
}
inline int fast_rand() {
	__g_seed = (214013 * __g_seed + 2531011);
	return (__g_seed >> 16) & 0x7FFF;
}
#define RAND01() (double(fast_rand())/RAND_MAX)

// fast power function from
// http://stackoverflow.com/questions/101439/the-most-efficient-way-to-implement-an-integer-based-power-function-powint-int
double power(double x, int n);

// a cruder but faster pow; used if the actual accuracy is not so much
// desired as general function property of amplifying the log magnitude of x
double crudepow(double x, double y);


struct Vector3D {
	double& operator[](int i);  
	double operator[](int i) const;  

	double length2() const; // length squared
	double length() const;
	double normalize(); 
	double normalize1(); // returns reciprocal of length instead
	double dot(const Vector3D& other) const; 
	Vector3D cross(const Vector3D& other) const; 

	double m_data[3];
};

struct Point3D {
	double& operator[](int i); 
	double operator[](int i) const;
	Vector3D asVec() const;

	double m_data[3];
};
const Point3D point3D_0{ 0, 0, 0 };
#define the_Point3D(x,y,z) {{x,y,z}}
const Vector3D vector3D_0{ { 0, 0, 0 } };
#define the_Vector3D(x,y,z) {{x,y,z}}

// standard operators on points and vectors
Vector3D operator *(double s, const Vector3D& v);
Vector3D operator *=(Vector3D& u, double s); 
Vector3D operator +(const Vector3D& u, const Vector3D& v); 
Point3D operator +(const Point3D& u, const Vector3D& v); 
Vector3D operator -(const Point3D& u, const Point3D& v); 
Vector3D operator -(const Vector3D& u, const Vector3D& v); 
Vector3D operator -(const Vector3D& u); 
Point3D operator -(const Point3D& u, const Vector3D& v); 
Vector3D cross(const Vector3D& u, const Vector3D& v); 
std::ostream& operator <<(std::ostream& o, const Point3D& p); 
std::ostream& operator <<(std::ostream& o, const Vector3D& v);

struct Matrix4x4 {
	Matrix4x4 transpose() const;
	double operator()(int i, int j) const;
	double& operator()(int i, int j);
	// applies matrix to v, but does NOT normalize
	Vector3D operator *(const Vector3D& v) const;
	// make v = (*this) * v and also returns length of v before
	virtual double applyNorm(Vector3D& v) const;
	// similar as above but returns 1/length v
	virtual double applyNorm1(Vector3D& v) const;
	virtual Vector3D transNorm(const Vector3D& n) const;

	Matrix4x4(); // constructs identity matrix
	double m_data[16];
};

Matrix4x4 operator *(const Matrix4x4& M, const Matrix4x4& N);
Point3D operator *(const Matrix4x4& M, const Point3D& p);
// Multiply n by the transpose of M, useful for transforming normals.  
// Recall that normals should be transformed by the inverse transpose 
// of the matrix.
std::ostream& operator <<(std::ostream& os, const Matrix4x4& M); 


struct Similarity4x4 : public Matrix4x4 {
	//double m_scaled[16];
	double scale1; // scale1 == 1.0 / (scaling constant of matrix)
	Vector3D operator *(const Vector3D& v) const;
	// make v = (*this) * v and also returns length of v before
	double applyNorm(Vector3D& v) const;
	// similar as above but returns 1/length v
	double applyNorm1(Vector3D& v) const;
	Vector3D transNorm(const Vector3D& n) const;

	Similarity4x4(); // return identity matrix by default
	Similarity4x4(Matrix4x4 init, double scale1);
};
const Similarity4x4 matrix4x4_id = Similarity4x4();
Similarity4x4 operator *(const Similarity4x4& M, const Similarity4x4& N);

struct Colour {
	Colour& operator +=(const Colour& other);
	Colour& operator -=(const Colour& other);
	Colour& operator *=(const Colour& other);
	Colour& operator *=(double s);
	Colour& operator /=(double s);
	Colour operator *(const Colour& other);
	double& operator[](int i);  
	double operator[](int i) const;
	double lum() const;
    
	void clamp(); 	

	double m_data[3];
};
const Colour colour_0{ { 0, 0, 0 } };
const Colour colour_1{ { 1, 1, 1 } };
#define the_Colour(r,g,b) {{r,g,b}}

Colour operator *(double s, const Colour& c);
Colour operator *(const Colour& c, double s);
Colour operator /(const Colour& c, double s);
Colour operator /(const Colour& c, int s);
Colour operator +(const Colour& u, const Colour& v); 
Colour operator -(const Colour& u, const Colour& v);
bool operator==(const Colour& u, const Colour& v);
Colour max(const Colour& u, const Colour& v);
Colour min(const Colour& u, const Colour& v);
std::ostream& operator <<(std::ostream& o, const Colour& c);

struct Intersection {
	// Location of intersection.
	Point3D point;
	// Normal at the intersection.
	Vector3D normal;
	// Extra parameter (e.g. the u,v in F(u,v)) for texture generation
	Point3D param;
	// Position of the intersection point on your ray.
	// (i.e. point = ray.origin + t_value * ray.dir)
	// This is used when you need to intersect multiply objects and
	// only want to keep the nearest intersection.
	double t_value;
	// Set to false when no intersection has occured.
	bool yes;
	// the (positive if looking onto the surface) cosine between incident ray and normal
	double cosine;
	// the vector that would result from perfect mirror reflection
	Vector3D reflect;
	operator bool() const {
		return yes;
	}
};
const Intersection intersection_none{};

// Ray structure. 
struct Ray3D {
	// Origin and direction of the ray.
	Point3D origin;
	Vector3D dir;
	// Hit points, i.e. how many more tmies it can reflect/refract
	double hp;
	// Intersection status, should be computed by the intersection
	// function.
	Intersection intersection;
	// Current colour of the ray, should be computed by the shading
	// function.
	Colour col;
	bool isShadow;
};
#define Ray3D_ray(origin, dir, hp) {origin, dir, hp, intersection_none, colour_0, false}
#endif





