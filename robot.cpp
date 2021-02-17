#include "robot.h"

Robot::Robot() {

}

void Robot::draw() {
	this->drawPedestal();
}

void Robot::drawPedestal() {
	glPushMatrix();
		glTranslatef(0, 1.0f, 0);
		glColor3f(1.0f, .0f, 0.0f);
		drawCuboid(3, 2, 4, Color(1.0, 0.0, 0.0));
	glPopMatrix();
}