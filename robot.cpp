#include "robot.h"

#pragma region OrientationDimension
cg_key* OrientationDimension::key = new cg_key;

OrientationDimension::OrientationDimension(char incrementationKey, char decrementationKey, float angleLimit): 
	angle(0), 
	incrementationKey(incrementationKey), 
	decrementationKey(decrementationKey), 
	angleLimit(angleLimit),
	isFullRangeOfMotionDim(angleLimit == 360)
{}

void OrientationDimension::update() {
	float INCREMENT = 0.2;

	bool modified = false;
	if (this->key->keyState(this->decrementationKey) != 0) {
		this->angle += INCREMENT;
		modified = true;
	}

	else if (this->key->keyState(this->incrementationKey) != 0) {
		this->angle -= INCREMENT;
		modified = true;
	}

	if (modified)
		this->clipAngle();
}

void OrientationDimension::clipAngle() {
	this->angle = std::min<float>(std::max<float>(this->angle, -this->angleLimit), this->angleLimit);

	if (this->isFullRangeOfMotionDim && abs(this->angle) == this->angleLimit)
		this->angle = this->angle == this->angleLimit ? -this->angleLimit : this->angleLimit;
}
#pragma endregion

#pragma region Axis
Axis::Axis(OrientationDimension& roll, OrientationDimension& tilt, Measurements& measurements):
	roll(roll),
	tilt(tilt),
	height(measurements.height),
	width(measurements.width),
	depth(measurements.depth),
	centerHeight(measurements.height / 2)
{}

void Axis::update() {
	this->roll.update();
	this->tilt.update();
}

const void Axis::adjustMatrixOrientationAccordingly() {
	// rotate
	glRotatep(this->roll.angle, Axes::Z);

	// tilt
	glTranslatef(0, -this->centerHeight, 0);
	glRotatep(this->tilt.angle, Axes::X);
	glTranslatef(0, this->centerHeight, 0);
}

void Axis::reset() {
	this->roll.angle = 0;
	this->tilt.angle = 0;
}
#pragma endregion

#pragma region Robot

Robot::Robot() :
	lowerAxis(Axis(OrientationDimension('a', 'd', 360), OrientationDimension('s', 'w', 45), Measurements(3, 0.5, 0.7))),
	centralAxis(Axis(OrientationDimension('f', 'h', 360), OrientationDimension('t', 'g', 65), Measurements(2.5, 0.3, 0.5))),
	outerAxis(Axis(OrientationDimension('j', 'l', 360), OrientationDimension('k', 'i', 45), Measurements(2.5, 0.2, 0.3))) {

	this->axisDrawFunction = {
		{&this->lowerAxis, std::bind(&Robot::drawLowerAxis, this)},
		{&this->centralAxis, std::bind(&Robot::drawCentralAxis, this)},
		{&this->outerAxis, std::bind(&Robot::drawOuterAxis, this)}
	};
	this->axes = { &this->lowerAxis, &this->centralAxis, &this->outerAxis };
}

const void Robot::draw() {
	this->drawPedestal();
	this->drawLowerSteelCylinder();
	
	glPushMatrix();
	glTranslateZ(this->LOWER_STEEL_CYLINDER_HEIGHT + this->PEDASTEL_HEIGHT);

		for (Axis* const axisPointer: this->axes)
			this->axisDrawFunction[axisPointer]();
	glPopMatrix();
}

void Robot::update() {
	for (Axis* const axisPointer : this->axes) {
		axisPointer->update();
	}
}

void Robot::reset() {
	for (Axis* const axisPointer : this->axes) {
		axisPointer->reset();
	}
}

#pragma region Drawing
const void Robot::drawPedestal() {
	glPushMatrix();
		glColor3f(1.0f, .0f, 0.0f);
		glTranslatef(0, this->PEDASTEL_HEIGHT / 2, 0);
			drawCuboid(3, this->PEDASTEL_HEIGHT, 4, Color(1.0, 0.0, 0.0, 0.2));
	glPopMatrix();
}

const void Robot::drawLowerSteelCylinder() {
	glPushMatrix();
		glColor3f(.4f, .4f, .4f);
		glTranslatef(0, this->PEDASTEL_HEIGHT, 0);
		glRotatef(90, -1, 0, 0);
			drawCylinder(1, 1, this->LOWER_STEEL_CYLINDER_HEIGHT);
	glPopMatrix();
}

#pragma region AxesDrawing
const void Robot::drawLowerAxis() {
	glTranslateZ(this->lowerAxis.centerHeight);
	this->lowerAxis.adjustMatrixOrientationAccordingly();
		drawCuboid(this->lowerAxis.depth, this->lowerAxis.height, this->lowerAxis.width, BASE_COLOR);
	glTranslateZ(this->lowerAxis.centerHeight);
}

const void Robot::drawCentralAxis() {
	glTranslateZ(this->centralAxis.centerHeight);
	this->centralAxis.adjustMatrixOrientationAccordingly();
		drawCuboid(this->centralAxis.depth, this->centralAxis.height, this->centralAxis.width, BASE_COLOR);
	glTranslateZ(this->centralAxis.centerHeight);
}

const void Robot::drawOuterAxis(){
	/*glTranslatef(this->centralAxis.depth / 2, this->centralAxis.centerHeight, 0);
	glRotatep(90, Axes::Y);
	this->centralAxis.adjustMatrixOrientationAccordingly();
		drawCuboid(this->outerAxis.depth, this->outerAxis.height, this->outerAxis.width, BASE_COLOR);*/
}
#pragma endregion

#pragma endregion

#pragma endregion