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

#include "ns_math.h"
#include <math.h>

/* Vector2 */

Vector2::Vector2()
{
	x = y = 0.0;
}

Vector2::Vector2(NSfloat x, NSfloat y)
{
	this->x = x;
	this->y = y;
}

Vector2 &
Vector2::operator+=(const Vector2 &v)
{
	x += v.x;
	y += v.y;
	return *this;
}

Vector2
Vector2::operator+(const Vector2 &v) const
{
	Vector2 w (*this);
	return (w += v);
}

Vector2 &
Vector2::operator-=(const Vector2 &v)
{
	x -= v.x;
	y -= v.y;
	return *this;
}

Vector2
Vector2::operator-(const Vector2 &v) const
{
	Vector2 w (*this);
	return (w -= v);
}

Vector2
Vector2::operator-() const
{
	return Vector2 () - *this;
}

Vector2 &
Vector2::operator*=(NSfloat s)
{
	x *= s;
	y *= s;
	return *this;
}

Vector2
Vector2::operator*(NSfloat s) const
{
	Vector2 w (*this);
	return (w *= s);
}

Vector2 &
Vector2::operator/=(NSfloat s)
{
	x /= s;
	y /= s;
	return *this;
}

Vector2
Vector2::operator/(NSfloat s) const
{
	Vector2 w (*this);
	return (w /= s);
}

bool
Vector2::operator==(const Vector2 &v) const
{
	return x == v.x && y == v.y;
}

static bool
Vector2::lexLessThan(const Vector2 &v1, const Vector2 &v2)
{
	if (v1.x != v2.x)
		return v1.x < v2.x;
	else
		return v1.y < v2.y;
}

/* Vector3 */

Vector3::Vector3()
{
	x = y = z = 0.0;
}

