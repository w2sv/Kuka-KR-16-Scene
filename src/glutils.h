#pragma once

#include <iostream>
#include <vector>
#include <functional>

#include "../dependencies/freeglut.h"


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
	void render() const;
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



typedef std::function<void(float)> RotationFunction;



struct ModelviewMatrixTransformation {
	ModelviewMatrixTransformation(Vector3&& shiftVector, RotationFunction rotationFunction = nullptr, float rotationAngle = -1);

	Vector3 shiftVector;
	std::function<void(float)> rotationFunction;
	float rotationAngle;

	void effectuate(bool invertedly = false) const;
};



void glScaleUniformly(float value);
