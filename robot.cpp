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
	orientation(orientation),
	startAngle(startAngle)
{ 
	this->orientation.angle = startAngle; 
}


void Axis::update() {
	this->orientation.update();
}


void Axis::reset() { this->orientation.angle = this->startAngle; }


void RotationAxis::adjustModelMatrixOrientationAccordingly() const { glRotatep(this->orientation.angle, Axes::Z); }


TiltAxis::TiltAxis(OrientationDimension&& orientation, float startAngle, float length):
	Axis(std::move(orientation), startAngle),
	halvedLength(length / 2)
{}
	

void TiltAxis::adjustModelMatrixOrientationAccordingly() const {
	/*float orientationAngle_Radian = toRadian(this->orientation.angle);

	float shiftVectorZ = sin(orientationAngle_Radian) * this->halvedLength;
	float shiftVectorX = cos(orientationAngle_Radian) * this->halvedLength;*/

	// glTranslatef(0, -shiftVectorZ, -shiftVectorX);
	glRotatep(this->orientation.angle, Axes::Z);
	// glTranslatef(0, shiftVectorZ, shiftVectorX);
}
#pragma endregion



#pragma region Robot
const Color Robot::BASE_COLOR = Color(230, 80, 21);
const std::vector<Vector2> Robot::SCREW_POSITIONS = discrete2DCircleRadiusPoints(0.25, 12);


#pragma region Objects
cg_object3D Robot::objects[Robot::N_OBJECTS] = {};

void Robot::loadObjects() {
	const char* DIR_NAME = ".\\objects\\";
	const char* FILE_NAMES[] = { "rotation_axis_1.obj", "screw_head.obj", "tilt_axis_1.obj", "tilt_axis_2.obj" };

	for (size_t i = 0; i < Robot::N_OBJECTS; i++) {
		Robot::objects[i].load(concatenatedCharPtr(DIR_NAME, FILE_NAMES[i]), false);
	}
}


void Robot::setObjectMaterials() {
	objects[Object::RotationAxis1].setMaterial(Color(BASE_COLOR), 0, 0, 0);
	objects[Object::ScrewHead].setMaterial(Color(Colors::GREY), 0, 0, 0);
	objects[Object::TiltAxis1].setMaterial(Color(BASE_COLOR), 0, 0, 0);
	objects[Object::TiltAxis2].setMaterial(Color(BASE_COLOR), 0, 0, 0);
}
#pragma endregion



#pragma region Publics
Robot::Robot() :
	firstAxis(RotationAxis(OrientationDimension('a', 'd', 360), 0)), 
	secondAxis(TiltAxis(OrientationDimension('w', 's', 45), 22.5, 2.2)),
	thirdAxis(TiltAxis(OrientationDimension('t', 'g', 45), 0., 2.)),

	axis2DrawFunction({
		{&this->firstAxis, std::bind(&Robot::drawFirstAxis, this)},
		{&this->secondAxis, std::bind(&Robot::drawSecondAxis, this)},
		{&this->thirdAxis, std::bind(&Robot::drawThirdAxis, this)}
	}),
	axes({ &this->firstAxis, &this->secondAxis, &this->thirdAxis})
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
#pragma endregion


#pragma region Drawing
void Robot::draw() {
	 this->drawPedestal();
	 this->drawLowerSteelCylinder();

	glPushMatrix();
		glTranslateZ(this->LOWER_STEEL_CYLINDER_HEIGHT + this->PEDASTEL_HEIGHT);
		for (Axis* const axisPointer : this->axes)
			this->axis2DrawFunction[axisPointer]();
	glPopMatrix();
}


#pragma region Parts
void Robot::drawAxisWeight() const {
	const static float PEDASTEL_HEIGHT = 0.1;
	const static float BLOCK_HEIGHT = 1;
	const static float UPPER_CYLINDER_HEIGTH = 0.05;

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
		drawCylinder(0.1, 0.1, UPPER_CYLINDER_HEIGTH);
	glPopMatrix();
}


void Robot::drawScrewCircle() const {
	for (size_t i = 0; i < SCREW_POSITIONS.size(); i++) {
		glPushMatrix();
			glTranslatef(SCREW_POSITIONS[i].x, 0, SCREW_POSITIONS[i].y);
			glScalef(0.2, 0.2, 0.2);
			objects[ScrewHead].draw();
		glPopMatrix();
	}
}
#pragma endregion



#pragma region Components
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
#pragma endregion


#pragma region AxesDrawing
void Robot::drawFirstAxis() const {
	static const float AXIS_MOUNT_DISK_HEIGHT = 0.3;

	this->firstAxis.adjustModelMatrixOrientationAccordingly();

	// hollow cylinder with second axis mount
	glPushMatrix();
		glTranslatef(0, 0, 0.07);
		glScalef(2, 2, 2);
		objects[RotationAxis1].draw();
	glPopMatrix();

	// draw axis weigth
	glPushMatrix();
		glTranslateZ(0.35);  // height of cylinder bottom disk
		drawAxisWeight();
	glPopMatrix();

	// draw second axis mount disk
		BASE_COLOR.render();
	glTranslatef(1.65, 1.63, 0.2);
	glRotatep(90, Axes::X);
	drawCylinder(0.5, 0.5, AXIS_MOUNT_DISK_HEIGHT);

	// translate located both at the top and in the center of the second axis mount disk
	glTranslateZ(AXIS_MOUNT_DISK_HEIGHT);
}


void Robot::drawSecondAxis()const {
	secondAxis.adjustModelMatrixOrientationAccordingly();

	glPushMatrix();

		// draw axis with objects residing upon it 
		glPushMatrix();
			glTranslatef(0, 0.15, 0);
			
			glPushMatrix();
				glTranslatef(0, 0, -this->secondAxis.halvedLength * 2);
				
				// draw axis
				glPushMatrix();
					glScalef(1.7, 1.7, 1.7);
					objects[TiltAxis1].draw();
				glPopMatrix();

				// draw upper screw circle
				glPushMatrix();
					glTranslatef(0, 0, -this->secondAxis.halvedLength * 2);
					this->drawScrewCircle();
				glPopMatrix();
			glPopMatrix();

			// draw lower screw circle
			this->drawScrewCircle();

		glPopMatrix();

		// draw orange axesweight pedastel octPrism
			BASE_COLOR.render();
		glTranslatef(0, -0.5, 0);
		glRotatep(180, Axes::X);
		OctagonalPrismVertices weightPedastelVertices = drawOctagonalPrism(0.15, 0.6, 0.1);

			Colors::BLACK.render();
		drawOctagonalPrismCage(weightPedastelVertices);

		// draw weight
		glTranslateZ(0.09);
		this->drawAxisWeight();
	glPopMatrix();

	glTranslatef(0, 0, -this->secondAxis.halvedLength * 4);
}


void Robot::drawThirdAxis() const {
	this->thirdAxis.adjustModelMatrixOrientationAccordingly();

	glPushMatrix();
		glTranslatef(-1.8, -0.25, -0.95);
		glScaleUniformly(5);
		glRotatep(90, Axes::Y);
		glRotatep(65, Axes::X);
		objects[Object::TiltAxis2].draw();
	glPopMatrix();
}
#pragma endregion

#pragma endregion

#pragma endregion