#include "camera.h"


#define _USE_MATH_DEFINES
#include <math.h>
#include <limits>

#include "../dependencies/freeglut.h"

#include "input.h"
#include "state.h"



const float Camera::START_RADIUS = 40;
const Extrema Camera::RADIUS_LIMITS(7.4, 400);


Camera::Camera(Robot* robot) :
	mode(Mode::Observer),
	robot(robot),
	radius(START_RADIUS)
{
	resetPosition(); 
}


void Camera::toggleMode(Mode mode) {
	this->mode = this->mode == mode ? Mode::Observer : mode;
}


void Camera::set() {
	switch (mode) {
		case Observer:
			setObserverMode(); break;
		case TCP:
			setTCPMode(); break;
		case ReverseTCP:
			setReverseTCPMode(); break;
		case Orbit:
			setOrbitMode(); break;
	}
}


void Camera::reset() {
	mode = Observer;
	resetPosition();
	radius = START_RADIUS;
}


////////////////////////////////////////////////////////////
/// Parameter Updating
////////////////////////////////////////////////////////////

void Camera::resetPosition() {
	position.set(0, GlobalState::screenSize[1] * 0.5);
}


void Camera::setCameraParameterAccordingly() {
	double phi = 0.2 * position.x / GlobalState::screenSize[0] * M_PI + M_PI * 0.5;
	double theta = 0.2 * position.y / GlobalState::screenSize[1] * M_PI;

	double eyePosX = radius * cos(phi) * cos(theta);
	double eyePosY = radius * sin(theta);
	double eyePosZ = radius * sin(phi) * cos(theta);

	int upY = (theta <= 0.5 * M_PI || theta > 1.5 * M_PI) * 2 - 1;

	gluLookAt(eyePosX, eyePosY, eyePosZ, 0, 0, 0, 0, upY, 0);
}


void Camera::updateRadiusViaScroll() {
	static const float DELTA_RADIUS = 1.5;

	if (cg_mouse::buttonState(GLUT_MOUSE_WHEEL_UP))
		radius -= DELTA_RADIUS;
	else if (cg_mouse::buttonState(GLUT_MOUSE_WHEEL_DOWN))
		radius += DELTA_RADIUS;
	else
		return;

	clipRadius();
}


void Camera::updateRadiusViaWheelPressing() {
	if (cg_mouse::buttonState(GLUT_MIDDLE_BUTTON)) {
		radius = std::max<float>(radius + 0.1 * cg_mouse::moveY(), 1.0);
		clipRadius();
	}
}


void Camera::clipRadius() {
	radius = RADIUS_LIMITS.clippedValue(radius);
}


////////////////////////////////////////////////////////////
/// Mode setting
////////////////////////////////////////////////////////////

void Camera::setObserverMode() {

	// Query position alteration input
	if (cg_mouse::buttonState(GLUT_LEFT_BUTTON)) {
		position.x += cg_mouse::moveX();
		position.y += cg_mouse::moveY();
	}
	
	updateRadiusViaScroll();
	updateRadiusViaWheelPressing();
	
	setCameraParameterAccordingly();
}


void Camera::setTCPMode() {
	robot->attachCameraToTCP();
}


void Camera::setReverseTCPMode() {
	robot->attachCameraToTCPReversely();
}


void Camera::setOrbitMode() {
	static const float X_POSITION_INCREMENT_PER_FRAME = 0.005;

	updateRadiusViaScroll();
	position.x -= int(X_POSITION_INCREMENT_PER_FRAME * GlobalState::screenSize[0]);

	setCameraParameterAccordingly();
}
