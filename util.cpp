/*
A lot of boilerplate code. The header is a more useful referenc, as tyically.
*/

#include <cmath>
#include "util.h"

double power(double x, int n) {
	double result = 1;
	while (n)
	{
		if (n & 1)
			result *= x;
		n >>= 1;
		x *= x;
	}

	return result;
}

double crudepow(double x, double y) {
	if (y >= 0) {
		return power(x, floor(y));
	}
	else {
		return 1.0 / power(x, floor(-y));
	}
}


Vector3D Point3D::asVec() const {
	return the_Vector3D(m_data[0], m_data[1], m_data[2]);
}

double& Point3D::operator[](int i) {
	return m_data[i];
}

double Point3D::operator[](int i) const {
	return m_data[i];
}

double& Vector3D::operator[](int i) {
	return m_data[i];
}
double Vector3D::operator[](int i) const {
	return m_data[i];
}

double Vector3D::length2() const
{
	return dot(*this);
}

double Vector3D::length() const
{
	return sqrt(dot(*this));
}

double Vector3D::normalize() {
	double denom = 1.0;
	double x = (m_data[0] > 0.0) ? m_data[0] : -m_data[0];
	double y = (m_data[1] > 0.0) ? m_data[1] : -m_data[1];
	double z = (m_data[2] > 0.0) ? m_data[2] : -m_data[2];

	if(x > y) {
		if(x > z) {
			if(1.0 + x > 1.0) {
				y = y / x;
				z = z / x;
				denom = 1.0 / (x * sqrt(1.0 + y*y + z*z));
			}
		} else { /* z > x > y */ 
			if(1.0 + z > 1.0) {
				y = y / z;
				x = x / z;
				denom = 1.0 / (z * sqrt(1.0 + y*y + x*x));
			}
		}
	} else {
		if(y > z) {
			if(1.0 + y > 1.0) {
				z = z / y;
				x = x / y;
				denom = 1.0 / (y * sqrt(1.0 + z*z + x*x));
			}
		} else { /* x < y < z */
			if(1.0 + z > 1.0) {
				y = y / z;
				x = x / z;
				denom = 1.0 / (z * sqrt(1.0 + y*y + x*x));
			}
		}
	}

	if(1.0 + x + y + z > 1.0) {
		m_data[0] *= denom;
		m_data[1] *= denom;
		m_data[2] *= denom;
		return 1.0 / denom;
	}

	return 0.0;
}

double Vector3D::normalize1() {
	double denom = 1.0;
	double x = (m_data[0] > 0.0) ? m_data[0] : -m_data[0];
	double y = (m_data[1] > 0.0) ? m_data[1] : -m_data[1];
	double z = (m_data[2] > 0.0) ? m_data[2] : -m_data[2];

	if(x > y) {
		if(x > z) {
			if(1.0 + x > 1.0) {
				y = y / x;
				z = z / x;
				denom = 1.0 / (x * sqrt(1.0 + y*y + z*z));
			}
		} else { /* z > x > y */ 
			if(1.0 + z > 1.0) {
				y = y / z;
				x = x / z;
				denom = 1.0 / (z * sqrt(1.0 + y*y + x*x));
			}
		}
	} else {
		if(y > z) {
			if(1.0 + y > 1.0) {
				z = z / y;
				x = x / y;
				denom = 1.0 / (y * sqrt(1.0 + z*z + x*x));
			}
		} else { /* x < y < z */
			if(1.0 + z > 1.0) {
				y = y / z;
				x = x / z;
				denom = 1.0 / (z * sqrt(1.0 + y*y + x*x));
			}
		}
	}

	if(1.0 + x + y + z > 1.0) {
		m_data[0] *= denom;
		m_data[1] *= denom;
		m_data[2] *= denom;
		return denom;
	}

	return HUGE_VAL;
}

double Vector3D::dot(const Vector3D& other) const
{
	return m_data[0]*other.m_data[0] + 
		m_data[1]*other.m_data[1] + 
		m_data[2]*other.m_data[2];
}

