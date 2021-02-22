#include "camera.h"


Camera::Camera():
	mode(Mode::Observer)
{
		this->screenPosX = 1956;  // TODO
		this->screenPosY = 883;
}

void Camera::setMode(Mode mode) { this->mode = mode; }

const void Camera::set() {
	switch (this->mode) {
		case Mode::Observer:
			this->setObserverMode(); break;
		case Mode::TCP:
			this->setTCPMode(); break;
	}
}
const void Camera::setObserverMode() {
	double x, y, z, The, Phi;
	static double radius = 60;

	if (cg_mouse::buttonState(GLUT_LEFT_BUTTON)) {
		this->screenPosX += mouse.moveX();
		this->screenPosY += mouse.moveY();
	}
	if (cg_mouse::buttonState(GLUT_MIDDLE_BUTTON)) {
		radius = std::max<float>(radius + 0.1 * mouse.moveY(), 1.0);
	}

	Phi = 0.2 * this->screenPosX / cg_globState::screenSize[0] * M_PI + M_PI * 0.5;
	The = 0.2 * this->screenPosY / cg_globState::screenSize[1] * M_PI;
	x = radius * cos(Phi) * cos(The);
	y = radius * sin(The);
	z = radius * sin(Phi) * cos(The);
	int Oben = (The <= 0.5 * M_PI || The > 1.5 * M_PI) * 2 - 1;

	gluLookAt(x, y, z, 0, 0, 0, 0, Oben, 0);
}

const void Camera::setTCPMode() {}
