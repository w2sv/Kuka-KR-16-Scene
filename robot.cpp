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
	length(measurements.depth),
	centerHeight(measurements.height / 2)
{}

void Axis::update() {
	this->roll.update();
	this->tilt.update();
}

const void Axis::adjustModelMatrixOrientationAccordingly() {
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
const void Robot::draw() {
	this->drawPedestal();
	this->drawLowerSteelCylinder();

	glPushMatrix();
	glTranslateZ(this->LOWER_STEEL_CYLINDER_HEIGHT + this->PEDASTEL_HEIGHT);
	this->BASE_COLOR.renderMaterialized();
	for (Axis* const axisPointer : this->axes)
		this->axisDrawFunction[axisPointer]();
	glPopMatrix();
}

const void drawScrewHead() {

}

const void Robot::drawPedestal() {
	glPushMatrix();
		setColor(1.0f, .0f, 0.0f);
		glTranslatef(0, this->PEDASTEL_HEIGHT / 2, 0);
		glScalef(4, this->PEDASTEL_HEIGHT, 3);
			drawCube();
	glPopMatrix();
}

const void Robot::drawLowerSteelCylinder() {
	static float LOWER_SEGMENT_HEIGHT = this->LOWER_STEEL_CYLINDER_HEIGHT * 0.2;
	static float CENTRAL_SEGMENT_HEIGHT = this->LOWER_STEEL_CYLINDER_HEIGHT * 0.7;
	static float UPPER_SEGMENT_HEIGHT = this->LOWER_STEEL_CYLINDER_HEIGHT * 0.1;

	// lower segment
	glPushMatrix();
		this->BASE_COLOR.renderMaterialized();
		glTranslateZ(this->PEDASTEL_HEIGHT);
		glRotatep(270, Axes::X);
		drawCylinder(1.2, 1, LOWER_SEGMENT_HEIGHT);
	glPopMatrix();

	// central segment
	glPushMatrix();
		setColor(0.2, 0.2, 0.2);
		glTranslateZ(LOWER_SEGMENT_HEIGHT + this->PEDASTEL_HEIGHT);
		glRotatep(270, Axes::X);
		drawCylinder(1, 1, CENTRAL_SEGMENT_HEIGHT);
	glPopMatrix();

	// upper segment
	glPushMatrix();
		this->BASE_COLOR.renderMaterialized();
		glTranslateZ(this->PEDASTEL_HEIGHT + LOWER_SEGMENT_HEIGHT + CENTRAL_SEGMENT_HEIGHT);
		glRotatep(270, Axes::X);
		drawCylinder(1.3, 1.3, UPPER_SEGMENT_HEIGHT);
	glPopMatrix();
}

#pragma region AxesDrawing
const void Robot::drawLowerAxis() {
	glTranslateZ(this->lowerAxis.centerHeight);
	this->lowerAxis.adjustModelMatrixOrientationAccordingly();
		drawCuboid(this->lowerAxis.length, this->lowerAxis.height, this->lowerAxis.width);

		glPushMatrix();
			glTranslatef(3, 0, 0);
			static cg_object3D hollowCylinder;
			hollowCylinder.load("obj_files\\hollow_curved_cylinder.obj", false);
			hollowCylinder.draw();
		glPopMatrix();

	glTranslateZ(this->lowerAxis.centerHeight);
}

const void Robot::drawCentralAxis() {
	glTranslateZ(this->centralAxis.centerHeight);
	this->centralAxis.adjustModelMatrixOrientationAccordingly();
		drawCuboid(this->centralAxis.length, this->centralAxis.height, this->centralAxis.width);
	glTranslateZ(this->centralAxis.centerHeight);
}

const void Robot::drawOuterAxis(){
	
}
#pragma endregion

#pragma endregion

#pragma endregion