Vector3D Vector3D::cross(const Vector3D& other) const
{
	return the_Vector3D(
			m_data[1]*other[2] - m_data[2]*other[1],
			m_data[2]*other[0] - m_data[0]*other[2],
			m_data[0]*other[1] - m_data[1]*other[0]);
}

Vector3D operator *(double s, const Vector3D& v)
{
  return the_Vector3D(s*v[0], s*v[1], s*v[2]);
}

Vector3D operator +(const Vector3D& u, const Vector3D& v)
{
  return the_Vector3D(u[0]+v[0], u[1]+v[1], u[2]+v[2]);
}

Point3D operator +(const Point3D& u, const Vector3D& v)
{
  return the_Point3D(u[0]+v[0], u[1]+v[1], u[2]+v[2]);
}

Vector3D operator -(const Point3D& u, const Point3D& v)
{
  return the_Vector3D(u[0]-v[0], u[1]-v[1], u[2]-v[2]);
}

Vector3D operator -(const Vector3D& u, const Vector3D& v)
{
  return the_Vector3D(u[0]-v[0], u[1]-v[1], u[2]-v[2]);
}

Vector3D operator -(const Vector3D& u)
{
  return the_Vector3D(-u[0], -u[1], -u[2]);
}

Point3D operator -(const Point3D& u, const Vector3D& v)
{
  return the_Point3D(u[0]-v[0], u[1]-v[1], u[2]-v[2]);
}

Vector3D cross(const Vector3D& u, const Vector3D& v) 
{
  return u.cross(v);
}

Vector3D operator *=(Vector3D& u, double s) {
	u.m_data[0] *= s;
	u.m_data[1] *= s;
	u.m_data[2] *= s;
	return u;
}

std::ostream& operator <<(std::ostream& s, const Point3D& p)
{
  return s << "p(" << p[0] << "," << p[1] << "," << p[2] << ")";
}

std::ostream& operator <<(std::ostream& s, const Vector3D& v)
{
  return s << "v(" << v[0] << "," << v[1] << "," << v[2] << ")";
}

Colour& Colour::operator +=(const Colour& other) {
	m_data[0] = m_data[0] + other.m_data[0];
	m_data[1] = m_data[1] + other.m_data[1];
	m_data[2] = m_data[2] + other.m_data[2];
	return *this;
}

Colour& Colour::operator -=(const Colour& other) {
	m_data[0] = m_data[0] - other.m_data[0];
	m_data[1] = m_data[1] - other.m_data[1];
	m_data[2] = m_data[2] - other.m_data[2];
	return *this;
}

Colour& Colour::operator *=(const Colour& other) {
	m_data[0] = m_data[0] * other.m_data[0];
	m_data[1] = m_data[1] * other.m_data[1];
	m_data[2] = m_data[2] * other.m_data[2];
	return *this;
}

Colour& Colour::operator *=(double s) {
	m_data[0] = m_data[0] * s;
	m_data[1] = m_data[1] * s;
	m_data[2] = m_data[2] * s;
	return *this;
}

Colour& Colour::operator /=(double s) {
	m_data[0] = m_data[0] / s;
	m_data[1] = m_data[1] / s;
	m_data[2] = m_data[2] / s;
	return *this;
}

Colour Colour::operator *(const Colour& other) {
	return the_Colour(m_data[0]*other.m_data[0], 
		m_data[1]*other.m_data[1], 
		m_data[2]*other.m_data[2]);
}

bool operator==(const Colour& u, const Colour& v) {
	return u.m_data[0] == v.m_data[1]
		&& u.m_data[1] == v.m_data[1]
		&& u.m_data[2] == v.m_data[2];
}

double& Colour::operator[](int i) {
	return m_data[i];
}
double Colour::operator[](int i) const {
	return m_data[i];
}

double Colour::lum() const {
	return m_data[0] + m_data[1] + m_data[2];
}

