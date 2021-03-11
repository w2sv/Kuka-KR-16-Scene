#pragma once

#include "robot.h"
#include "glutils.h"


class Camera{
public:
	Camera(Robot* robot);
	
	void set();
	void reset();

	enum Mode {
		Observer,
		TCP,
		ReverseTCP,
		Orbit
	};

	void toggleMode(Mode mode);
private:
	static const float START_RADIUS;
	static const float RADIUS_MIN;

	Vector2 position;
	Mode mode;
	float radius;
	Robot* robot;

	void resetPosition();

	void setObserverMode();
	void setTCPMode();
	void setReverseTCPMode();
	void setOrbitMode();

	void setCameraParameterAccordingly();

	void updateRadiusViaScroll();
	void updateRadiusViaWheelPressing();
	void floorRadius();
};