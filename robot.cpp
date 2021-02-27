#include "robot.h"


using namespace Axes;


#pragma region OrientationDimension
OrientationDimension::OrientationDimension(char incrementationKey, char decrementationKey, float startAngle, Extrema&& angleLimits): 
	angle(startAngle),
	startAngle(startAngle),
	angleLimits(angleLimits),
	incrementationKey(incrementationKey), 
	decrementationKey(decrementationKey), 
	isFullRangeOfMotionDim(angleLimits.spread() == 360)
{}


void OrientationDimension::update() {
	float INCREMENT = 0.4;

	if (cg_key::keyState(this->decrementationKey) != 0)
		this->angle += INCREMENT;

	else if (cg_key::keyState(this->incrementationKey) != 0)
		this->angle -= INCREMENT;
	else
		return;

	this->clipAngle();
}


void OrientationDimension::clipAngle() {
	if (isFullRangeOfMotionDim) {
		if (angle >= 360)
			angle -= 360;
		else if (angle <= 0)
			angle += 360;
	}
	else
		angle = std::min<float>(std::max<float>(angle, angleLimits.min), angleLimits.max);
}


void OrientationDimension::reset() { 
	this->angle = this->startAngle; 
}


float OrientationDimension::getAngle() const {
	return this->angle;
}


void OrientationDimension::setArbitraryAngle() {
	// Reference: https://stackoverflow.com/a/7560564

	std::random_device rd; // obtain a random number from hardware
	std::mt19937 gen(rd()); // seed the generator
	std::uniform_int_distribution<> distr(angleLimits.min, angleLimits.max);

	this->angle = distr(gen);
}
#pragma endregion



#pragma region Axis
Axis::Axis(OrientationDimension&& orientation):
	orientation(orientation)
{}


void RotationAxis::adjustModelMatrixOrientationAccordingly() const {
	Z::rotate(this->orientation.getAngle()); 
}
	

void TiltAxis::adjustModelMatrixOrientationAccordingly() const {
	Z::rotate(this->orientation.getAngle());
}
#pragma endregion



std::vector<Vector2> discrete2DCircleRadiusPoints(float radius, int nPoints) {
	std::vector<Vector2> circlePoints;

	for (double t = 0; t < 2 * M_PI; t += 2 * M_PI / nPoints)
		circlePoints.push_back(Vector2(cos(t) * radius, sin(t) * radius));

	return circlePoints;
}



#pragma region Robot
const Color Robot::BASE_COLOR = Color(230./255., 80./255., 21./255.);
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
	axes({
		new RotationAxis(OrientationDimension('a', 'd', 0, Extrema(0, 360))),
		new TiltAxis(OrientationDimension('w', 's', 22.5, Extrema(-45, 80))),
		new TiltAxis(OrientationDimension('t', 'g', -20, Extrema(-45, 75))),
		new RotationAxis(OrientationDimension('f', 'h', 0, Extrema(0, 360))) }),

	axis2DrawFunction({
		{axes[0], std::bind(&Robot::drawFirstAxis, this)},
		{axes[1], std::bind(&Robot::drawSecondAxis, this)},
		{axes[2], std::bind(&Robot::drawThirdAxis, this)},
		{axes[3], std::bind(&Robot::drawFourthAxis, this)},
	}),
	drawTCPCoordSystem(false)
{}


Robot::~Robot() {
	for (size_t i = 0; i < this->axes.size(); i++)
		delete this->axes[i];
}


void Robot::update() {
	for (Axis* const axisPointer : this->axes)
		axisPointer->orientation.update();
}


void Robot::reset() {
	for (Axis* const axisPointer : this->axes)
		axisPointer->orientation.reset();
}


void Robot::setArbitraryAxesConfiguration() {
	for (Axis* const axisPointer : this->axes)
		axisPointer->orientation.setArbitraryAngle();
}


void Robot::toggleDrawTCPCoordSystem() {
	this->drawTCPCoordSystem = toggleFlag(this->drawTCPCoordSystem);
}
#pragma endregion


