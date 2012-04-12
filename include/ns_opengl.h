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

#ifndef __NS_OPENGL_H__
#define __NS_OPENGL_H__

#include "GL/gl.h"

//! A transformation consisting of a translation, rotation and scale
class Transform
{
public:
	Transform();
	//! Times operator
	friend Transform operator*(const Transform &t1, const Transform &t2);
	//! Times operator
	Vector3 operator*(const class Vector3 &v) const;
	//! Returns a matrix holding the transform
	class Matrix4 toMatrix4() const;
	// Format of rotation matrix? See http://en.wikipedia.org/wiki/Euler_angles
	// fromEuler indicates that it might be "zyx" form
	// Yaw, Pitch, Roll correspond to rotations in X, Y, Z axes respectively
	// Note that by convention, all angles in mathematics are anticlockwise,
	// looking top-down at the plane ie. rotation on X axis means "in the YZ
	// plane where a positive X vector is up"
	// From entim: "The rotations are applied after each other: first yaw,
	// then pitch, then roll"
	// For A,X,Y,Z representation, A is amplitude, each axis is a percentage
	class Matrix rotation;
	class Vector3 translation;
	float scale;
};

//! A 3 value color (RGB)
class Color3
{
public:
	Color3();
	Color3(GLfloat r, GLfloat g, GLfloat b);
	explicit Color3(const class Color4 &);
	Color3 operator*(GLfloat x) const;
	Color3 &operator+=(const Color3 &);
	Color3 &operator-=(const Color3 &);
	Color3 operator+(const Color3 &) const;
	Color3 operator-(const Color3 &) const;
	//! Get the red component
	GLfloat red() const;
	//! Get the green component
	GLfloat green() const;
	//! Get the blue component
	GLfloat blue() const;
	//! Set the red component
	void setRed(GLfloat);
	//! Set the green component
	void setGreen(GLfloat);
	//! Set the blue component
	void setBlue(GLfloat);
	//! Set the color components
	void setRGB(GLfloat, GLfloat, GLfloat);
	//! %Data accessor
	const GLfloat *data() const;
protected:
	union {
		GLfloat rgb[3];
		struct {
			GLfloat r, g, b;
		};
	};
};

class Color4
{
public:
	Color4();
	explicit Color4(const Color3 &c, GLfloat alpha = 1.0);
	Color4(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
	Color4 operator*(GLfloat) const;
	Color4 &operator+=(const Color4 &);
	Color4 &operator-=(const Color4 &);
	Color4 operator+(const Color4 &) const;
	Color4 operator-(const Color4 &) const;
	bool operator==(const Color4 &) const;
	//! Get the red component
	GLfloat red() const;
	//! Get the green component
	GLfloat green() const;
	//! Get the blue component
	GLfloat blue() const;
	//! Get the alpha component
	GLfloat alpha() const;
	//! Set the red component
	void setRed(GLfloat);
	//! Set the green component
	void setGreen(GLfloat);
	//! Set the blue component
	void setBlue(GLfloat);
	//! Set the alpha component
	void setAlpha(GLfloat);
	//! Set the color components
	void setRGBA(GLfloat, GLfloat, GLfloat, GLfloat);
	//! %Data accessor
	const GLfloat *data() const;
	//! Alpha blend
	Color4 blend(GLfloat alpha) const;
protected:
	union {
		GLfloat rgba[4];
		struct {
			GLfloat r, g, b, a;
		};
	};
};

inline Color3::Color3(const Color4 &c4)
{
	r = c4.r;
	g = c4.g;
	b = c4.b;
}

#endif /* __NS_OPENGL_H__ */
