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
Axis::Axis(OrientationDimension&& orientation, float startAngle):
	orientation(orientation) 
{ 
	this->orientation.angle = startAngle; 
}


void Axis::update() {
	this->orientation.update();
}


void Axis::reset() { this->orientation.angle = 0; }


void RotationAxis::adjustModelMatrixOrientationAccordingly() const { glRotatep(this->orientation.angle, Axes::Z); }


void TiltAxis::adjustModelMatrixOrientationAccordingly() const { 
	/*glTranslatef(0, -this->centerHeight, 0);
	glRotatep(this->tilt.angle, Axes::X);
	glTranslatef(0, this->centerHeight, 0);*/
}
#pragma endregion



#pragma region Robot
const Color Robot::BASE_COLOR = Color(230, 80, 21);



#pragma region Objects
cg_object3D Robot::objects[Robot::N_OBJECTS] = {};

void Robot::loadObjects() {
	const char* DIR_NAME = ".\\objects\\";
	const char* FILE_NAMES[] = { "first_joint.obj" };

	for (size_t i = 0; i < Robot::N_OBJECTS; i++) {
		Robot::objects[i].load(concatenatedCharPtr(DIR_NAME, FILE_NAMES[i]), false);
	}
}


void Robot::setObjectMaterials() {
	objects[Object::HollowCylinder].setMaterial(Color(BASE_COLOR), 0, 0, 0);
}
#pragma endregion



Robot::Robot() :
	lowerAxis(RotationAxis(OrientationDimension('a', 'd', 360), 0)), 
	axis2DrawFunction({{&this->lowerAxis, std::bind(&Robot::drawLowerAxis, this)} }),
	axes({ &this->lowerAxis})
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
	drawAxisWeight();

	 /*this->drawPedestal();
	 this->drawLowerSteelCylinder();

	glPushMatrix();
		glTranslateZ(this->LOWER_STEEL_CYLINDER_HEIGHT + this->PEDASTEL_HEIGHT);
		for (Axis* const axisPointer : this->axes)
			this->axis2DrawFunction[axisPointer]();
	glPopMatrix();*/
}


void Robot::drawAxisWeight() const {
	static float PEDASTEL_HEIGHT = 0.1;
	static float BLOCK_HEIGHT = 1;

	glPushMatrix();
		// draw lower octPrism pedastel
			Colors::BLACK.render();
		glTranslateZ(PEDASTEL_HEIGHT / 2);
		OctagonalPrismVertices pedastelVertices = drawOctagonalPrism(PEDASTEL_HEIGHT, 0.5, 0.2);

			Colors::GREY.render();
		drawOctagonalPrismCage(pedastelVertices);

		// draw octPrism block
			Colors::BLACK.render();
		glTranslateZ(BLOCK_HEIGHT / 2);
		OctagonalPrismVertices blockVertices = drawOctagonalPrism(BLOCK_HEIGHT, 0.3, 0.3);

			Colors::GREY.render();
		drawOctagonalPrismCage(blockVertices);

		// draw upper black cylinder
			Colors::BLACK.render();
		glTranslateZ(BLOCK_HEIGHT / 2);
		drawCylinder(0.1, 0.1, 0.05);
	glPopMatrix();
}


void Robot::drawScrewHead() const {

}


void Robot::drawPedestal() const {
	glPushMatrix();
		setColor(1.0f, .0f, 0.0f);
		glTranslatef(0, this->PEDASTEL_HEIGHT / 2, 0);
		glScalef(6, this->PEDASTEL_HEIGHT, 5);
			drawCube();
	glPopMatrix();
}


void Robot::drawLowerSteelCylinder() const {
	static float LOWER_SEGMENT_HEIGHT = this->LOWER_STEEL_CYLINDER_HEIGHT * 0.2;
	static float CENTRAL_SEGMENT_HEIGHT = this->LOWER_STEEL_CYLINDER_HEIGHT * 0.7;
	static float UPPER_SEGMENT_HEIGHT = this->LOWER_STEEL_CYLINDER_HEIGHT * 0.1;

	// lower segment
	glPushMatrix();
		this->BASE_COLOR.render();
		glTranslateZ(this->PEDASTEL_HEIGHT);
		drawCylinder(1.2, 1, LOWER_SEGMENT_HEIGHT);
	glPopMatrix();

	// central segment
	glPushMatrix();
		setColor(0.2, 0.2, 0.2);
		glTranslateZ(LOWER_SEGMENT_HEIGHT + this->PEDASTEL_HEIGHT);
		drawCylinder(1, 1, CENTRAL_SEGMENT_HEIGHT);
	glPopMatrix();

	// upper segment
	glPushMatrix();
		this->BASE_COLOR.render();
		glTranslateZ(this->PEDASTEL_HEIGHT + LOWER_SEGMENT_HEIGHT + CENTRAL_SEGMENT_HEIGHT);
		drawCylinder(1.3, 1.3, UPPER_SEGMENT_HEIGHT);
	glPopMatrix();
}


#pragma region AxesDrawing
void Robot::drawLowerAxis() const {
	this->lowerAxis.adjustModelMatrixOrientationAccordingly();

	// hollow cylinder with second axis mount
	glPushMatrix();
		glTranslatef(0, 0, 0.07);
		glScalef(2, 2, 2);
		objects[Object::HollowCylinder].draw();
	glPopMatrix();

	glTranslateZ(0.7);
	drawAxisWeight();
}


void Robot::drawCentralAxis()const {
	
}


void Robot::drawOuterAxis() const {
	
}
#pragma endregion

#pragma endregion

#pragma endregion