#pragma region Drawing
void Robot::draw() {
	 this->drawPedestal();
	 this->drawLowerSteelCylinder();

	glPushMatrix();
		Z::translate(this->LOWER_STEEL_CYLINDER_HEIGHT + this->PEDASTEL_HEIGHT);
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
		Z::translate(PEDASTEL_HEIGHT / 2);
		OctagonalPrismVertices pedastelVertices = drawOctagonalPrism(PEDASTEL_HEIGHT, 0.5, 0.2);

			Colors::GREY.render();
		drawOctagonalPrismCage(pedastelVertices);

		// draw octPrism block
			Colors::BLACK.render();
			Z::translate(BLOCK_HEIGHT / 2);
		OctagonalPrismVertices blockVertices = drawOctagonalPrism(BLOCK_HEIGHT, 0.3, 0.3);

			Colors::GREY.render();
		drawOctagonalPrismCage(blockVertices);

		// draw upper black cylinder
			Colors::BLACK.render();
			Z::translate(BLOCK_HEIGHT / 2);
		drawCylinder(0.1, 0.1, UPPER_CYLINDER_HEIGTH);
	glPopMatrix();
}


void Robot::drawScrewCircle() const {
	for (size_t i = 0; i < SCREW_POSITIONS.size(); i++) {
		glPushMatrix();
			glTranslatef(SCREW_POSITIONS[i].x, 0, SCREW_POSITIONS[i].y);
			glScaleUniformly(0.2);
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
		Z::translate(this->PEDASTEL_HEIGHT);
		drawCylinder(1.2, 1, LOWER_SEGMENT_HEIGHT);
	glPopMatrix();

	// central segment
	glPushMatrix();
		setColor(0.2, 0.2, 0.2);
		Z::translate(LOWER_SEGMENT_HEIGHT + this->PEDASTEL_HEIGHT);
		drawCylinder(1, 1, CENTRAL_SEGMENT_HEIGHT);
	glPopMatrix();

	// upper segment
	glPushMatrix();
		this->BASE_COLOR.render();
		Z::translate(this->PEDASTEL_HEIGHT + LOWER_SEGMENT_HEIGHT + CENTRAL_SEGMENT_HEIGHT);
		drawCylinder(1.3, 1.3, UPPER_SEGMENT_HEIGHT);
	glPopMatrix();
}
#pragma endregion


#pragma region AxesDrawing
void Robot::drawFirstAxis() const {
	static const float AXIS_MOUNT_DISK_HEIGHT = 0.3;

	axes[0]->adjustModelMatrixOrientationAccordingly();

	// hollow cylinder with second axis mount
	glPushMatrix();
		glTranslatef(0, 0, 0.07);
		glScaleUniformly(2);
		objects[RotationAxis1].draw();
	glPopMatrix();

	// draw axis weigth
	glPushMatrix();
	Z::translate(0.35);  // height of cylinder bottom disk
		glScaleUniformly(1.3);
		drawAxisWeight();
	glPopMatrix();

	// draw second axis mount disk
		BASE_COLOR.render();
	glTranslatef(1.65, 1.63, 0.2);

	glPushMatrix();
		X::rotate(90);
		drawCylinder(0.5, 0.5, AXIS_MOUNT_DISK_HEIGHT);
	glPopMatrix();

	// translate located both at the top and in the center of the second axis mount disk
	glTranslatef(0, 0, AXIS_MOUNT_DISK_HEIGHT);
}


void Robot::drawSecondAxis()const {
	static const float LENGTH = 4.4;

	X::rotate(90);
	this->axes[1]->adjustModelMatrixOrientationAccordingly();

	glPushMatrix();

		// draw axis with objects residing upon it 
		glPushMatrix();
			glTranslatef(0, 0.15, 0);
			
			glPushMatrix();
				glTranslatef(0, 0, -LENGTH / 2);
				
				// draw axis
				glPushMatrix();
				glScaleUniformly(1.7);
					objects[TiltAxis1].draw();
				glPopMatrix();

				// draw upper screw circle
				glPushMatrix();
					glTranslatef(0, 0, -LENGTH / 2);
					this->drawScrewCircle();
				glPopMatrix();
			glPopMatrix();

			// draw lower screw circle
			this->drawScrewCircle();

		glPopMatrix();

		// draw orange axes weight pedastel octPrism
			BASE_COLOR.render();
		glTranslatef(0, -0.5, 0);
		X::rotate(180);
		OctagonalPrismVertices weightPedastelVertices = drawOctagonalPrism(0.15, 0.6, 0.1);

			Colors::BLACK.render();
		drawOctagonalPrismCage(weightPedastelVertices);

		// draw weight
		Z::translate(0.09);
		this->drawAxisWeight();
	glPopMatrix();

	glTranslatef(0, 0, -LENGTH);
}


void Robot::drawThirdAxis() const {
	static const float LENGTH = 3.9;
	static const float WIDTH = 0.5;
	static const float MOUNT_PART_HEIGHT = 1.7;

	this->axes[2]->adjustModelMatrixOrientationAccordingly();

	// draw axis
	glPushMatrix();
		glTranslatef(LENGTH * 0.41, -WIDTH * 0.5, 0);  // translate slightly to the right, up, forward 
		glScaleUniformly(5);
		Y::rotate(90);
		X::rotate(-90);
		objects[Object::TiltAxis2].draw();
	glPopMatrix();

	// draw weight block at axis beginning along respective x-axes
	glPushMatrix();
		glTranslatef(-0.6, -WIDTH / 2, 0);

		static const float LATERAL_SCALING_FACTOR = 0.5;
		static const float Z_INCREMENT = 0.35;

		for (float zTranslation = -Z_INCREMENT; zTranslation <= Z_INCREMENT; zTranslation += Z_INCREMENT) {
			glPushMatrix();
				glTranslatef(0, 0, zTranslation);
				glScalef(1, LATERAL_SCALING_FACTOR, LATERAL_SCALING_FACTOR);
				Y::rotate(90);

				this->drawAxisWeight();
			glPopMatrix();
		}
	glPopMatrix();

	// draw singular weight along y-axis
	glPushMatrix();
		glTranslatef(0.17, -WIDTH, 0);
		glScaleUniformly(0.8);
		X::rotate(180);
		Z::rotate(45);
		this->drawAxisWeight();
	glPopMatrix();

	glTranslatef(LENGTH * 0.975, -WIDTH / 2, 0);
	Y::rotate(270);
}


void Robot::drawFourthAxis() const {
	this->axes[3]->adjustModelMatrixOrientationAccordingly();
	
	glPushMatrix();
		BASE_COLOR.render();

		// partial cone
		drawCylinder(0.2, 0.3, 0.15);

		// short disk
		glTranslatef(0, 0.15, 0);
		drawCylinder(0.3, 0.3, 0.08);

		// long disk
		glTranslatef(0, 0.08, 0);
		drawCylinder(0.27, 0.27, 0.3);

		glTranslatef(0, 0.3, 0);

		// screw circle
		glPushMatrix();
			static const float XY_SCALE_FACTOR = 0.86;
			glScalef(XY_SCALE_FACTOR, 1, XY_SCALE_FACTOR);
			this->drawScrewCircle();
		glPopMatrix();

		BASE_COLOR.render();
		
		// disk
		drawCylinder(0.13, 0.13, 0.1);

		// disk
		glTranslatef(0, 0.1, 0);
		drawCylinder(0.15, 0.15, 0.15);

		// cone
		glTranslatef(0, 0.15, 0);
		drawCylinder(0.15, 0.1, 0.25);

		if (this->drawTCPCoordSystem) {
			static Extrema TCP_COORD_SYSTEM_EXTREMA = Extrema(-1, 1);

			glTranslatef(0, 0.25, 0);
			drawCoordSystem(TCP_COORD_SYSTEM_EXTREMA, TCP_COORD_SYSTEM_EXTREMA, TCP_COORD_SYSTEM_EXTREMA, 0.3);
		}

	glPopMatrix();
}
#pragma endregion

#pragma endregion

#pragma endregion