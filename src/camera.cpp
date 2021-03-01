#include "camera.h"


Camera::Camera():
	mode(Mode::Observer)
{
		this->screenPosX = 1956;  // TODO
		this->screenPosY = 883;
}

void Camera::setMode(Mode mode) { this->mode = mode; }


const void Camera::set(Robot* robot) {
	switch (this->mode) {
		case Mode::Observer:
			this->setObserverMode(robot); break;
	}
}



void Camera::setObserverMode(Robot* robot) {
	double x, y, z, The, Phi;
	static double radius = 40;

	static const float WHEEL_EVENT_RADIUS_INCREMENT = 1.5;


	// Observer angle movement
	if (cg_mouse::buttonState(GLUT_LEFT_BUTTON)) {
		this->screenPosX += cg_mouse::moveX();
		this->screenPosY += cg_mouse::moveY();
	}

	// Zooming via scroll event
	if (cg_mouse::buttonState(GLUT_MOUSE_WHEEL_UP))
		radius -= WHEEL_EVENT_RADIUS_INCREMENT;
	if (cg_mouse::buttonState(GLUT_MOUSE_WHEEL_DOWN))
		radius += WHEEL_EVENT_RADIUS_INCREMENT;

	// Zooming via mouse movement during pressing of mouse wheel
	if (cg_mouse::buttonState(GLUT_MIDDLE_BUTTON)) {
		radius = std::max<float>(radius + 0.1 * cg_mouse::moveY(), 1.0);
	}

	Phi = 0.2 * this->screenPosX / cg_globState::screenSize[0] * M_PI + M_PI * 0.5;
	The = 0.2 * this->screenPosY / cg_globState::screenSize[1] * M_PI;
	x = radius * cos(Phi) * cos(The);
	y = radius * sin(The);
	z = radius * sin(Phi) * cos(The);
	int Oben = (The <= 0.5 * M_PI || The > 1.5 * M_PI) * 2 - 1;

	gluLookAt(x, y, z, 0, 0, 0, 0, Oben, 0);
}