Vector3::Vector3(NSfloat x, NSfloat y, NSfloat z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

explicit Vector3::Vector3(const Vector2 &v2, NSfloat z)
{
	x = v2.x;
	y = v2.y;
	this->z = z;
}

Vector3 &
Vector3::operator+=(const Vector3 &v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

Vector3 &
Vector3::operator-=(const Vector3 &v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

Vector3 &
Vector3::operator*=(NSfloat s)
{
	x *= s;
	y *= s;
	z *= s;
	return *this;
}

Vector3 &
Vector3::operator/=(NSfloat s)
{
	x /= s;
	y /= s;
	z /= s;
	return *this;
}

Vector3
Vector3::operator+(Vector3 v) const
{
	Vector3 w (*this);
	return w += v;
}

Vector3
Vector3::operator-(Vector3 v) const
{
	Vector3 w (*this);
	return w -= v;
}

Vector3
Vector3::operator-() const
{
	return Vector3 () - *this;
}

Vector3
Vector3::operator*(NSfloat s) const
{
	Vector3 v (*this);
	return v *= s;
}

Vector3
Vector3::operator/(NSfloat s) const
{
	Vector3 v (*this);
	return v /= s;
}

bool
Vector3::operator==(const Vector3 &v) const
{
	return (x == v.x) && (y == v.y) && (z == v.z);
}
	
NSfloat
Vector3::length() const
{
	return sqrt ((x * x) + (y * y) + (z * z));
}

NSfloat
Vector3::squaredLength() const
{
	return (x * x) + (y * y) + (z * z);
}
	
Vector3 &
Vector3::normalize()
{
	NSfloat m = length ();
	if (m > 0.0)
		m = 1.0 / m;
	else
		m = 0.0;
	x *= m;
	y *= m;
	z *= m;
	return *this;
}
	
static NSfloat
Vector3::dotproduct(const Vector3 &v1, const Vector3 &v2)
{
	return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

static Vector3
Vector3::crossproduct(const Vector3 &a, const Vector3 &b)
{
	return Vector3 (
		(a.y * b.z) - (a.z * b.y),
		(a.z * b.x) - (a.x * b.z),
		(a.x * b.y) - (a.y * b.x));
}

static NSfloat
Vector3::angle(const Vector3 &v1, const Vector3 &v2)
{
	NSfloat dot = dotproduct (v1, v2);
	if (dot > 1.0)
		return 0.0;
	else if (dot < - 1.0)
		return (NSfloat)NS_PI;
	else if (dot == 0.0)
		return (NSfloat)(NS_PI/2);
	else
		return acos (dot);
}
	
static bool
Vector3::lexLessThan(const Vector3 &v1, const Vector3 &v2)
{
	if (v1.x != v2.x)
		return v1.x < v2.x;
	else {
		if (v1.y != v2.y)
			return v1.y < v2.y;
		else
			return v1.z < v2.z;
	}
}
	
void
Vector3::boundMin(const Vector3 &v)
{
	if (v.x < x)
		x = v.x;
	if (v.y < y)
		y = v.y;
	if (v.z < z)
		z = v.z;
}

void
Vector3::boundMax(const Vector3 &v)
{
	if (v.x > x)
		x = v.x;
	if (v.y > y)
		y = v.y;
	if (v.z > z)
		z = v.z;
}

/* Vector4 */

Vector4::Vector4()
{
	x = y = z = w = 0.0;
}

Vector4::Vector4(NSfloat x, NSfloat y, NSfloat z, NSfloat w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

explicit Vector4::Vector4(const Vector3 &v3, NSfloat w)
{
	x = v3.x;
	y = v3.y;
	z = v3.z;
	this->w = w;
}
	//! Add-equals operator
Vector4 &
Vector4::operator+=(const Vector4 &v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;
	return *this;
}

Vector4 &
Vector4::operator-=(const Vector4 &v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;
	return *this;
}

Vector4 &
Vector4::operator*=(NSfloat s)
{
	x *= s;
	y *= s;
	z *= s;
	w *= s;
	return *this;
}

Vector4 &
Vector4::operator/=(NSfloat s)
{
	x /= s;
	y /= s;
	z /= s;
	w /= s;
	return *this;
}

Vector4
Vector4::operator+(Vector4 v) const
{
	Vector4 w (*this);
	return w += v;
}

Vector4
Vector4::operator-(Vector4 v) const
{
	Vector4 w (*this);
	return w -= v;
}

Vector4
Vector4::operator-() const
{
	return Vector4 () - *this;
}

Vector4
Vector4::operator*(NSfloat s) const
{
	Vector4 v (*this);
	return v *= s;
}

Vector4
Vector4::operator/(NSfloat s) const
{
	Vector4 v (*this);
	return v /= s;
}

bool
Vector4::operator==(const Vector4 &v) const
{
	return (x == v.x) && (y == v.y) && (z == v.z) && (w == v.w);
}
	
NSfloat
Vector4::length() const
{
	return sqrt ((x * x) + (y * y) + (z * z) + (w * w));
}
	
NSfloat
Vector4::squaredLength() const
{
	return (x * x) + (y * y) + (z * z) + (w * w);
}

void
Vector4::normalize()
{
	NSfloat m = length ();
	if ( m > 0.0 )
		m = 1.0 / m;
	else
		m = 0.0F;
	x *= m;
	y *= m;
	z *= m;
	w *= m;
}

static NSfloat
Vector4::dotproduct(const Vector4 &v1, const Vector4 &v2)
{
	return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z) + (v1.w * v2.w);
}
	
static NSfloat
Vector4::angle(const Vector4 &v1, const Vector4 &v2)
{
	NSfloat dot = dotproduct (v1, v2);
	if (dot > 1.0)
		return 0.0;
	else if (dot < - 1.0)
		return (NSfloat)NS_PI;
	else if (dot == 0.0)
		return (NSfloat)(NS_PI/2);
	else
		return (NSfloat)acos (dot);
}
	
static bool
Vector4::lexLessThan(const Vector4 &v1, const Vector4 &v2)
{
	if (v1.x != v2.x)
		return v1.x < v2.x;
	else {
		if (v1.y != v2.y)
			return v1.y < v2.y;
		else {
			if (v1.z != v2.z)
				return v1.z < v2.z;
			else
				return v1.w < v2.w;
		}
	}
}

/* Quat */
const NSfloat Quat::iw = 1.0;
const NSfloat Quat::ix = 0.0;
const NSfloat Quat::iy = 0.0;
const NSfloat Quat::iz = 0.0;
	
Quat::Quat()
{
	w = iw;
	x = ix;
	y = iy;
	z = iz;
}

Quat::Quat(NSfloat w, Nfloat x, NSfloat y, NSfloat z)
{
	this->w = w;
	this->x = x;
	this->y = y;
	this->z = z;
}

void
Quat::normalize()
{
	NSfloat mag = ((w*w) + (x*x) + (y*y) + (z*z));
	w /= mag;
	x /= mag;
	y /= mag;
	z /= mag;
}

void
Quat::negate()
{
	w = -w;
	x = -x;
	y = -y;
	z = -z;
}
	
Quat &
Quat::operator*=(NSfloat s)
{
	w *= s;
	x *= s;
	y *= s;
	z *= s;
	return *this;
}

Quat
Quat::operator*(NSfloat s) const
{
	Quat q (*this);
	return (q *= s);
}

Quat &
Quat::operator+=(const Quat &q)
{
	w += q.w;
	x += q.x;
	y += q.y;
	z += q.z;
	return *this;
}

Quat
Quat::operator+(const Quat &q) const
{
	Quat r (*this);
	return (r += q);
}

static NSfloat
Quat::dotproduct(const Quat &q1, const Quat &q2)
{
	return (q1.w * q2.w) + (q1.x * q2.x) + (q1.y * q2.y) + (q1.z * q2.z);
}
	
void
Quat::fromAxisAngle(Vector3 axis, NSfloat angle)
{
	axis.normalize ();
    NSfloat s = sin (angle / 2);
    w = cos (angle / 2);
    x = s * axis.x;
    y = s * axis.y;
    z = s * axis.z;
}

void
Quat::toAxisAngle(Vector3 &axis, NSfloat &angle) const
{
    NSfloat squaredLength = (x * x) + (y * y) + (z * z);
    if (squaredLength > 0.0) {
        angle = acos (w) * 2.0;
        axis.x = x;
        axis.y = y;
        axis.z = z;
		axis /= sqrt (squaredLength);
    }
    else {
		axis = Vector3 (1, 0, 0);
		angle = 0;
    }
}

static inline NSfloat
ISqrt_approx_in_neighborhood(NSfloat s)
{
   static const NSfloat NEIGHBORHOOD = 0.959066;
   static const NSfloat SCALE = 1.000311;
   static const NSfloat ADDITIVE_CONSTANT =
		SCALE / (NSfloat)sqrt (NEIGHBORHOOD);
   static const NSfloat FACTOR =
		SCALE * (-0.5 / (NEIGHBORHOOD * (NSfloat)sqrt (NEIGHBORHOOD)));
   return ADDITIVE_CONSTANT + ((s - NEIGHBORHOOD) * FACTOR);
}

static inline void
fast_normalize(Quat &q)
{
	NSfloat s = q.sqrSum ();
	NSfloat k = ISqrt_approx_in_neighborhood (s);
	if (s <= 0.91521198) {
		k *= ISqrt_approx_in_neighborhood (k * k * s);
		if (s <= 0.65211970)
			k *= ISqrt_approx_in_neighborhood (k * k * s);
	}
	q *= k;
}

static inline NSfloat
lerp(NSfloat v0, NSfloat v1, NSfloat perc)
{
   return v0 + (perc * (v1 - v0));
}

static inline NSfloat
correction(NSfloat t, NSfloat fCos) 
{
   const NSfloat s = 0.8228677;
   const NSfloat kc = 0.5855064;
   float factor = 1.0 - (s * fCos);
   float k = kc * factor * factor;
   return t * (k * t * ((2.0 * t) - 3.0) + 1.0 + k);
}

static Quat
Quat::slerp(NSfloat t, const Quat &p, const Quat &q)
{
	// Copyright (c) 2002 Jonathan Blow
	//  "Hacking Quaternions", The Inner Product, March 2002
	//   http://number-none.com/product/Hacking%20Quaternions/index.html
 	NSfloat tprime;
	if (t <= 0.5)
      tprime = correction (t, dotproduct (p, q));
	else
      tprime = 1.0 - correction (1.0 - t, dotproduct(p, q));
	Quat result (
		lerp (p.w, q.w, tprime), lerp (p.x, q.x, tprime),
		lerp (p.y, q.y, tprime), lerp (p.z, q.z, tprime));
   fast_normalize (result);
   return result;
}

/* Matrix3 */
const NSfloat Matrix3::identity[9] =
	{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};

Matrix3::Matrix3()
{
	memcpy (m, identity, sizeof(NSfloat)*9);
}

Matrix3
Matrix3::operator*(const Matrix3 &m2) const
{
	Matrix m3;
	for (int r = 0; r < 3; r++)
		for (int c = 0; c < 3; c++)
			m3.m[r][c] =
				(m[r][0] * m2.m[0][c]) +
				(m[r][1] * m2.m[1][c]) +
				(m[r][2] * m2.m[2][c]);
	return m3;
}

Vector3
Matrix3::operator*(const Vector3 &v) const
{
	return Vector3(
		(m[0][0] * v[0]) + (m[0][1] * v[1]) + (m[0][2] * v[2]),
		(m[1][0] * v[0]) + (m[1][1] * v[1]) + (m[1][2] * v[2]),
		(m[2][0] * v[0]) + (m[2][1] * v[1]) + (m[2][2] * v[2]));
}

NSfloat &
Matrix3::operator()(unsigned int c, unsigned int d)
{
	NS_ASSERT(c < 3 && d < 3)
	return m[c][d];
}

NSfloat
Matrix3::operator()(unsigned int c, unsigned int d) const
{
	NS_ASSERT(c < 3 && d < 3)
	return m[c][d];
}

Matrix3
Matrix3::inverted() const
{
    Matrix3 i;
    i(0,0) = (m[1][1] * m[2][2]) - (m[1][2] * m[2][1]);
    i(0,1) = (m[0][2] * m[2][1]) - (m[0][1] * m[2][2]);
    i(0,2) = (m[0][1] * m[1][2]) - (m[0][2] * m[1][1]);
    i(1,0) = (m[1][2] * m[2][0]) - (m[1][0] * m[2][2]);
    i(1,1) = (m[0][0] * m[2][2]) - (m[0][2] * m[2][0]);
    i(1,2) = (m[0][2] * m[1][0]) - (m[0][0] * m[1][2]);
    i(2,0) = (m[1][0] * m[2][1]) - (m[1][1] * m[2][0]);
    i(2,1) = (m[0][1] * m[2][0]) - (m[0][0] * m[2][1]);
    i(2,2) = (m[0][0] * m[1][1]) - (m[0][1] * m[1][0]);
    NSfloat d = (m[0][0] * i(0,0)) + (m[0][1] * i(1,0)) + (m[0][2] * i(2,0));
    if (fabs (d) <= 0.0)
        return Matrix ();
	for (int x = 0; x < 3; x++)
		for (int y = 0; y < 3; y++)
			i(x,y) /= d;
    return i;
}

void
Matrix3::fromQuat(const Quat &q)
{
	NSfloat fTx  = 2.0 * q.x;
	NSfloat fTy  = 2.0 * q.y;
	NSfloat fTz  = 2.0 * q.z;
	NSfloat fTwx = fTx * q.w;
	NSfloat fTwy = fTy * q.w;
	NSfloat fTwz = fTz * q.w;
	NSfloat fTxx = fTx * q.x;
	NSfloat fTxy = fTy * q.x;
	NSfloat fTxz = fTz * q.x;
	NSfloat fTyy = fTy * q.y;
	NSfloat fTyz = fTz * q.y;
	NSfloat fTzz = fTz * q.z;
	m[0][0] = 1.0 - (fTyy + fTzz);
	m[0][1] = fTxy - fTwz;
	m[0][2] = fTxz + fTwy;
	m[1][0] = fTxy + fTwz;
	m[1][1] = 1.0 - (fTxx + fTzz);
	m[1][2] = fTyz - fTwx;
	m[2][0] = fTxz - fTwy;
	m[2][1] = fTyz + fTwx;
	m[2][2] = 1.0 - (fTxx + fTyy);
}

Quat
Matrix3::toQuat() const
{
	Quat q;
	NSfloat trace = m[0][0] + m[1][1] + m[2][2];
	NSfloat root;
	
	if (trace > 0.0) {
		root = sqrt (trace + 1.0);
		q[0] = root / 2.0;
		root = 0.5 / root;
		q[1] = (m[2][1] - m[1][2]) * root;
		q[2] = (m[0][2] - m[2][0]) * root;
		q[3] = (m[1][0] - m[0][1]) * root;
	}
	else {
		int i = (m[1][1] > m[0][0] ? 1 : 0);
		if (m[2][2] > m[i][i])
			i = 2;
		const int next[3] = {1, 2, 0};
		int j = next[i];
		int k = next[j];
		root = sqrt (m[i][i] - m[j][j] - m[k][k] + 1.0);
		q[i+1] = root / 2;
		root = 0.5 / root;
		q[0] = (m[k][j] - m[j][k]) * root;
		q[j+1] = (m[j][i] + m[i][j]) * root;
		q[k+1] = (m[k][i] + m[i][k]) * root;
	}
	return q;
}

void
Matrix3::fromEuler(NSfloat x, NSfloat y, NSfloat z)
{
	NSfloat sinX = sin (x);
	NSfloat cosX = cos (x);
	NSfloat sinY = sin (y);
	NSfloat cosY = cos (y);
	NSfloat sinZ = sin (z);
	NSfloat cosZ = cos (z);
	m[0][0] = cosY * cosZ;
	m[0][1] = -cosY * sinZ;
	m[0][2] = sinY;
	m[1][0] = (sinX * sinY * cosZ) + (sinZ * cosX);
	m[1][1] = (cosX * cosZ) - (sinX * sinY * sinZ);
	m[1][2] = -sinX * cosY;
	m[2][0] = (sinX * sinZ) - (cosX * sinY * cosZ);
	m[2][1] = (cosX * sinY * sinZ) + (sinX * cosZ);
	m[2][2] = cosX * cosY;
}

bool
Matrix3::toEuler(NSfloat &x, NSfloat &y, NSfloat &z) const
{
	if (m[0][2] < 1.0) {
		if (m[0][2] > - 1.0) {
			x = atan2 (-m[1][2], m[2][2]);
			y = asin (m[0][2]);
			z = atan2 (-m[0][1], m[0][0]);
			return true;
		}
		else {
			x = -atan2 (-m[1][0], m[1][1]);
			y = -PI / 2;
			z = 0.0;
			return false;
		}
	}
	else {
		x = atan2 (m[1][0], m[1][1]);
		y = PI / 2;
		z = 0.0;
		return false;
	}
}

/* Matrix4 */
const NSfloat Matrix4::identity[16] = { 1.0, 0.0, 0.0, 0.0,  0.0, 1.0, 0.0, 0.0,  0.0, 0.0, 1.0, 0.0,  0.0, 0.0, 0.0, 1.0 };

void Matrix4::decompose( Vector3 & trans, Matrix & rot, Vector3 & scale ) const
{
	trans = Vector3( m[ 3 ][ 0 ], m[ 3 ][ 1 ], m[ 3 ][ 2 ] );
	
	Matrix rotT;
	
	for ( int i = 0; i < 3; i++ )
	{
		for ( int j = 0; j < 3; j++ )
		{
			rot( j, i ) = m[ i ][ j ];
			rotT( i, j ) = m[ i ][ j ];
		}
	}
	
	Matrix mtx = rot * rotT;
	
	scale = Vector3( sqrt( mtx( 0, 0 ) ), sqrt( mtx( 1, 1 ) ), sqrt( mtx( 2, 2 ) ) );
	
	for ( int i = 0; i < 3; i++ )
		for ( int j = 0; j < 3; j++ )
			rot( i, j ) /= scale[ i ];
}

void Matrix4::compose( const Vector3 & trans, const Matrix & rot, const Vector3 & scale )
{
	m[0][3] = 0.0;
	m[1][3] = 0.0;
	m[2][3] = 0.0;
	m[3][3] = 1.0;

	m[3][0] = trans[0];
	m[3][1] = trans[1];
	m[3][2] = trans[2];
	
	for ( int i = 0; i < 3; i++ )
		for ( int j = 0; j < 3; j++ )
			m[ i ][ j ] = rot( j, i ) * scale[ j ];
}