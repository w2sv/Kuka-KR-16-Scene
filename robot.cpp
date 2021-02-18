#include "robot.h"

#pragma region Axis
cg_key* Axis::key = new cg_key;

#pragma endregion

#pragma region Robot
Robot::Robot() {

}

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
	glPushMatrix();
		glTranslatef(0, this->LOWER_STEEL_CYLINDER_HEIGHT + this->PEDASTEL_CEILING_Z_COORDINATE + 1.5, 0);
		glRotatep(this->lowerAxis.rotationAngle, Z_AXIS);
		// glRotatef(this->lowerAxis.rotationAngle, 0, 1, 0);
		drawCuboid(0.5, 3, 0.3, Color(230, 80, 21));
	glPopMatrix();
}
#pragma endregion

#pragma endregion