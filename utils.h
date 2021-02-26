#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "Include/freeglut.h"


struct Vector3 {
public:
	GLfloat x, y, z;

	Vector3(GLfloat x, GLfloat y, GLfloat z);
	friend std::ostream& operator<<(std::ostream& os, const Vector3& vector);
};


struct Vector2 {
public:
	GLdouble x, y;

	Vector2(GLdouble x, GLdouble y);
};


#pragma region Color
struct Color {
public:
	GLfloat r, g, b, a;

	Color(double r, double g, double b, double a = 1.0);

	// Constructor for initialization by means of int color values ranging from 0 to 255
	explicit Color(int r, int g, int b, int a = 255);

	void render() const;
};


namespace Colors {
	extern Color const BLACK, WHITE, GREY;
}
#pragma endregion


struct CoordinateExtrema {
public:
	float min, max;

	CoordinateExtrema(float min, float max);
	CoordinateExtrema(float absValue);

	float spread() const;
};


void glRotatep(float angle, const GLfloat* axis);


void glScaleUniformly(float value);


namespace Axes {
	extern GLfloat const X[3], Y[3], Z[3];
}


struct Measurements {
	float height, width, depth;

	Measurements(float height, float width, float depth);
};


void glTranslateZ(GLfloat value);


char* concatenatedCharPtr(const char* a, const char* b);


float toRadian(float degrees);


std::vector<Vector2> discrete2DCircleRadiusPoints(float radius, int nPoints);