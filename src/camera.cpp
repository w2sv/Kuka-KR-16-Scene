#include "camera.h"


#pragma region Publics
Camera::Camera(Robot* robot) :
	mode(Mode::Observer),
	robot(robot),

	position(START_POSITION),
	radius(START_RADIUS)
{}


void Camera::toggleMode() {
	static const int TOGGLE_MODES = 2;

	this->mode = Mode((this->mode + 1) % TOGGLE_MODES);
}


void Camera::toggleGyrateMode() {
	mode = mode == Mode::Gyrate ? Mode::Observer : Mode::Gyrate;
}



const void Camera::set() {
	switch (mode) {
		case Observer:
			setObserverMode(); break;
		case TCP:
			setTCPMode(); break;
		case Gyrate:
			setGyrateMode(); break;
	}
}


void Camera::reset() {
	position = Vector2(START_POSITION);
	radius = START_RADIUS;
}
#pragma endregion



#pragma region ParameterUpdating
void Camera::setCameraParameterAccordingly() {
	double phi = 0.2 * position.x / cg_globState::screenSize[0] * M_PI + M_PI * 0.5;
	double theta = 0.2 * position.y / cg_globState::screenSize[1] * M_PI;

	double eyePosX = radius * cos(phi) * cos(theta);
	double eyePosY = radius * sin(theta);
	double eyePosZ = radius * sin(phi) * cos(theta);

	int upY = (theta <= 0.5 * M_PI || theta > 1.5 * M_PI) * 2 - 1;

	gluLookAt(eyePosX, eyePosY, eyePosZ, 0, 0, 0, 0, upY, 0);
}


void Camera::updateRadiusViaScroll() {
	static const float WHEEL_EVENT_RADIUS_INCREMENT = 1.5;

	if (cg_mouse::buttonState(GLUT_MOUSE_WHEEL_UP))
		radius -= WHEEL_EVENT_RADIUS_INCREMENT;
	else if (cg_mouse::buttonState(GLUT_MOUSE_WHEEL_DOWN))
		radius += WHEEL_EVENT_RADIUS_INCREMENT;
}
#pragma endregion



#pragma region ModeSetting
void Camera::setObserverMode() {

	// Query position alteration input
	if (cg_mouse::buttonState(GLUT_LEFT_BUTTON)) {
		position.x += cg_mouse::moveX();
		position.y += cg_mouse::moveY();
	}

	// Zooming via scroll event
	updateRadiusViaScroll();

	// Zooming via mouse movement during pressing of mouse wheel
	if (cg_mouse::buttonState(GLUT_MIDDLE_BUTTON))
		radius = std::max<float>(radius + 0.1 * cg_mouse::moveY(), 1.0);

	setCameraParameterAccordingly();
}


void Camera::setTCPMode() {
	// TODO: debug

	this->robot->assumeSpatialTCPConfiguration();
}


void Camera::setGyrateMode() {
	static const float X_POSITION_INCREMENT_PER_FRAME = 0.005;


	updateRadiusViaScroll();
	position.x -= int(X_POSITION_INCREMENT_PER_FRAME * cg_globState::screenSize[0]);

	setCameraParameterAccordingly();
}
#pragma endregion