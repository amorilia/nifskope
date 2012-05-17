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

#include "ns_opengl.h"

/* Transform */

Transform::Transform()
{
	scale = 1.0;
}
	
Transform
Transform::operator*(const Transform &t1, const Transform &t2)
{
	Transform t;
	t.rotation = t1.rotation * t2.rotation;
	t.translation = t1.translation + (t1.rotation * t2.translation * t1.scale);
	t.scale = t1.scale * t2.scale;
	return t;
}
	
Vector3
Transform::operator*(const Vector3 &v) const
{
	return (rotation * v * scale) + translation;
}
	
Matrix4
Transform::toMatrix4() const
{
	Matrix4 m;
	for (int c = 0; c < 3; c++) {
		for (int d = 0; d < 3; d++)
			m(c, d) = rotation(d, c) * scale;
		m(3, c) = translation[c];
	}
	m(0, 3) = 0.0;
	m(1, 3) = 0.0;
	m(2, 3) = 0.0;
	m(3, 3) = 1.0;
	return m;
}

/* Color3 */

Color3::Color3()
{
	r = g = b = 1.0;
}

Color3::Color3(GLfloat r, GLfloat g, GLfloat b)
{
	setRGB (r, g, b);
}
	
Color3
Color3::operator*(GLfloat x) const
{
	Color3 c (*this);
	c.r *= x;
	c.g *= x;
	c.b *= x;
	return c;
}
	
Color3 &
Color3::operator+=(const Color3 &c)
{
	r += c.r;
	g += c.g;
	b += c.b;
	return *this;
}
	
Color3 &
Color3::operator-=(const Color3 &c)
{
	r -= c.r;
	g -= c.g;
	b -= c.b;
	return *this;
}
	
Color3
Color3::operator+(const Color3 &c) const
{
	Color3 tmp (*this);
	return (tmp += c);
}
	
Color3
Color3::operator-(const Color3 &c) const
{
	Color3 tmp (*this);
	return (tmp -= c);
}
	
GLfloat
Color3::red() const
{
	return r;
}

GLfloat
Color3::green() const
{
	return g;
}

GLfloat
Color3::blue() const
{
	return b;
}
	
void
Color3::setRed(GLfloat value)
{
	r = value;
}

void
Color3::setGreen(GLfloat value)
{
	g = value;
}

void
Color3::setBlue(GLfloat value)
{
	b = value;
}
	
void
Color3::setRGB(GLfloat r, GLfloat g, GLfloat b)
{
	this->r = r;
	this->g = g;
	this->b = b;
}

/*const GLfloat *
Color3::data() const
{
	return &(rgb[0]);
}*/

/* Color4 */

Color4::Color4()
{
	r = g = b = a = 1.0;
}

explicit Color4::Color4 const Color3 &c, GLfloat alpha)
{
	r = c.r;
	g = c.g;
	b = c.b;
	a = alpha;
}

Color4::Color4(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	setRGBA (r, g, b, a);
}
	
Color4
Color4::operator*(GLfloat x) const
{
	Color4 c (*this);
	c.r *= x;
	c.g *= x;
	c.b *= x;
	c.a *= x;
	return c;
}

Color4 &
Color4::operator+=(const Color4 &c)
{
	r += c.r;
	g += c.g;
	b += c.b;
	a += c.a;
	return *this;
}
	
Color4 &
Color4::operator-=(const Color4 &c)
{
	r -= c.r;
	g -= c.g;
	b -= c.b;
	a -= c.a;
	return *this;
}
	
Color4
Color4::operator+(const Color4 &c) const
{
	Color4 tmp (*this);
	return (tmp += c);
}
	
Color4
Color4::operator-(const Color4 &c) const
{
	Color4 tmp (*this);
	return (tmp -= c);
}
	
bool
Color4::operator==(const Color4 &c) const
{
	return (r == c.r) && (g == c.g) && (b == c.b) && (a == c.a);
}
	
GLfloat
Color4::red() const
{
	return r;
}

GLfloat
Color4::green() const
{
	return g;
}

GLfloat
Color4::blue() const
{
	return b;
}

GLfloat
Color4::alpha() const
{
	return a;
}
	
void
Color4::setRed(GLfloat value)
{
	r = value;
}

void
Color4::setGreen(GLfloat value)
{
	g = value;
}

void
Color4::setBlue(GLfloat value)
{
	b = value;
}

void
Color4::setAlpha(GLfloat value)
{
	a = value;
}
	
void
Color4::setRGBA(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}

/*const GLfloat *
Color4::data() const
{
	return &(rgba[0]);
}*/
	
Color4
Color4::blend(GLfloat alpha) const
{
	Color4 tmp (*this);
	tmp.setAlpha (tmp.alpha () * alpha);
	return tmp;
}
