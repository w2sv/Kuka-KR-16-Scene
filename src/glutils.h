#pragma once

#include <functional>

#include "../dependencies/freeglut.h"

#include "utils.h"


////////////////////////////////////////////////////////////
/// Color
////////////////////////////////////////////////////////////

struct Color {
	GLfloat r, g, b;
	Color(GLfloat r, GLfloat g, GLfloat b);
	Color(GLfloat rgb);
	static Color fromUnnormalizedValues(unsigned int r, unsigned int g, unsigned int b);

	void render(bool materialized = true) const;
};

namespace COLORS {
	extern Color const BLACK, WHITE, GREY;
}

////////////////////////////////////////////////////////////
/// Transformation
////////////////////////////////////////////////////////////

typedef std::function<void(float)> glRotationFunction;

namespace glTransformationAxes {
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

void glScaleUniformly(float value);

void glTranslatev(const Vector3& vector);

////////////////////////////////////////////////////////////
/// Miscellanous
////////////////////////////////////////////////////////////

void drawCoordSystem(const Extrema& x, const Extrema& y, const Extrema& z, float coneScale = 1);