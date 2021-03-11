#include <iostream>  // !
#include <limits>
#include <functional>
#include <map>
#include <vector>
#include <sstream>
#include <string>
#include <math.h>

#include "../dependencies/freeglut.h"

#include "geometry.h"
#include "input.h"
#include "wavefront.h"
#include "glutils.h"
#include "utils.h"
#include "image.h"
#include "state.h"
#include "help.h"

#include "robot.h"


using namespace TransformationAxes;


#pragma region AxisParameterState
AxisParameterState::AxisParameterState(float startValue, Extrema&& limits):
	startValue(startValue),
	value(startValue),
	limits(limits),
	limitReached_b(false)
{}

AxisParameterState::~AxisParameterState()
{}

float AxisParameterState::getValue() const {
	return value;
}
bool AxisParameterState::limitReached() const {
	return limitReached_b;
}
void AxisParameterState::reset() {
	value = startValue;
}

void AxisParameterState::updateLimitReached() {
	limitReached_b = value == limits.min || value == limits.max;
}
void AxisParameterState::clipValue() {
	value = std::min<float>(std::max<float>(value, limits.min), limits.max);
}


void AxisParameterState::operator+=(float increment) {
	value += increment;
}
void AxisParameterState::operator-=(float decrement) {
	value -= decrement;
}
bool AxisParameterState::operator<=(float value) {
	return this->value <= value;
}
bool AxisParameterState::operator>=(float value) {
	return this->value >= value;
}


AngleState::AngleState(float startValue, Extrema&& limits, char incrementationKey, char decrementationKey, bool invertedControl) :
	AxisParameterState::AxisParameterState(startValue, std::move(limits)),
	incrementationKey(incrementationKey),
	decrementationKey(decrementationKey),
	incrementationStepCoeff(invertedControl ? -1 : 1),
	decrementationStepCoeff(invertedControl ? 1 : -1)
{}
void AngleState::setArbitrarily() {
	value = drawArbitraryValue();
}
int AngleState::drawArbitraryValue() const {
	return randInt(limits);
}
VelocityState::VelocityState(float max, char identificationKey):
	AxisParameterState::AxisParameterState(0.4, Extrema(0.1, max)),
	identificationKey(identificationKey)
{}
#pragma endregion



#pragma region OrientationDimension
constexpr int UNINITIALIZED = -1;

OrientationDimension::OrientationDimension(AngleState&& angle, VelocityState&& velocity):
	angle(std::move(angle)),
	velocity(std::move(velocity)),

	targetAngle(UNINITIALIZED),
	targetAngleApproachManner(UNINITIALIZED),
	targetAngleState(TargetAngleState::Disabled)
{}
OrientationDimension::~OrientationDimension()
{}

void OrientationDimension::update() {
	updateVelocity();
	updateAngle();
}
void OrientationDimension::updateVelocity() {
	static const float INCREMENT = 0.01;

	if (cg_key::keyState(velocity.identificationKey) == 2) {
		if (cg_key::keyState('+') != 0)
			velocity += INCREMENT;
		else if (cg_key::keyState('-') != 0)
			velocity -= INCREMENT;
		else
			return;

		velocity.clipValue();
		velocity.updateLimitReached();
	}
}
void OrientationDimension::updateAngle() {
	switch (targetAngleState) {
		case (TargetAngleState::YetToBeReached):
				approachTargetAngle();
				adjustAngle();
				break;
		case (TargetAngleState::Disabled):
			if (cg_key::keyState(angle.decrementationKey) != 0)
				angle += angle.incrementationStepCoeff * velocity.getValue();
			else if (cg_key::keyState(angle.incrementationKey) != 0)
				angle += angle.decrementationStepCoeff * velocity.getValue();
			else
				return;

			adjustAngle();
			angle.updateLimitReached();
	}
}
void UnlimitedOrientationDimension::adjustAngle() {
	if (angle >= 360)
		angle -= 360;
	else if (angle <= 0)
		angle += 360;
}
void LimitedOrientationDimension::adjustAngle() {
	angle.clipValue();
}

void OrientationDimension::reset() {
	angle.reset();
	velocity.reset();
}



