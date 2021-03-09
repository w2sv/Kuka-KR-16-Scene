#pragma once

#include <iostream>
#include <vector>
#include <functional>

#include "../dependencies/freeglut.h"

#include "utils.h"


struct Vector3 {
public:
	Vector3(GLfloat x, GLfloat y, GLfloat z);

	GLfloat x, y, z;

	Vector3 inverted() const;
	void glTranslate() const;

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
	
	void render(bool materialized = true) const;
};


namespace Colors {
	extern Color const BLACK, WHITE, GREY;
}
#pragma endregion



namespace TransformationAxes {
	namespace X {
		void rotate(float angle);
		void translate(float distance);
	}

	namespace Y {
		void rotate(float angle);
		void translate(float distance);
	}

	namespace Z {
		void rotate(float angle);
		void translate(float distance);
	}
}



typedef std::function<void(float)> glRotationFunction;



void glScaleUniformly(float value);



void drawCoordSystem(Extrema& x, Extrema& y, Extrema& z, float coneScale = 1);