void Colour::clamp() {
	for (int i = 0; i < 3; i++) {
		if (m_data[i] > 1.0) m_data[i] = 1.0; 
		if (m_data[i] < 0.0) m_data[i] = 0.0; 
	}
}

Colour operator *(double s, const Colour& c)
{
  return the_Colour(s*c[0], s*c[1], s*c[2]);
}

Colour operator *(const Colour& c, double s)
{
  return the_Colour(s*c[0], s*c[1], s*c[2]);
}

Colour operator /(const Colour& c, double s)
{
	return the_Colour(c[0] / s, c[1] / s, c[2] / s);
}

Colour operator /(const Colour& c, int s)
{
	return the_Colour(c[0] / (double)s, c[1] / (double)s, c[2] / (double)s);
}

Colour operator +(const Colour& u, const Colour& v)
{
  return the_Colour(u[0]+v[0], u[1]+v[1], u[2]+v[2]);
}

Colour operator -(const Colour& u, const Colour& v)
{
  return the_Colour(u[0]-v[0], u[1]-v[1], u[2]-v[2]);
}

Colour max(const Colour& u, const Colour& v) {
	return the_Colour(MAX(u[0],v[0]), MAX(u[1],v[1]), MAX(u[2],v[2]));
}

Colour min(const Colour& u, const Colour& v) {
	return the_Colour(MIN(u[0],v[0]), MIN(u[1],v[1]), MIN(u[2],v[2]));
}

std::ostream& operator <<(std::ostream& s, const Colour& c)
{
  return s << "c(" << c[0] << "," << c[1] << "," << c[2] << ")";
}

Matrix4x4::Matrix4x4() {
	m_data[0] = 1;
	m_data[5] = 1;
	m_data[10] = 1;
	m_data[15] = 1;
}

Matrix4x4 Matrix4x4::transpose() const {
		/*{ {
			m_data[0], m_data[4], m_data[8], m_data[12],
			m_data[1], m_data[5], m_data[9], m_data[13],
			m_data[2], m_data[6], m_data[10], m_data[14],
			m_data[3], m_data[7], m_data[11], m_data[12]
		} };*/
	Matrix4x4 M;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			M.m_data[i*4+j] = m_data[j*4+i];
		}
	}
	return M;
}

double Matrix4x4::operator()(int i, int j) const{
	return m_data[4 * i + j];
}

double& Matrix4x4::operator()(int i, int j) {
	return m_data[4 * i + j];
}
 
