#include "robot.h"

#pragma region OrientationDimension
cg_key* OrientationDimension::key = new cg_key;

OrientationDimension::OrientationDimension(char incrementationKey, char decrementationKey, float angleLimit) {
	this->angle = 0;

	this->incrementationKey = incrementationKey;
	this->decrementationKey = decrementationKey;

	this->angleLimit = angleLimit;
}

void OrientationDimension::update() {
	float INCREMENT = 0.4;

	if (this->key->keyState(this->decrementationKey) != 0)
		this->angle += INCREMENT;

	else if (this->key->keyState(this->incrementationKey) != 0)
		this->angle -= INCREMENT;

	this->clipAngle();
}

void OrientationDimension::clipAngle() {
	this->angle = std::min<float>(std::max<float>(this->angle, -this->angleLimit), this->angleLimit);
}
#pragma endregion

#pragma region Axis
Axis::Axis(OrientationDimension& roll, OrientationDimension& tilt) {
	this->roll = roll;
	this->tilt = tilt;
}

void Axis::update() {
	this->roll.update();
	this->tilt.update();
}
#pragma endregion

#pragma region Robot
Robot::Robot() {}

const void Robot::draw() {
	this->drawPedestal();

	this->lowerAxis.update();
	this->drawArm();
}

const void Robot::drawPedestal() {
	glPushMatrix();
		glColor3f(1.0f, .0f, 0.0f);
		glTranslatef(0, this->PEDASTEL_CEILING_Z_COORDINATE / 2, 0);
			drawCuboid(3, this->PEDASTEL_CEILING_Z_COORDINATE, 4, Color(1.0, 0.0, 0.0, 0.2));
	glPopMatrix();
}

#pragma region Arm
const void Robot::drawArm() {
	this->drawLowerSteelCylinder();
	this->drawLowerAxis();
}

const void Robot::drawLowerSteelCylinder() {
	glPushMatrix();
		glColor3f(.4f, .4f, .4f);
		glTranslatef(0, this->PEDASTEL_CEILING_Z_COORDINATE, 0);
		glRotatef(90, -1, 0, 0);
			drawCylinder(1, 1, this->LOWER_STEEL_CYLINDER_HEIGHT);
	glPopMatrix();
}

const void Robot::drawLowerAxis() {
	static float HEIGHT = 3.;

	glPushMatrix();
		glTranslatef(0, this->LOWER_STEEL_CYLINDER_HEIGHT + this->PEDASTEL_CEILING_Z_COORDINATE + 1.5, 0);
		
		// rotate
		glRotatep(this->lowerAxis.roll.angle, Axes::Z);

		// tilt
		glTranslatef(0, -HEIGHT / 2, 0);
		glRotatep(this->lowerAxis.tilt.angle, Axes::X);
		glTranslatef(0, HEIGHT / 2, 0);
			drawCuboid(0.5, HEIGHT, 0.3, Color(230, 80, 21));
	glPopMatrix();
}
#pragma endregion

#pragma endregion