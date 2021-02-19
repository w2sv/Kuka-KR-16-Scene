#pragma once

#include <cmath>
#include <cassert>
#include <iostream>

#include "Include/freeglut.h"

struct Vector3 {
public:
	GLfloat x, y, z;

	Vector3(GLfloat x, GLfloat y, GLfloat z);
	friend std::ostream& operator<<(std::ostream& os, const Vector3& vector);
};

struct Color {
public:
	GLfloat r, g, b, o;

	Color(double r, double g, double b, double o = 1.0);
	explicit Color(int r, int g, int b, double o = 1.0);

	const void render();
};

struct CoordinateExtrema {
public:
	float min, max;

	CoordinateExtrema(float min, float max);
	CoordinateExtrema(float absValue);

	const float spread();
};

void glRotatep(float angle, const GLfloat* axis);

namespace Axes {
	extern GLfloat const X[3];
	extern GLfloat const Y[3];
	extern GLfloat const Z[3];
}

struct Measurements {
	float height, width, depth;

	Measurements(float height, float width, float depth);
};

void glTranslateZ(GLfloat value);

GLdouble* getModelMatrix();
GLdouble* getModelViewMatrix();
Vector3 parseTranslationVector(GLdouble* modelMatrix);