#include "robot.h"

Robot::Robot() {

}

const void Robot::draw() {
	this->drawPedestal();
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
	this->drawLowerJoint();
}

const void Robot::drawLowerSteelCylinder() {
	glPushMatrix();
		glColor3f(.4f, .4f, .4f);
		glTranslatef(0, this->PEDASTEL_CEILING_Z_COORDINATE, 0);
		glRotatef(90, -1, 0, 0);
			drawCylinder(1, 1, this->LOWER_STEEL_CYLINDER_HEIGHT);
	glPopMatrix();
}

const void Robot::drawLowerJoint() {
	glPushMatrix();
		glTranslatef(0, this->LOWER_STEEL_CYLINDER_HEIGHT + this->PEDASTEL_CEILING_Z_COORDINATE + 1.5, 0);
		drawCuboid(0.5, 3, 0.3, Color(230, 80, 21));
	glPopMatrix();
}
#pragma endregion