////////////////////////////////////////////////////////////
/// Target Angle
////////////////////////////////////////////////////////////
void OrientationDimension::setTargetAngleParameters() {
	targetAngle = angle.drawArbitraryValue();
	if (targetAngle == angle.getValue()) {
		targetAngleState = TargetAngleState::Reached;
		return;
	}

	determineTargetAngleApproachManner();
	targetAngleState = TargetAngleState::YetToBeReached;
}
void UnlimitedOrientationDimension::determineTargetAngleApproachManner() {
	float decrementalDifference = targetAngle > angle.getValue() ? angle.getValue() + 360 - targetAngle : angle.getValue() - targetAngle;
	float incrementalDifference = targetAngle < angle.getValue() ? 360 - angle.getValue() + targetAngle : targetAngle - angle.getValue();

	targetAngleApproachManner = incrementalDifference < decrementalDifference;
}
void LimitedOrientationDimension::determineTargetAngleApproachManner() {
	targetAngleApproachManner = targetAngle > angle.getValue();
}

void OrientationDimension::approachTargetAngle() {
	float step = std::min<float>(velocity.getValue(), abs(targetAngle - angle.getValue()));
	if (targetAngleApproachManner == 1)
		angle += step;
	else
		angle -= step;

	// update target angle state if applicable
	if (targetAngle == angle.getValue())
		targetAngleState = TargetAngleState::Reached;
}

OrientationDimension::TargetAngleState OrientationDimension::getTargetAngleState() const {
	return targetAngleState;
}

void OrientationDimension::resetTargetAngleParameters() {
	targetAngle = UNINITIALIZED;
	targetAngleApproachManner = UNINITIALIZED;
	targetAngleState = TargetAngleState::Disabled;
}

int OrientationDimension::getTargetAngle() const{
	return targetAngle;
}
#pragma endregion



#pragma region Axis
////////////////////////////////////////////////////////////
/// Axis
////////////////////////////////////////////////////////////
Axis::Axis(OrientationDimension* orientation, glRotationFunction rotationFunction):
	orientation(orientation),
	rotate(rotationFunction)
{}
Axis::~Axis() {
	delete orientation;
}



void Axis::adjustGLModelMatrixAccordingly() const {
	rotate(orientation->angle.incrementationStepCoeff * orientation->angle.getValue()); 
}
void Axis::adjustGLModelMatrixInversely() const {
	rotate(orientation->angle.decrementationStepCoeff * orientation->angle.getValue());
}



void Axis::adjustGLModelMatrixTargetAngleAccordingly() const {
	rotate(orientation->angle.incrementationStepCoeff * orientation->getTargetAngle());
}



YawAxis::YawAxis(AngleState&& angle, VelocityState&& velocity, glRotationFunction rotationFunction):
	Axis::Axis(new UnlimitedOrientationDimension(std::move(angle), std::move(velocity)), rotationFunction)
{}
TiltAxis::TiltAxis(AngleState&& angle, VelocityState&& velocity, glRotationFunction rotationFunction) :
	Axis::Axis(new LimitedOrientationDimension(std::move(angle), std::move(velocity)), rotationFunction)
{}
#pragma endregion



#pragma region Robot
////////////////////////////////////////////////////////////
/// Robot
////////////////////////////////////////////////////////////
std::vector<Vector2> discrete2DCircleRadiusPoints(float radius, int nPoints) {
	std::vector<Vector2> circlePoints;

	for (double t = 0; t < 2 * M_PI; t += 2 * M_PI / nPoints)
		circlePoints.push_back(Vector2(cos(t) * radius, sin(t) * radius));

	return circlePoints;
}



const Color Robot::BASE_COLOR = Color(230, 80, 21);
const std::vector<Vector2> Robot::SCREW_CIRCLE_POSITIONS = discrete2DCircleRadiusPoints(0.25, 12);