Matrix4x4 operator *(const Matrix4x4& a, const Matrix4x4& b) {
		/*{ {
			a.m_data[0] * b.m_data[0] + a.m_data[1] * b.m_data[4] + a.m_data[2] * b.m_data[8] + a.m_data[3] * b.m_data[12],
			a.m_data[0] * b.m_data[1] + a.m_data[1] * b.m_data[5] + a.m_data[2] * b.m_data[9] + a.m_data[3] * b.m_data[13],
			a.m_data[0] * b.m_data[2] + a.m_data[1] * b.m_data[6] + a.m_data[2] * b.m_data[10] + a.m_data[3] * b.m_data[14],
			a.m_data[0] * b.m_data[3] + a.m_data[1] * b.m_data[7] + a.m_data[2] * b.m_data[11] + a.m_data[3] * b.m_data[15],

			a.m_data[4] * b.m_data[0] + a.m_data[5] * b.m_data[4] + a.m_data[6] * b.m_data[8] + a.m_data[7] * b.m_data[12],
			a.m_data[4] * b.m_data[1] + a.m_data[5] * b.m_data[5] + a.m_data[6] * b.m_data[9] + a.m_data[7] * b.m_data[13],
			a.m_data[4] * b.m_data[2] + a.m_data[5] * b.m_data[6] + a.m_data[6] * b.m_data[10] + a.m_data[7] * b.m_data[14],
			a.m_data[4] * b.m_data[3] + a.m_data[5] * b.m_data[7] + a.m_data[6] * b.m_data[11] + a.m_data[7] * b.m_data[15],

			a.m_data[8] * b.m_data[0] + a.m_data[9] * b.m_data[4] + a.m_data[10] * b.m_data[8] + a.m_data[11] * b.m_data[12],
			a.m_data[8] * b.m_data[1] + a.m_data[9] * b.m_data[5] + a.m_data[10] * b.m_data[9] + a.m_data[11] * b.m_data[13],
			a.m_data[8] * b.m_data[2] + a.m_data[9] * b.m_data[6] + a.m_data[10] * b.m_data[10] + a.m_data[11] * b.m_data[14],
			a.m_data[8] * b.m_data[3] + a.m_data[9] * b.m_data[7] + a.m_data[10] * b.m_data[11] + a.m_data[11] * b.m_data[15],

			a.m_data[12] * b.m_data[0] + a.m_data[13] * b.m_data[4] + a.m_data[14] * b.m_data[8] + a.m_data[15] * b.m_data[12],
			a.m_data[12] * b.m_data[1] + a.m_data[13] * b.m_data[5] + a.m_data[14] * b.m_data[9] + a.m_data[15] * b.m_data[13],
			a.m_data[12] * b.m_data[2] + a.m_data[13] * b.m_data[6] + a.m_data[14] * b.m_data[10] + a.m_data[15] * b.m_data[14],
			a.m_data[12] * b.m_data[3] + a.m_data[13] * b.m_data[7] + a.m_data[14] * b.m_data[11] + a.m_data[15] * b.m_data[15]
		} };*/
	// the written out way is not more efficient
	Matrix4x4 ret;

	for (size_t i = 0; i < 4; ++i) {
		const int r = i * 4;
		for (size_t j = 0; j < 4; ++j) {
			ret.m_data[r+j] = a.m_data[r+0] * b.m_data[0*4+j] + a.m_data[r+1] * b.m_data[1*4+j] +
				a.m_data[r+2] * b.m_data[2*4+j] + a.m_data[r+3] * b.m_data[3*4+j];
		}
	}

	return ret;
}

Point3D operator *(const Matrix4x4& M, const Point3D& p) {
	return{ {
			p[0] * M.m_data[0] + p[1] * M.m_data[1] + p[2] * M.m_data[2] + M.m_data[3],
			p[0] * M.m_data[4] + p[1] * M.m_data[5] + p[2] * M.m_data[6] + M.m_data[7],
			p[0] * M.m_data[8] + p[1] * M.m_data[9] + p[2] * M.m_data[10] + M.m_data[11]
		} };
}

std::ostream& operator <<(std::ostream& os, const Matrix4x4& M) {
	return os << "[" << M.m_data[0] << " " << M.m_data[1] << " " 
		<< M.m_data[2] << " " << M.m_data[3] << "]" << std::endl
		<< "[" << M.m_data[4] << " " << M.m_data[5] << " " 
		<< M.m_data[6] << " " << M.m_data[7] << "]" << std::endl
		<< "[" << M.m_data[8] << " " << M.m_data[9] << " " 
		<< M.m_data[10] << " " << M.m_data[11] << "]" << std::endl
		<< "[" << M.m_data[12] << " " << M.m_data[13] << " " 
		<< M.m_data[14] << " " << M.m_data[15] << "]";
}

Similarity4x4::Similarity4x4(): Matrix4x4() {
	scale1 = 1;
}

Similarity4x4::Similarity4x4(Matrix4x4 init, double scale1) : Matrix4x4(init), scale1(scale1) {
	for (int i = 0; i < 16; ++i) {
		m_data[i] = init.m_data[i];
	}
}

