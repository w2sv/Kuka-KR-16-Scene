#include "camera.h"


Camera::Camera() {
	this->screenPosX = 1956;  // TODO
	this->screenPosY = 883;
}

const void Camera::set() {
	static cg_mouse mouse;
	
	double x, y, z, The, Phi;
	static double radius = 60;

	if (cg_mouse::buttonState(GLUT_LEFT_BUTTON)) {
		this->screenPosX += mouse.moveX();
		this->screenPosY += mouse.moveY();
	}
	if (cg_mouse::buttonState(GLUT_MIDDLE_BUTTON)) {
		radius += 0.1 * mouse.moveY();
		if (radius < 1.0) 
			radius = 1.0;
	}

	std::cout << cg_globState::screenSize[0] << " " << cg_globState::screenSize[1] << std::endl;

	Phi = 0.2 * this->screenPosX / cg_globState::screenSize[0] * M_PI + M_PI * 0.5;
	The = 0.2 * this->screenPosY / cg_globState::screenSize[1] * M_PI;
	x = radius * cos(Phi) * cos(The);
	y = radius * sin(The);
	z = radius * sin(Phi) * cos(The);
	int Oben = (The <= 0.5 * M_PI || The > 1.5 * M_PI) * 2 - 1;

	gluLookAt(x, y, z, 0, 0, 0, 0, Oben, 0);
}