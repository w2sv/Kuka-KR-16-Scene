#include "robot.h"


using namespace Axes;


#pragma region OrientationDimension
OrientationDimension::OrientationDimension(char incrementationKey, char decrementationKey, float startAngle, Extrema&& angleLimits): 
	angle(startAngle),
	startAngle(startAngle),
	angleLimits(angleLimits),
	incrementationKey(incrementationKey), 
	decrementationKey(decrementationKey), 
	isFullRangeOfMotionDim(angleLimits.spread() == 360),
	angleLimitReached_b(false)
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
	this->angleLimitReached_b = this->angle == this->angleLimits.min || this->angle == this->angleLimits.max;
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


bool OrientationDimension::angleLimitReached() const {
	return this->angleLimitReached_b;
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



#pragma region Textures
#define NUM_TEXTURES 2
cg_image textures[NUM_TEXTURES];


enum Texture {
	Knobs,
	Steel
};


void loadTextures() {
	const char* FILE_PATH = "resources\\textures\\";
	const char* FILE_NAMES[NUM_TEXTURES] = {
		"smoothed-square-textured-metal.bmp",
		"warped-sheet-metal_roughness.bmp"
	};

	for (size_t i = 0; i < NUM_TEXTURES; i++) {
		textures[i].load(concatenatedCharPtr(FILE_PATH, FILE_NAMES[i]), true);

		textures[i].setMinFilter(GL_NEAREST);
		textures[i].setMagFilter(GL_LINEAR);
		textures[i].setWrapMode(GL_CLAMP);
		textures[i].setEnvMode(GL_MODULATE);
	}
}
#pragma endregion



#pragma region Robot
const Color Robot::BASE_COLOR = Color(230./255., 80./255., 21./255.);
const std::vector<Vector2> Robot::SCREW_POSITIONS = discrete2DCircleRadiusPoints(0.25, 12);


#pragma region Objects
cg_object3D Robot::objects[Robot::N_OBJECTS] = {};

void Robot::loadObjects() {
	const char* FILE_PATH = "resources\\objects\\";
	const char* FILE_NAMES[Robot::N_OBJECTS] = { 
		"rotation_axis_1.obj", 
		"screw_head.obj", 
		"tilt_axis_1.obj", 
		"tilt_axis_2.obj" 
	};

	for (size_t i = 0; i < Robot::N_OBJECTS; i++) {
		Robot::objects[i].load(concatenatedCharPtr(FILE_PATH, FILE_NAMES[i]), false);
	}
}


void Robot::setObjectMaterials() {
	GLfloat spec = 200;  // 0 -> saturated color
	GLfloat shine = 30;
	GLfloat emis = 1; // 0 -> matt, 1->bright

	objects[Object::RotationAxis1].setMaterial(Color(BASE_COLOR), spec, shine, emis);
	objects[Object::ScrewHead].setMaterial(Color(Colors::GREY), 0.2, 0.5, 0);
	objects[Object::TiltAxis1].setMaterial(Color(BASE_COLOR), spec, shine, emis);
	objects[Object::TiltAxis2].setMaterial(Color(BASE_COLOR), spec, shine, emis);
}
#pragma endregion


#pragma region Publics
Robot::Robot() :
	axes({
		new RotationAxis(OrientationDimension('a', 'd', 0, Extrema(0, 360))),
		new TiltAxis(OrientationDimension('w', 's', 22.5, Extrema(-45, 60))),
		new TiltAxis(OrientationDimension('t', 'g', -20, Extrema(-45, 75))),
		new RotationAxis(OrientationDimension('f', 'h', 0, Extrema(0, 360))) }),

	axis2DrawFunction({
		{axes[0], std::bind(&Robot::drawFirstAxis, this)},
		{axes[1], std::bind(&Robot::drawSecondAxis, this)},
		{axes[2], std::bind(&Robot::drawThirdAxis, this)},
		{axes[3], std::bind(&Robot::drawFourthAxis, this)},
	}),
	drawTCPCoordSystem(false),
	displayAxesAngles_b(true)
{}


Robot::~Robot() {
	for (size_t i = 0; i < this->axes.size(); i++)
		delete this->axes[i];
}


void Robot::update() {
	for (Axis* const axisPointer : this->axes)
		axisPointer->orientation.update();
}


void Robot::displayAxesAngles() const {
	for (size_t i = 0; i < 4; i++) {
		std::ostringstream oss;
		oss << roundf(this->axes[i]->orientation.getAngle() * 100.) / 100.;
		if (this->axes[i]->orientation.angleLimitReached())
			oss << "!";

		projectText(0.8, 0.8 - (i * 0.05), oss.str().c_str(), Color(0.8, 0, 0), GLUT_BITMAP_9_BY_15);
	}
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


void Robot::toggleDisplayAxesAngles() {
	this->displayAxesAngles_b = toggleFlag(this->displayAxesAngles_b);
}


void Robot::assumeSpatialTCPConfiguration() const {
	Z::translate(this->LOWER_STEEL_CYLINDER_HEIGHT + this->PEDASTEL_HEIGHT);

	this->axes[0]->adjustModelMatrixOrientationAccordingly();
	glTranslatef(0, 0, 0.3);
	glTranslatef(1.65, 1.63, 0.2);

	X::rotate(90);
	this->axes[1]->adjustModelMatrixOrientationAccordingly();
	glTranslatef(0, 0, -4.4);

	this->axes[2]->adjustModelMatrixOrientationAccordingly();
	glTranslatef(3.9 * 0.965, -0.5 / 2, 0);
	Y::rotate(270);

	this->axes[3]->adjustModelMatrixOrientationAccordingly();
	glTranslatef(0, 1.03, 0);
}
#pragma endregion


#pragma region Drawing
void Robot::draw() {
	this->drawPedestal();
	this->drawBase();

	glPushMatrix();
	Z::translate(this->LOWER_STEEL_CYLINDER_HEIGHT + this->PEDASTEL_HEIGHT);
	for (Axis* const axisPointer : this->axes)
		this->axis2DrawFunction[axisPointer]();
	glPopMatrix();

	if (this->displayAxesAngles_b)
		this->displayAxesAngles();
}


#pragma region Parts
void Robot::drawAxisWeight() const {
	const static float PEDASTEL_HEIGHT = 0.1;
	const static float BLOCK_HEIGHT = 1;
	const static float UPPER_CYLINDER_HEIGTH = 0.05;

	glPushMatrix();
		setDefaultLightAndMaterial(true);

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
		glTranslatef(0, this->PEDASTEL_HEIGHT / 2, 0);
		glScalef(8, this->PEDASTEL_HEIGHT, 8);
			
		Color(.6, .6, .6).render();
		textures[Texture::Knobs].bind();
		
		glEnable(GL_TEXTURE_2D);
			drawCube();
		glDisable(GL_TEXTURE_2D);

	glPopMatrix();
}


void Robot::drawBase() const {
	static float LOWER_SEGMENT_HEIGHT = this->LOWER_STEEL_CYLINDER_HEIGHT * 0.2;
	static float CENTRAL_SEGMENT_HEIGHT = this->LOWER_STEEL_CYLINDER_HEIGHT * 0.7;
	static float UPPER_SEGMENT_HEIGHT = this->LOWER_STEEL_CYLINDER_HEIGHT * 0.1;
	
	textures[Texture::Steel].bind();
	glEnable(GL_TEXTURE_2D);

		// lower segment
		glPushMatrix();
			Z::translate(this->PEDASTEL_HEIGHT);
				this->BASE_COLOR.render();
				drawCylinder(1.4, 1, LOWER_SEGMENT_HEIGHT);

			// central segment
			Z::translate(LOWER_SEGMENT_HEIGHT);
				setColor(.4, .4, .4);
				drawCylinder(1, 1, CENTRAL_SEGMENT_HEIGHT);

			// upper segment
			Z::translate(CENTRAL_SEGMENT_HEIGHT);
				this->BASE_COLOR.render();
				drawCylinder(1.3, 1.3, UPPER_SEGMENT_HEIGHT);
		glPopMatrix();

	glDisable(GL_TEXTURE_2D);
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
			
		textures[Texture::Steel].bind();
		glEnable(GL_TEXTURE_2D);
			objects[RotationAxis1].draw();
		glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	// draw axis weigth
	glPushMatrix();
		Z::translate(0.35);  // height of cylinder bottom disk
		glScaleUniformly(1.3);
		drawAxisWeight();
	glPopMatrix();

	// draw second axis mount disk
	glTranslatef(1.65, 1.63, 0.2);
		BASE_COLOR.render();

	glPushMatrix();
		X::rotate(90);
		
		textures[Texture::Steel].bind();
		glEnable(GL_TEXTURE_2D);
			drawCylinder(0.5, 0.5, AXIS_MOUNT_DISK_HEIGHT);
		glDisable(GL_TEXTURE_2D);
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
				
					textures[Texture::Steel].bind();
					glEnable(GL_TEXTURE_2D);
						objects[TiltAxis1].draw();
					glDisable(GL_TEXTURE_2D);
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
		glTranslatef(0, -0.5, 0);
		X::rotate(180);
			BASE_COLOR.render();
	
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
			textures[Texture::Steel].bind();
		glEnable(GL_TEXTURE_2D);
			objects[TiltAxis2].draw();
		glDisable(GL_TEXTURE_2D);
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

	glTranslatef(LENGTH * 0.965, -WIDTH / 2, 0);
	Y::rotate(270);
}


void Robot::drawFourthAxis() const {
	this->axes[3]->adjustModelMatrixOrientationAccordingly();
	
	glPushMatrix();
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