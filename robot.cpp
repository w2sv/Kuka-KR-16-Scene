#include "robot.h"

#pragma region Axis
cg_key* Robot::Axis::key = new cg_key;

Robot::Axis::Axis(char rotationDecrementalKey, char rotationIncrementalKey, char tiltDecrementalKey, char tiltIncrementalKey, int tiltAngleLimit) {
	this->rotationAngle = 0;
	this->tiltAngle = 0;

	this->tiltAngleLimit = tiltAngleLimit;

	this->rotationDecrementalKey = rotationDecrementalKey;
	this->rotationIncrementalKey = rotationIncrementalKey;

	this->tiltDecrementalKey = tiltDecrementalKey;
	this->tiltIncrementalKey = tiltIncrementalKey;
}

void Robot::Axis::updateOrientation(){
	float INCREMENT = 0.4;

	// rotation angle modification
	if (this->key->keyState(this->rotationDecrementalKey) != 0)
		this->rotationAngle += INCREMENT;

	else if (this->key->keyState(this->rotationIncrementalKey) != 0)
		this->rotationAngle -= INCREMENT;

	// tilt angle modification
	else if (this->key->keyState(this->tiltDecrementalKey) != 0)
		this->tiltAngle+= INCREMENT;

	else if (this->key->keyState(this->tiltIncrementalKey) != 0)
		this->tiltAngle -= INCREMENT;

	this->clipTiltAngle();
}

void Robot::Axis::clipTiltAngle() {
	this->tiltAngle = std::min<float>(std::max<float>(this->tiltAngle, -this->tiltAngleLimit), this->tiltAngleLimit);
}
#pragma endregion

#pragma region Robot
Robot::Robot() {}

const void Robot::draw() {
	this->drawPedestal();

	this->lowerAxis.updateOrientation();
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
		glRotatep(this->lowerAxis.rotationAngle, Axes::Z);

		// tilt
		glTranslatef(0, -HEIGHT / 2, 0);
		glRotatep(this->lowerAxis.tiltAngle, Axes::X);
		glTranslatef(0, HEIGHT / 2, 0);
			drawCuboid(0.5, HEIGHT, 0.3, Color(230, 80, 21));
	glPopMatrix();
}
#pragma endregion

#pragma endregion