Robot::Robot():
	axes({
		new YawAxis(AngleState(0, Extrema(0, 360), 'd', 'a', true), VelocityState(3, '1'), &Z::rotate),
		new TiltAxis(AngleState(22.5, Extrema(-45, 60), 'w', 's'), VelocityState(2, '2'), &Y::rotate),
		new TiltAxis(AngleState(-20, Extrema(-45, 75), 't', 'g'), VelocityState(2, '3'), &Y::rotate),
		new YawAxis(AngleState(0, Extrema(0, 360), 'f', 'h'), VelocityState(3, '4'), &X::rotate)
	}),
	axis2DrawFunction({
		{axes[0], std::bind(&Robot::drawFirstAxis, this)},
		{axes[1], std::bind(&Robot::drawSecondAxis, this)},
		{axes[2], std::bind(&Robot::drawThirdAxis, this)},
		{axes[3], std::bind(&Robot::drawFourthAxis, this)},
	}),
	drawTCPCoordSystem_b(false),
	drawTCPCoordSystemPrevious_b(false),
	displayAxesStates_b(true),
	approachArbitraryAxisConfiguration_b(false),
	approachArbitraryAxisConfigurationInfinitely_b(false)
{}



Robot::~Robot() {
	for (size_t i = 0; i < N_AXES; i++)
		delete axes[i];
}



////////////////////////////////////////////////////////////
/// .Publics
////////////////////////////////////////////////////////////
void Robot::update() {
	// update axes states
	for (Axis* axisPointer : axes)
		axisPointer->orientation->update();

	// check whether all axes have reached target angles
	if (approachArbitraryAxisConfiguration_b) {
		for (Axis* axisPointer : axes) {
			if (axisPointer->orientation->getTargetAngleState() == OrientationDimension::TargetAngleState::YetToBeReached)
				return;
		}
		
		// reset target angle parameters if so and infinite approach mode false
		if (!approachArbitraryAxisConfigurationInfinitely_b) {
			approachArbitraryAxisConfiguration_b = false;
			drawTCPCoordSystem_b = drawTCPCoordSystemPrevious_b;
			for (Axis* axisPointer : axes)
				axisPointer->orientation->resetTargetAngleParameters();
		}
		// otherwise initialize new approach cycle
		else
			initializeArbitraryAxisConfigurationApproach();
	}
}



void Robot::reset() {
	for (Axis* axisPointer : axes)
		axisPointer->orientation->reset();
}



void Robot::setArbitraryAxesConfiguration() {
	for (Axis* axisPointer : axes)
		axisPointer->orientation->angle.setArbitrarily();
}



void Robot::initializeArbitraryAxisConfigurationApproach() {
	approachArbitraryAxisConfiguration_b = true;
	
	if (!approachArbitraryAxisConfigurationInfinitely_b)
		drawTCPCoordSystemPrevious_b = drawTCPCoordSystem_b;
	drawTCPCoordSystem_b = true;

	for (Axis* axisPointer : axes)
		axisPointer->orientation->setTargetAngleParameters();
}



////////////////////////////////////////////////////////////
/// ..Toggling
////////////////////////////////////////////////////////////
void Robot::toggleDrawTCPCoordSystem() {
	if (!approachArbitraryAxisConfiguration_b)
		drawTCPCoordSystem_b = !drawTCPCoordSystem_b;
}



void Robot::toggleDisplayAxesStates() {
	displayAxesStates_b = !displayAxesStates_b;
}



void Robot::toggleInfiniteArbitraryAxisConfigurationApproachMode() {
	approachArbitraryAxisConfigurationInfinitely_b = !approachArbitraryAxisConfigurationInfinitely_b;

	if (approachArbitraryAxisConfigurationInfinitely_b) {
		drawTCPCoordSystemPrevious_b = drawTCPCoordSystem_b;
		initializeArbitraryAxisConfigurationApproach();
	}
}



////////////////////////////////////////////////////////////
/// .Text
////////////////////////////////////////////////////////////
bool Robot::textToBeDisplayed() const {
	return approachArbitraryAxisConfigurationInfinitely_b || displayAxesStates_b;
}



void Robot::displayText() const {
	if (displayAxesStates_b)
		displayAxesStates();
	if (approachArbitraryAxisConfigurationInfinitely_b)
		displayInfiniteAutomaticConfigurationApproachModeText();
}



