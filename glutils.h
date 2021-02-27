#pragma once

#include <iostream>
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
	GLfloat r, g, b;

	Color(double r, double g, double b);

	void render() const;
};


namespace Colors {
	extern Color const BLACK, WHITE, GREY;
}
#pragma endregion


void glRotatep(float angle, const GLfloat* axis);


void glScaleUniformly(float value);


namespace Axes {
	extern GLfloat const X[3], Y[3], Z[3];
}





void glTranslateZ(GLfloat value);