Similarity4x4 operator *(const Similarity4x4& a, const Similarity4x4& b) {
	Matrix4x4 ret;

	for (size_t i = 0; i < 4; ++i) {
		const int r = i * 4;
		for (size_t j = 0; j < 4; ++j) {
			ret.m_data[r + j] = a.m_data[r + 0] * b.m_data[0 * 4 + j] + a.m_data[r + 1] * b.m_data[1 * 4 + j] +
				a.m_data[r + 2] * b.m_data[2 * 4 + j] + a.m_data[r + 3] * b.m_data[3 * 4 + j];
		}
	}

	return Similarity4x4(ret, a.scale1 * b.scale1);
}

Vector3D Matrix4x4::operator*(const Vector3D& v) const {
	return { {
			v[0] * m_data[0] + v[1] * m_data[1] + v[2] * m_data[2],
			v[0] * m_data[4] + v[1] * m_data[5] + v[2] * m_data[6],
			v[0] * m_data[8] + v[1] * m_data[9] + v[2] * m_data[10]
		} };
}

Vector3D Matrix4x4::transNorm(const Vector3D& n) const{
	Vector3D norm = { {
			n[0] * m_data[0] + n[1] * m_data[4] + n[2] * m_data[8],
			n[0] * m_data[1] + n[1] * m_data[5] + n[2] * m_data[9],
			n[0] * m_data[2] + n[1] * m_data[6] + n[2] * m_data[10]
		} };
	norm.normalize();
	return norm;
}

Vector3D Similarity4x4::transNorm(const Vector3D& n) const {
	return { {
			scale1 * (n[0] * m_data[0] + n[1] * m_data[4] + n[2] * m_data[8]),
			scale1 * (n[0] * m_data[1] + n[1] * m_data[5] + n[2] * m_data[9]),
			scale1 * (n[0] * m_data[2] + n[1] * m_data[6] + n[2] * m_data[10])
		} };
}


double Matrix4x4::applyNorm(Vector3D& v) const {
	double m[3] = {
		v[0] * m_data[0] + v[1] * m_data[1] + v[2] * m_data[2],
		v[0] * m_data[4] + v[1] * m_data[5] + v[2] * m_data[6],
		v[0] * m_data[8] + v[1] * m_data[9] + v[2] * m_data[10]
	};
	v.m_data[0] = m[0];
	v.m_data[1] = m[1];
	v.m_data[2] = m[2];
	return v.normalize();
}

double Matrix4x4::applyNorm1(Vector3D& v) const {
	double m[3] = {
		v[0] * m_data[0] + v[1] * m_data[1] + v[2] * m_data[2],
		v[0] * m_data[4] + v[1] * m_data[5] + v[2] * m_data[6],
		v[0] * m_data[8] + v[1] * m_data[9] + v[2] * m_data[10]
	};
	v.m_data[0] = m[0];
	v.m_data[1] = m[1];
	v.m_data[2] = m[2];
	return v.normalize1();
}

double Similarity4x4::applyNorm(Vector3D& v) const {
	double m[3] = {
		scale1 * (v[0] * m_data[0] + v[1] * m_data[1] + v[2] * m_data[2]),
		scale1 * (v[0] * m_data[4] + v[1] * m_data[5] + v[2] * m_data[6]),
		scale1 * (v[0] * m_data[8] + v[1] * m_data[9] + v[2] * m_data[10])
	};
	v.m_data[0] = m[0];
	v.m_data[1] = m[1];
	v.m_data[2] = m[2];
	return 1.0/scale1;
}

double Similarity4x4::applyNorm1(Vector3D& v) const {
	double m[3] = {
		scale1 * (v[0] * m_data[0] + v[1] * m_data[1] + v[2] * m_data[2]),
		scale1 * (v[0] * m_data[4] + v[1] * m_data[5] + v[2] * m_data[6]),
		scale1 * (v[0] * m_data[8] + v[1] * m_data[9] + v[2] * m_data[10])
	};
	std::cout << "cunt";
	v.m_data[0] = m[0];
	v.m_data[1] = m[1];
	v.m_data[2] = m[2];
	return scale1;
}