void Robot::displayAxesStates() const {
	const int MAX_ANGLE_STATE_DIGITS = 7;

	for (size_t i = 0; i < N_AXES; i++) {
		std::ostringstream oss;

		// add angle state
		oss << roundf(axes[i]->orientation->angle.getValue() * 100.) / 100. << 'o';  // '°' not renderable by bitmap font
		if (axes[i]->orientation->angle.limitReached())
			oss << "!";

		// pad to uniform length
		oss << std::string(MAX_ANGLE_STATE_DIGITS - oss.str().length(), ' ');

		// add delimiter
		oss << " ";

		// add velocity
		oss << axes[i]->orientation->velocity.getValue() << "m/s";
		if (axes[i]->orientation->velocity.limitReached())
			oss << "!";

		Text::displayColored(Vector2(0.75, 0.8 - (i * 0.05)), oss.str().c_str(), Color(0.8, 0.8, 0.8), GLUT_BITMAP_9_BY_15);
	}
}



void Robot::displayInfiniteAutomaticConfigurationApproachModeText() const {
	Text::displayColored(Vector2(-0.9, 0.85), "Infinite Random Configuration Approach Mode", Color(214, 15, 38), GLUT_BITMAP_9_BY_15);
}



////////////////////////////////////////////////////////////
/// .Objects
////////////////////////////////////////////////////////////
cg_object3D Robot::objects[Robot::N_OBJECTS] = {};
void Robot::loadObjects() {
	const static char* DIR_PATH = getResourceSubDirPath("objects");
	const static char* FILE_NAMES[Robot::N_OBJECTS] = {
		"rotation_axis_1.obj",
		"screw_head.obj",
		"tilt_axis_1.obj",
		"tilt_axis_2.obj",
		"kuka_logo.obj"
	};

	for (size_t i = 0; i < Robot::N_OBJECTS; i++) {
		Robot::objects[i].load(joinPath(DIR_PATH, FILE_NAMES[i]), false);
	}
}

 

void Robot::setObjectMaterials() {
	GLfloat spec = 200;  // 0 -> saturated color
	GLfloat shine = 30;
	GLfloat emis = 1; // 0 -> matt, 1->bright

	objects[Object::YawAxis1].setMaterial(Color(BASE_COLOR), spec, shine, emis);
	objects[Object::ScrewHead].setMaterial(Color(Colors::GREY), 0.2, 0.5, 0);
	objects[Object::TiltAxis1].setMaterial(Color(BASE_COLOR), spec, shine, emis);
	objects[Object::TiltAxis2].setMaterial(Color(BASE_COLOR), spec, shine, emis);
	objects[Object::KukaLogo].setMaterial(Color(Colors::BLACK), spec, shine, emis);
}



////////////////////////////////////////////////////////////
/// .Textures
////////////////////////////////////////////////////////////
cg_image Robot::textures[Robot::N_TEXTURES] = {};
void Robot::loadTextures() {
	const static char* DIR_PATH = getResourceSubDirPath("textures");
	const static char* FILE_NAMES[Robot::N_TEXTURES] = {
		"smoothed-square-textured-metal.bmp",
		"warped-sheet-metal_roughness.bmp"
	};

	for (size_t i = 0; i < Robot::N_TEXTURES; i++) {
		textures[i].load(joinPath(DIR_PATH, FILE_NAMES[i]), true);

		textures[i].setMinFilter(GL_NEAREST);
		textures[i].setMagFilter(GL_LINEAR);
		textures[i].setWrapMode(GL_CLAMP);
		textures[i].setEnvMode(GL_MODULATE);
	}
}



////////////////////////////////////////////////////////////
/// .Drawing
////////////////////////////////////////////////////////////
void Robot::draw() const {
	// draw target axis configuration coord system if applicable
	if (approachArbitraryAxisConfiguration_b)
		drawTargetAxesConfigurationCoordSystem();

	// draw robot
	glPushMatrix();
		
		// base
		drawPedestal();
		drawBottom();

		// axes
		for (size_t i = 0; i < N_AXES; i++) {
			relativeAxesStartPositionShiftVectors[i].glTranslate();
			axes[i]->adjustGLModelMatrixAccordingly();
			axis2DrawFunction.at(axes[i])();
		}

		// tcp coord system if applicable
		if (drawTCPCoordSystem_b) {
			relativeAxesStartPositionShiftVectors[4].glTranslate();
			drawShrunkCoordSystem();
		}
	glPopMatrix();
}



