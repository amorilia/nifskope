/***** BEGIN LICENSE BLOCK *****

BSD License

Copyright (c) 2005-2012, NIF File Format Library and Tools
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the NIF File Format Library and Tools project may not be
   used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

***** END LICENCE BLOCK *****/

#ifndef __NS_MATH_H__
#define __NS_MATH_H__

#include "ns_types.h"
#define NS_PI 3.1416

//! A vector of 2 floats
class Vector2
{
public:
	Vector2();
	Vector2(NSfloat x, NSfloat y);
	Vector2 &operator+=(const Vector2 &);
	Vector2	operator+(const Vector2 &) const;
	Vector2 &operator-=(const Vector2 &);
	Vector2	operator-(const Vector2 &) const;
	Vector2	operator-() const;
	Vector2 &operator*=(NSfloat);
	Vector2	operator*(NSfloat) const;
	Vector2 &operator/=(NSfloat);
	Vector2	operator/(NSfloat) const;
	bool operator==(const Vector2 &) const;
	//! Comparison function for lexicographic sorting
	static bool lexLessThan(const Vector2 &v1, const Vector2 &v2);
protected:
	NSfloat x, y;
};

//! A vector of 3 NSfloats
class Vector3
{
public:
	Vector3();
	Vector3(NSfloat x, NSfloat y, NSfloat z);
	explicit Vector3(const Vector2 &v2, NSfloat z = 0);
	explicit Vector3(const class Vector4 &);
	Vector3 &operator+=(const Vector3 &);
	Vector3 &operator-=(const Vector3 &);
	Vector3 &operator*=(NSfloat);
	Vector3 &operator/=(NSfloat);
	Vector3	operator+(Vector3) const;
	Vector3	operator-(Vector3) const;
	Vector3	operator-() const;
	Vector3	operator*(NSfloat) const;
	Vector3	operator/(NSfloat) const;
	bool operator==(const Vector3 &) const;
	//! Find the length of the vector
	NSfloat length() const;
	//! Find the length of the vector, squared
	NSfloat squaredLength() const;
	//! Normalize the vector
	Vector3 &normalize();
	//! Find the dot product of two vectors
	static NSfloat dotproduct(const Vector3 &, const Vector3 &);
	//! Find the cross product of two vectors
	static Vector3 crossproduct(const Vector3 &, const Vector3 &);
	//! Find the angle between two vectors
	static NSfloat angle(const Vector3 &, const Vector3 &);
	//! Comparison function for lexicographic sorting
	static bool lexLessThan(const Vector3 &, const Vector3 &);
	//! Size a vector to a minimum bound
	void boundMin(const Vector3 &);
	//! Size a vector to a maximum bound
	void boundMax(const Vector3 &);
	//! Return vector first component
protected:
	NSfloat x, y, z;
	friend class Quad;
};

//! A vector of 4 floats
class Vector4
{
public:
	Vector4();
	Vector4(NSfloat x, NSfloat y, NSfloat z, NSfloat w);
	explicit Vector4(const Vector3 &v3, NSfloat w = 0.0);
	Vector4 &operator+=(const Vector4 &);
	Vector4 &operator-=(const Vector4 &);
	Vector4 &operator*=(NSfloat);
	Vector4 &operator/=(NSfloat);
	Vector4 operator+(Vector4) const;
	Vector4 operator-(Vector4) const;
	Vector4 operator-() const;
	Vector4 operator*(NSfloat) const;
	Vector4 operator/(NSfloat) const;
	bool operator==(const Vector4 &) const;
	//! Find the length of the vector
	NSfloat length() const;
	//! Find the length of the vector, squared
	NSfloat squaredLength() const;
	//! Normalize the vector
	void normalize();
	//! Find the dot product of two vectors
	static NSfloat dotproduct(const Vector4 &, const Vector4 &);
	//! Find the angle between two vectors
	static NSfloat angle(const Vector4 &, const Vector4 &);
	//! Comparison function for lexicographic sorting
	static bool lexLessThan(const Vector4 &, const Vector4 &);
protected:
	NSfloat x, y, z, w;
};

inline Vector3::Vector3(const Vector4 &v4)
{
	x = v4.X ();
	y = v4.Y ();
	z = v4.Z ();
}

//! A quaternion
class Quat
{
public:
	Quat();
	Quat(NSfloat w, NSfloat x, NSfloat y, NSfloat z);
	void normalize();
	void negate();
	Quat &operator*=(NSfloat);
	Quat operator*(NSfloat) const;
	Quat &operator+=(const Quat &);
	Quat operator+(const Quat &) const;
	static NSfloat dotproduct(const Quat &, const Quat &)
	//! Set from vector and angle
	void fromAxisAngle(Vector3 axis, NSfloat angle);
	//! Find vector and angle
	void toAxisAngle(Vector3 &axis, NSfloat &angle) const;
	//! Spherical linear interpolatation between two quaternions
	static Quat slerp(NSfloat t, const Quat &p, const Quat &q);
	inline NSfloat sqrSum()
	{
		return (w * w) + (x * x) + (y * y) + (z * z);
	}
protected:
	NSfloat w, x, y, z;
	static const NSfloat iw, ix, iy, iz;
};

//! A 3 by 3 matrix
class Matrix3
{
public:
	Matrix3();
	Matrix3 operator*(const Matrix &) const;
	Vector3 operator*(const Vector3 &) const;
	//float &operator()(int c, int d);
	//float operator()(int c, int d) const;
	Matrix3 inverted() const;
	void fromQuat(const Quat &);
	Quat toQuat() const;
	//! Convert from Euler angles in a (Z,Y,X) manner.
	void fromEuler(NSfloat x, NSfloat y, NSfloat z);
	//! Convert to Euler angles
	bool toEuler(NSfloat &x, NSfloat &y, NSfloat &z) const;
	//! Find a matrix from Euler angles
	static Matrix3 euler(NSfloat x, NSfloat y, NSfloat z);
protected:
	NSfloat m[3][3];
	static const NSfloat identity[9];
};

class Matrix: public Matrix3
{
};

//! A 4 by 4 matrix
class Matrix4
{
public:
	Matrix4();
	Matrix4 operator*(const Matrix4 &) const;
	Vector3 operator*(const Vector3 &) const;
	//float & operator()( unsigned int c, unsigned int d )
	//float operator()( unsigned int c, unsigned int d ) const
	// Not implemented; use decompose() instead
	/*
	Matrix rotation() const;
	Vector3 translation() const;
	Vector3 scale() const;
	*/
	//! Decompose into translation, rotation and scale
	void decompose(Vector3 &trans, Matrix &rot, Vector3 &scale) const;
	//! Compose from translation, rotation and scale
	void compose(const Vector3 &trans, const Matrix &rot, const Vector3 &scale);
protected:
	float m[4][4];
	static const float identity[16];
};

#endif /* __NS_MATH_H__ */
