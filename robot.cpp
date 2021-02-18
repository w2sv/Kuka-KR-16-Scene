#include "robot.h"

#pragma region OrientationDimension
cg_key* OrientationDimension::key = new cg_key;

OrientationDimension::OrientationDimension(char incrementationKey, char decrementationKey, float angleLimit): 
	angle(0), 
	incrementationKey(incrementationKey), 
	decrementationKey(decrementationKey), 
	angleLimit(angleLimit) 
{}

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
Axis::Axis(OrientationDimension& roll, OrientationDimension& tilt):
	roll(roll),
	tilt(tilt)
{}

void Axis::update() {
	this->roll.update();
	this->tilt.update();
}
#pragma endregion

#pragma region Robot
Robot::Robot():
	lowerAxis(Axis(OrientationDimension('a', 'd', 360), OrientationDimension('s', 'w', 45))),
	centralAxis(Axis(OrientationDimension('f', 'h', 360), OrientationDimension('t', 'g', 45)))
{}

const void Robot::draw() {
	this->drawPedestal();

	this->lowerAxis.update();
	this->centralAxis.update();
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
	this->drawCentralAxis();
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
	glPushMatrix();
		glTranslatef(0, this->LOWER_STEEL_CYLINDER_HEIGHT + this->PEDASTEL_CEILING_Z_COORDINATE + this->LOWER_AXIS_HEIGHT / 2, 0);
		
		// rotate
		glRotatep(this->lowerAxis.roll.angle, Axes::Z);

		// tilt
		glTranslatef(0, -this->LOWER_AXIS_HEIGHT / 2, 0);
		glRotatep(this->lowerAxis.tilt.angle, Axes::X);
		glTranslatef(0, this->LOWER_AXIS_HEIGHT / 2, 0);
			drawCuboid(0.7, this->LOWER_AXIS_HEIGHT, 0.3, BASE_COLOR);
}

const void Robot::drawCentralAxis() {
	static float HEIGHT = 2.5;

	glTranslatef(0, this->LOWER_AXIS_HEIGHT / 2 + HEIGHT / 2, 0);

	// rotate
	glRotatep(this->centralAxis.roll.angle, Axes::Z);

	// tilt
	glTranslatef(0, -HEIGHT / 2, 0);
	glRotatep(this->centralAxis.tilt.angle, Axes::X);
	glTranslatef(0, HEIGHT / 2, 0);
	drawCuboid(0.5, HEIGHT, 0.3, BASE_COLOR);

	glPopMatrix();
}
#pragma endregion

#pragma endregion