void Robot::drawShrunkCoordSystem() {
	static Extrema TCP_COORD_SYSTEM_EXTREMA(-1, 1);

	drawCoordSystem(TCP_COORD_SYSTEM_EXTREMA, TCP_COORD_SYSTEM_EXTREMA, TCP_COORD_SYSTEM_EXTREMA, 0.3);
}



void Robot::drawTargetAxesConfigurationCoordSystem() const {
	glPushMatrix();

		for (size_t i = 0; i < N_AXES + 1; i++) {
			relativeAxesStartPositionShiftVectors[i].glTranslate();
			
			if (i < N_AXES)
				axes[i]->adjustGLModelMatrixTargetAngleAccordingly();
		}

		drawShrunkCoordSystem();
	glPopMatrix();
}



#pragma region Parts
////////////////////////////////////////////////////////////
/// ..Repeatedly Used Parts
////////////////////////////////////////////////////////////
void Robot::drawAxisWeight() const {
	const static float PEDASTEL_HEIGHT = 0.1;
	const static float BLOCK_HEIGHT = 1;
	const static float UPPER_CYLINDER_HEIGTH = 0.05;

	glPushMatrix();
		setDefaultLightAndMaterial(GlobalState::lightMode);

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
	for (size_t i = 0; i < SCREW_CIRCLE_POSITIONS.size(); i++) {
		glPushMatrix();
			glTranslatef(SCREW_CIRCLE_POSITIONS[i].x, 0, SCREW_CIRCLE_POSITIONS[i].y);
			glScaleUniformly(0.2);
			objects[ScrewHead].draw();
		glPopMatrix();
	}
}
#pragma endregion



#pragma region Components
////////////////////////////////////////////////////////////
/// ..Components
////////////////////////////////////////////////////////////
void Robot::drawPedestal() const {
	glPushMatrix();
		Z::translate(PEDASTEL_HEIGHT / 2);

		glScalef(8, PEDASTEL_HEIGHT, 8);
			
		Color(.6, .6, .6).render();
		textures[Texture::Knobs].bind();
		
		glEnable(GL_TEXTURE_2D);
			drawCube();
		glDisable(GL_TEXTURE_2D);

	glPopMatrix();
}



void Robot::drawBottom() const {
	static float LOWER_SEGMENT_HEIGHT = LOWER_STEEL_CYLINDER_HEIGHT * 0.2;
	static float CENTRAL_SEGMENT_HEIGHT = LOWER_STEEL_CYLINDER_HEIGHT * 0.7;
	static float UPPER_SEGMENT_HEIGHT = LOWER_STEEL_CYLINDER_HEIGHT * 0.1;
	
	glPushMatrix();
		Z::translate(PEDASTEL_HEIGHT);

		textures[Texture::Steel].bind();
		glEnable(GL_TEXTURE_2D);

		// lower segment
		BASE_COLOR.render();
		drawCylinder(1.4, 1, LOWER_SEGMENT_HEIGHT);

		// central segment
		Z::translate(LOWER_SEGMENT_HEIGHT);
		setColor(.4, .4, .4);
		drawCylinder(1, 1, CENTRAL_SEGMENT_HEIGHT);

		// upper segment
		Z::translate(CENTRAL_SEGMENT_HEIGHT);
		BASE_COLOR.render();
		drawCylinder(1.3, 1.3, UPPER_SEGMENT_HEIGHT);

		glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}
#pragma endregion



#pragma region Axes
////////////////////////////////////////////////////////////
/// ...TransformationAxes
////////////////////////////////////////////////////////////
const Vector3 Robot::relativeAxesStartPositionShiftVectors[Robot::N_AXES + 1] = {
	Vector3(0, 4.6, 0),
	Vector3(1.65, 1.63, 0.5),
	Vector3(0, 4.4, 0),
	Vector3(3.7635, 0, -0.25),
	Vector3(1.03, 0, 0),
};



