#pragma once

#include <iostream>
#include <limits>

#include "Include/freeglut.h"

#include "utils.h"
#include "geometry.h"
#include "input.h"


class Robot
{
public:
	Robot();

	void moveArm(Vector3& position);
	const void draw();

private:
	struct Axis {
		private:
			static cg_key* key;

			int tiltAngleLimit;
			
			char rotationDecrementalKey;
			char rotationIncrementalKey;

			char tiltDecrementalKey;
			char tiltIncrementalKey;

			void clipTiltAngle();

		public:
			float tiltAngle;
			float rotationAngle;

			Axis(char rotationDecrementalKey, char rotationIncrementalKey, char tiltDecrementalKey, char tiltIncrementalKey, int tiltAngleLimit);
			void updateOrientation();
	};

	Axis lowerAxis = Axis('a', 'd', 's', 'w', 45);

	const float PEDASTEL_CEILING_Z_COORDINATE = 2.;
	const float LOWER_STEEL_CYLINDER_HEIGHT = 1.2;

	const void drawPedestal();

	const void drawArm();
	const void drawLowerSteelCylinder();
	const void drawLowerAxis();
};

