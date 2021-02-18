#pragma once

#include <iostream>

#include "Include/freeglut.h"

#include "utils.h"
#include "geometry.h"
#include "input.h"


struct Axis {
public:
	float tiltAngle;
	float rotationAngle;

	Axis() {
		this->rotationAngle = 0;
		this->tiltAngle = 0;

		std::cout << "instantiated axis" << std::endl;
	}

	void update() {
		if (this->key->keyState('d') == 1)
			this->rotationAngle += 0.5;
		else if (this->key->keyState('d') == 2)
			this->rotationAngle += 1;

		if (this->key->keyState('a') == 1)
			this->rotationAngle -= 0.5;
		else if (this->key->keyState('a') == 2)
			this->rotationAngle -= 1;

		std::cout << this->rotationAngle << std::endl;
	}
private:
	static cg_key* key;
};

class Robot
{
public:
	Robot();

	void moveArm(Vector3& position);
	const void draw();

private:
	Axis lowerAxis = Axis();

	const float PEDASTEL_CEILING_Z_COORDINATE = 2.;
	const float LOWER_STEEL_CYLINDER_HEIGHT = 1.2;

	const void drawPedestal();

	const void drawArm();
	const void drawLowerSteelCylinder();
	const void drawLowerAxis();
};