void Robot::drawFirstAxis() const {
	static const float AXIS_MOUNT_DISK_HEIGHT = 0.3;

	// hollow cylinder with second axis mount
	glPushMatrix();
		glTranslatef(0, 0, 0.07);
		glScaleUniformly(2);
			
		textures[Texture::Steel].bind();
		glEnable(GL_TEXTURE_2D);
			objects[YawAxis1].draw();
		glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	// draw axis weigth
	glPushMatrix();
		Z::translate(0.35);  // height of cylinder bottom disk
		glScaleUniformly(1.3);
		drawAxisWeight();
	glPopMatrix();

	glPushMatrix();
		// draw second axis mount disk
			BASE_COLOR.render();
		glTranslatef(1.65, 1.63, 0.2);
		X::rotate(90);
		
		textures[Texture::Steel].bind();
		glEnable(GL_TEXTURE_2D);
			drawCylinder(0.5, 0.5, AXIS_MOUNT_DISK_HEIGHT);
		glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}



void Robot::drawSecondAxis()const {
	static const float LENGTH = 4.4;

	glPushMatrix();
		X::rotate(90);

		// draw axis with objects residing upon it 
		glPushMatrix();
			glTranslatef(0, 0.15, 0);
			
			glPushMatrix();
				glTranslatef(0, 0, -LENGTH / 2);
				
				// draw axis with logo
				glPushMatrix();
					glScaleUniformly(1.7);
				
					textures[Texture::Steel].bind();
					glEnable(GL_TEXTURE_2D);
						objects[TiltAxis1].draw();
					glDisable(GL_TEXTURE_2D);

					objects[KukaLogo].draw();
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
}



void Robot::drawThirdAxis() const {
	static const float LENGTH = 3.9;
	static const float WIDTH = 0.5;
	static const float MOUNT_PART_HEIGHT = 1.7;

	// draw axis
	glPushMatrix();
		Z::rotate(-90);
		glTranslatef(-WIDTH * 0.5, 0, -LENGTH * 0.41);  // translate slightly to the right, up, forward 
		
		glPushMatrix();
			glScaleUniformly(5);
				textures[Texture::Steel].bind();
			glEnable(GL_TEXTURE_2D);
				objects[TiltAxis2].draw();
			glDisable(GL_TEXTURE_2D);
		glPopMatrix();

		// Z::translate(WIDTH * 0.1);
		glScaleUniformly(1.4);
		Z::rotate(180);
		Y::rotate(90);
		objects[KukaLogo].draw();
	glPopMatrix();

	// draw weight block at axis beginning along respective x-axes
	glPushMatrix();
		glTranslatef(-0.6, 0, -WIDTH / 2);
		X::rotate(90);

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
		glTranslatef(0.1, -0.04, -WIDTH);
		glScaleUniformly(0.8);
		X::rotate(-90);
		Z::rotate(45);

		this->drawAxisWeight();
	glPopMatrix();
}



void Robot::drawFourthAxis() const {
	glPushMatrix();
		Y::rotate(-90);

		// partial cone
		drawCylinder(0.2, 0.3, 0.15);

		// short disk
		glTranslatef(0, 0.15, 0);
		drawCylinder(0.3, 0.3, 0.08);

		// long disk
		glTranslatef(0, 0.08, 0);
		drawCylinder(0.27, 0.27, 0.3);

		glTranslatef(0, 0.3, 0);

		// screw screw circle
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

		// translate to top of cone
		glTranslatef(0, 0.25, 0);
	glPopMatrix();
}
#pragma endregion



////////////////////////////////////////////////////////////
/// .Camera
////////////////////////////////////////////////////////////
void Robot::attachCameraToTCP() const {
	Z::rotate(90);
	approachSpatialTCPConfigurationInversely();
}



void Robot::attachCameraToTCPReversely() const {
	Z::rotate(-90);
	X::translate(-1);

	approachSpatialTCPConfigurationInversely();
}



void Robot::approachSpatialTCPConfigurationInversely() const {
	relativeAxesStartPositionShiftVectors[N_AXES].inverted().glTranslate();

	for (int i = N_AXES - 1; i >= 0; i--) {
		axes[i]->adjustGLModelMatrixInversely();
		relativeAxesStartPositionShiftVectors[i].inverted().glTranslate();
	}
}
#pragma endregion
#pragma endregion