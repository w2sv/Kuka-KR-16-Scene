#include "camera.h"


using namespace TransformationAxes;


const float Camera::START_RADIUS = 40;
const float Camera::RADIUS_MIN = 7.4;


#pragma region Publics
Camera::Camera(Robot* robot) :
	mode(Mode::Observer),
	robot(robot),
	START_POSITION(Vector2(0, cg_globState::screenSize[1] * 0.5)),
	position(START_POSITION),
	radius(START_RADIUS)
{}



void Camera::toggleMode() {
	static const int TOGGLE_MODES = 2;

	this->mode = Mode((this->mode + 1) % TOGGLE_MODES);
}



void Camera::toggleOrbitMode() {
	mode = mode == Mode::Orbit ? Mode::Observer : Mode::Orbit;
}



const void Camera::set() {
	switch (mode) {
		case Observer:
			setObserverMode(); break;
		case TCP:
			setTCPMode(); break;
		case Orbit:
			setOrbitMode(); break;
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



void Camera::setWalkMode() {
	const static float DELTA_POSITION = 0.1;
	
	static float xStep = 0;
	static float yStep = 0;

	if (cg_key::specialKeyState(GLUT_KEY_DOWN))
		yStep -= DELTA_POSITION;
	if (cg_key::specialKeyState(GLUT_KEY_UP))
		yStep += DELTA_POSITION;
	if (cg_key::specialKeyState(GLUT_KEY_LEFT))
		xStep += DELTA_POSITION;
	if (cg_key::specialKeyState(GLUT_KEY_RIGHT))
		xStep -= DELTA_POSITION;

	X::translate(xStep);
	Y::translate(yStep);

	setCameraParameterAccordingly();
}



void Camera::setTCPMode() {
	// gluPerspective(300, 3, 0, 0);

	robot->assumeSpatialTCPConfiguration();
	// glFrustum(0, 0, 0, 0, 0, -2);
}



void Camera::setOrbitMode() {
	static const float X_POSITION_INCREMENT_PER_FRAME = 0.005;

	updateRadiusViaScroll();
	position.x -= int(X_POSITION_INCREMENT_PER_FRAME * cg_globState::screenSize[0]);

	setCameraParameterAccordingly();
}
#pragma endregion