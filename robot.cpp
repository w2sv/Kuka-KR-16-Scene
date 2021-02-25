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
Axis::Axis(OrientationDimension&& roll, OrientationDimension&& tilt, Measurements&& measurements):
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


void Axis::adjustModelMatrixOrientationAccordingly() const {
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
const Color Robot::BASE_COLOR = Color(230, 80, 21);



#pragma region Objects
cg_object3D Robot::objects[Robot::N_OBJECTS] = {};

void Robot::loadObjects() {
	const char* DIR_NAME = ".\\objects\\";
	const char* FILE_NAMES[] = { "hollow_curved_cylinder.obj" };

	for (size_t i = 0; i < Robot::N_OBJECTS; i++) {
		Robot::objects[i].load(concatenatedCharPtr(DIR_NAME, FILE_NAMES[i]), false);
	}
}


void Robot::setObjectMaterials() {
	objects[Object::HollowCylinder].setMaterial(BASE_COLOR.r, BASE_COLOR.g, BASE_COLOR.b, BASE_COLOR.o, 0.5, 0.5, 0.8);
}
#pragma endregion



Robot::Robot() :
	lowerAxis(Axis(
		OrientationDimension('a', 'd', 360), 
		OrientationDimension('s', 'w', 45), 
		Measurements(3, 0.5, 0.7))),
	centralAxis(Axis(
		OrientationDimension('f', 'h', 360), 
		OrientationDimension('t', 'g', 65), 
		Measurements(2.5, 0.3, 0.5))),
	outerAxis(Axis(
		OrientationDimension('j', 'l', 360), 
		OrientationDimension('k', 'i', 45), 
		Measurements(2.5, 0.2, 0.3))),
	axis2DrawFunction({
		{&this->lowerAxis, std::bind(&Robot::drawLowerAxis, this)},
		{&this->centralAxis, std::bind(&Robot::drawCentralAxis, this)},
		{&this->outerAxis, std::bind(&Robot::drawOuterAxis, this)}}),
	axes({ &this->lowerAxis, &this->centralAxis, &this->outerAxis })
{}


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
void Robot::draw() {
	this->drawPedestal();
	this->drawLowerSteelCylinder();

	glPushMatrix();
		glTranslateZ(this->LOWER_STEEL_CYLINDER_HEIGHT + this->PEDASTEL_HEIGHT);
		this->BASE_COLOR.renderMaterialized();
		for (Axis* const axisPointer : this->axes)
			this->axis2DrawFunction[axisPointer]();
	glPopMatrix();
}


void Robot::drawScrewHead() const {

}


void Robot::drawPedestal() const {
	glPushMatrix();
		setColor(1.0f, .0f, 0.0f);
		glTranslatef(0, this->PEDASTEL_HEIGHT / 2, 0);
		glScalef(4, this->PEDASTEL_HEIGHT, 3);
			drawCube();
	glPopMatrix();
}


void Robot::drawLowerSteelCylinder() const {
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
void Robot::drawLowerAxis() const {
	glTranslateZ(this->lowerAxis.centerHeight);
	this->lowerAxis.adjustModelMatrixOrientationAccordingly();
		drawCuboid(this->lowerAxis.length, this->lowerAxis.height, this->lowerAxis.width);

		glPushMatrix();
		glTranslatef(0, 3, 0);
		objects[Object::HollowCylinder].draw();
		glPopMatrix();

	glTranslateZ(this->lowerAxis.centerHeight);
}


void Robot::drawCentralAxis()const {
	glTranslateZ(this->centralAxis.centerHeight);
	this->centralAxis.adjustModelMatrixOrientationAccordingly();
		drawCuboid(this->centralAxis.length, this->centralAxis.height, this->centralAxis.width);
	glTranslateZ(this->centralAxis.centerHeight);
}


void Robot::drawOuterAxis() const {
	
}
#pragma endregion

#pragma endregion

#pragma endregion