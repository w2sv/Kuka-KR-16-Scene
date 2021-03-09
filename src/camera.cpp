#include "camera.h"


using namespace TransformationAxes;


const float Camera::START_RADIUS = 40;
const float Camera::RADIUS_MIN = 7.4;


#pragma region Publics
Camera::Camera(Robot* robot) :
	mode(Mode::Observer),
	robot(robot),
	START_POSITION(Vector2(0, GlobalState::screenSize[1] * 0.5)),
	position(START_POSITION),
	radius(START_RADIUS)
{}



void Camera::toggleMode(Mode mode) {
	this->mode = this->mode == mode ? Mode::Observer : mode;
}



const void Camera::set() {
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
	position = Vector2(START_POSITION);
	radius = START_RADIUS;
}
#pragma endregion



#pragma region ParameterUpdating
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

	if (cg_mouse::buttonState(GLUT_MOUSE_WHEEL_UP)) {
		radius -= DELTA_RADIUS;
		floorRadius();
	}
	else if (cg_mouse::buttonState(GLUT_MOUSE_WHEEL_DOWN))
		radius += DELTA_RADIUS;
}



void Camera::updateRadiusViaWheelPressing() {
	if (cg_mouse::buttonState(GLUT_MIDDLE_BUTTON)) {
		radius = std::max<float>(radius + 0.1 * cg_mouse::moveY(), 1.0);
		floorRadius();
	}
}



void Camera::floorRadius() {
	radius = std::max<float>(radius, RADIUS_MIN);
}
#pragma endregion



#pragma region ModeSetting
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
#pragma endregion