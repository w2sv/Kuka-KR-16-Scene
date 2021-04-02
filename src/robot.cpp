#include "robot.h"

#include "geometry.h"
#include "input.h"
#include "state.h"
#include "text.h"
#include "light.h"

#include "../dependencies/freeglut.h"

#include <limits>
#include <functional>
#include <sstream>
#include <string>
#define _USE_MATH_DEFINES
#include <math.h>


using namespace glTransformationAxes;


#pragma region AxisParameterState
////////////////////////////////////////////////////////////
/// AxisParameterState
////////////////////////////////////////////////////////////

AxisParameterState::AxisParameterState(float startValue, Extrema&& limits):
	ValueAbstraction<float>(startValue),
	startValue(startValue),
	limits(limits),
	limitReached_b(false)
{}


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
	value = limits.clippedValue(value);
}


////////////////////////////////////////////////////////////
/// AngleState
////////////////////////////////////////////////////////////

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


////////////////////////////////////////////////////////////
/// VelocityState
////////////////////////////////////////////////////////////

VelocityState::VelocityState(float max, char identificationKey):
	AxisParameterState::AxisParameterState(0.4, Extrema(0.1, max)),
	identificationKey(identificationKey)
{}
#pragma endregion



#pragma region Axis
////////////////////////////////////////////////////////////
/// Axis
////////////////////////////////////////////////////////////

Axis::Axis(AngleState&& angle, VelocityState&& velocity, glRotationFunction rotationFunction):
	angle(angle),
	velocity(velocity),
	rotate(rotationFunction)
{}


Axis::~Axis() 
{}


void Axis::update() {
	updateVelocity();
	updateAngle();
}


void Axis::updateVelocity() {
	static const float STEP = 0.01;

	if (cg_key::keyState(velocity.identificationKey) == 2) {
		if (cg_key::keyState('+') != 0)
			velocity += STEP;
		else if (cg_key::keyState('-') != 0)
			velocity -= STEP;
		else
			return;

		velocity.clipValue();
		velocity.updateLimitReached();
	}
}


void Axis::updateAngle() {
	switch (targetAngle.state) {
		case (TargetAngle::State::YetToBeReached):
			approachTargetAngle();
			adjustAngle();
			break;
		case (TargetAngle::State::Disabled):
			if (cg_key::keyState(angle.decrementationKey) != 0)
				angle += angle.incrementationStepCoeff * velocity;
			else if (cg_key::keyState(angle.incrementationKey) != 0)
				angle += angle.decrementationStepCoeff * velocity;
			else
				return;

		adjustAngle();
		angle.updateLimitReached();
	}
}


////////////////////////////////////////////////////////////
/// glModelMatrix Manipulation
////////////////////////////////////////////////////////////

void Axis::adjustGLModelMatrixAccordingly() const {
	rotate(angle.incrementationStepCoeff * angle); 
}


void Axis::adjustGLModelMatrixInversely() const {
	rotate(angle.decrementationStepCoeff * angle);
}


void Axis::adjustGLModelMatrixTargetAngleAccordingly() const {
	rotate(angle.incrementationStepCoeff * targetAngle);
}


////////////////////////////////////////////////////////////
/// TargetAngle
////////////////////////////////////////////////////////////

void Axis::setTargetAngleParameters(bool targetHomePosition) {
	targetAngle.value = targetHomePosition ? angle.startValue : angle.drawArbitraryValue();
	targetAngle.updateState(angle.value);

	if (targetAngle.state == TargetAngle::State::Reached)
		return;

	determineTargetAngleApproachManner();
}


void Axis::approachTargetAngle() {
	float angleDifferenceAbs = abs(targetAngle - angle);
	if (angleDifferenceAbs > 180)
		angleDifferenceAbs = abs(angleDifferenceAbs - 360);

	float step = std::min<float>(velocity.value, angleDifferenceAbs);
	targetAngle.approachManner ? angle += step: angle -= step;

	targetAngle.updateState(angle.value);
}


////////////////////////////////////////////////////////////
/// Axis::Target Angle
////////////////////////////////////////////////////////////

Axis::TargetAngle::TargetAngle():
	ValueAbstraction<float>(NULL)
{ 
	reset(); 
}


void Axis::TargetAngle::reset() {
	value = NULL;
	approachManner = NULL;
	state = State::Disabled;
}


void Axis::TargetAngle::updateState(float currentAngle) {
	state = currentAngle == value ? State::Reached : State::YetToBeReached;
}
#pragma endregion


////////////////////////////////////////////////////////////
/// YawAxis(Axis)
////////////////////////////////////////////////////////////

void YawAxis::adjustAngle() {
	if (angle >= 360)
		angle -= 360;
	else if (angle <= 0)
		angle += 360;
}


void YawAxis::determineTargetAngleApproachManner() {
	float decrementalDifference = targetAngle > angle ? angle + 360 - targetAngle : angle - targetAngle;
	float incrementalDifference = targetAngle < angle ? 360 - angle + targetAngle : targetAngle - angle;

	targetAngle.approachManner = incrementalDifference < decrementalDifference;
}


bool YawAxis::atHomePosition() const {
	return angle == 0 || angle == 360;
}


////////////////////////////////////////////////////////////
/// TiltAxis(Axis)
////////////////////////////////////////////////////////////

void TiltAxis::adjustAngle() {
	angle.clipValue();
}


void TiltAxis::determineTargetAngleApproachManner() {
	targetAngle.approachManner = targetAngle > angle;
}


bool TiltAxis::atHomePosition() const {
	return angle == angle.startValue;
}



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


const Color Robot::BASE_COLOR = Color::fromUnnormalizedValues(230, 80, 21);
const std::vector<Vector2> Robot::SCREW_CIRCLE_POSITIONS = discrete2DCircleRadiusPoints(0.25, 12);


Robot::Robot():
	axes{
		new YawAxis(AngleState(0, Extrema(0, 360), 'd', 'a', true), VelocityState(3, '1'), &Z::rotate),
		new TiltAxis(AngleState(22.5, Extrema(-45, 60), 'w', 's'), VelocityState(2, '2'), &Y::rotate),
		new TiltAxis(AngleState(-20, Extrema(-45, 75), 't', 'g'), VelocityState(2, '3'), &Y::rotate),
		new YawAxis(AngleState(0, Extrema(0, 360), 'f', 'h'), VelocityState(3, '4'), &X::rotate)
	},
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
	approachArbitraryAxisConfigurationInfinitely_b(false),
	approachHomePosition_b(false),
	lastApproachCycle_b(false),
	drawTCPSpotlight_b(false)
{
	resetDisplayTimeLimit(athomepositionDisplayTimeLimit);
	resetDisplayTimeLimit(velocitiesatdefaultDisplayTimeLimit);
	resetDisplayTimeLimit(velocitiesresetDisplayTimeLimit);
}


Robot::~Robot() {
	for (size_t i = 0; i < N_AXES; i++)
		delete axes[i];
}


////////////////////////////////////////////////////////////
/// .Actions
////////////////////////////////////////////////////////////

void Robot::resetVelocities() {
	if (velocitiesAtDefault()) {
		setDisplayTimeLimit(velocitiesatdefaultDisplayTimeLimit);
		return;
	}

	for (Axis* axisPointer : axes)
		axisPointer->velocity.reset();
	setDisplayTimeLimit(velocitiesresetDisplayTimeLimit);
}


void Robot::setArbitraryAxesConfiguration() {
	for (Axis* axisPointer : axes)
		axisPointer->angle.setArbitrarily();
}


void Robot::approachArbitraryAxisConfiguration() {
	approachArbitraryAxisConfiguration_b = true;

	if (!approachArbitraryAxisConfigurationInfinitely_b)
		drawTCPCoordSystemPrevious_b = drawTCPCoordSystem_b;
	drawTCPCoordSystem_b = true;

	for (Axis* axisPointer : axes)
		axisPointer->setTargetAngleParameters();
}


void Robot::assumeHomePosition(bool snap) {
	if (atHomePosition())
		setDisplayTimeLimit(athomepositionDisplayTimeLimit);

	else if (snap)
		for (Axis* axisPointer : axes)
			axisPointer->angle.reset();
	else {
		approachArbitraryAxisConfiguration_b = approachHomePosition_b = true;
		for (Axis* axisPointer : axes)
			axisPointer->setTargetAngleParameters(true);
	}
}


////////////////////////////////////////////////////////////
/// .Toggling
////////////////////////////////////////////////////////////

void Robot::toggleTCPCoordSystem() {
	if (!approachArbitraryAxisConfiguration_b || approachHomePosition_b)
		drawTCPCoordSystem_b = !drawTCPCoordSystem_b;
}


void Robot::toggleAxesStatesDisplay() {
	displayAxesStates_b = !displayAxesStates_b;
}


void Robot::toggleInfiniteArbitraryAxisConfigurationApproachMode() {
	approachArbitraryAxisConfigurationInfinitely_b = !approachArbitraryAxisConfigurationInfinitely_b;

	if (approachArbitraryAxisConfigurationInfinitely_b) {
		drawTCPCoordSystemPrevious_b = drawTCPCoordSystem_b;
		approachArbitraryAxisConfiguration();
	}
	else
		lastApproachCycle_b = true;
}


void Robot::toggleTCPSpotlight() {
	drawTCPSpotlight_b = !drawTCPSpotlight_b;
}


void Robot::update() {
	// update axes states
	for (Axis* axisPointer : axes)
		axisPointer->update();

	// check whether all axes have reached target angles
	if (approachArbitraryAxisConfiguration_b) {
		for (Axis* axisPointer : axes) {
			if (axisPointer->targetAngle.state == Axis::TargetAngle::State::YetToBeReached)
				return;
		}

		// reset target angle parameters if so and infinite approach == false
		if (!approachArbitraryAxisConfigurationInfinitely_b) {
			approachArbitraryAxisConfiguration_b = false;

			for (Axis* axisPointer : axes)
				axisPointer->targetAngle.reset();

			if (approachHomePosition_b)
				approachHomePosition_b = false;
			else
				drawTCPCoordSystem_b = drawTCPCoordSystemPrevious_b;

			if (lastApproachCycle_b)
				lastApproachCycle_b = false;
		}
		// otherwise initialize new approach cycle
		else
			approachArbitraryAxisConfiguration();
	}
}


////////////////////////////////////////////////////////////
/// .Text
////////////////////////////////////////////////////////////

void Robot::displayText() {
	if (displayAxesStates_b)
		displayAxesStates();

	displayStatus();
}


void Robot::displayAxesStates() const {
	const int MAX_ANGLE_STATE_DIGITS = 7;

	for (size_t i = 0; i < N_AXES; i++) {
		std::ostringstream oss;

		// add angle state
		oss << roundf(axes[i]->angle.value * 100.) / 100. << 'o';  // '°' not renderable by bitmap font
		if (axes[i]->angle.limitReached())
			oss << "!";

		// pad to uniform length
		oss << std::string(MAX_ANGLE_STATE_DIGITS - oss.str().length(), ' ');

		// add delimiter
		oss << " ";

		// add velocity
		oss << axes[i]->velocity.value << "m/s";
		if (axes[i]->velocity.limitReached())
			oss << "!";

		Text::displayColored(Vector2(0.75, 0.8 - (i * 0.05)), oss.str().c_str(), Color(0.8f), GLUT_BITMAP_9_BY_15);
	}
}


void Robot::displayStatus() {
	static const float MESSAGE_POSITIONS_X = -0.9;
	static const Vector2 MESSAGE_ROW_POSITIONS[2] = { Vector2(MESSAGE_POSITIONS_X, 0.85), Vector2(MESSAGE_POSITIONS_X, 0.8) };

	time_t now = currentSecondsTimestamp();

	// reset homePositionAlreadyReached stop time to prevent renewed display after display of different approach message
	if (approachHomePosition_b || approachArbitraryAxisConfiguration_b)
		resetDisplayTimeLimit(athomepositionDisplayTimeLimit);

	// display contravalent text to first message row if applicable
	if (approachArbitraryAxisConfigurationInfinitely_b)
		displayStatusMessage("Endless Random Configuration Approach Mode", MESSAGE_ROW_POSITIONS[0]);
	else if (approachHomePosition_b)
		displayStatusMessage("Approaching home position", MESSAGE_ROW_POSITIONS[0]);
	else if (approachArbitraryAxisConfiguration_b && !lastApproachCycle_b)
		displayStatusMessage("Approaching random configuration", MESSAGE_ROW_POSITIONS[0]);
	else if (athomepositionDisplayTimeLimit > now)
		displayStatusMessage("Robot already at home position", MESSAGE_ROW_POSITIONS[0]);

	// display velocities at default to second message row if applicable
	if (velocitiesresetDisplayTimeLimit > now)
		displayStatusMessage("Reset Velocities", MESSAGE_ROW_POSITIONS[1]);
	else if (velocitiesatdefaultDisplayTimeLimit > now)
		displayStatusMessage("Velocities at default", MESSAGE_ROW_POSITIONS[1]);
}


void Robot::displayStatusMessage(const char* message, const Vector2& screenPosition) const {
	static const Color color(Color::fromUnnormalizedValues(214, 15, 38));

	Text::displayColored(screenPosition, message, color, GLUT_BITMAP_9_BY_15);
}


////////////////////////////////////////////////////////////
/// ..Persistent Text
////////////////////////////////////////////////////////////

bool Robot::atHomePosition() {
	for (Axis* axisPointer : axes)
		if (!axisPointer->atHomePosition()) {
			resetDisplayTimeLimit(athomepositionDisplayTimeLimit);
			return false;
		}
	return true;
}


bool Robot::velocitiesAtDefault() {
	for (Axis* axisPointer : axes) {
		if (axisPointer->velocity != axisPointer->velocity.startValue) {
			resetDisplayTimeLimit(velocitiesatdefaultDisplayTimeLimit);
			return false;
		}
	}
	return true;
}


void Robot::setDisplayTimeLimit(time_t& timeLimit) {
	timeLimit = currentSecondsTimestamp() + 3;
}


void Robot::resetDisplayTimeLimit(time_t& timeLimit) {
	timeLimit = -1;
}


////////////////////////////////////////////////////////////
/// .Objects
////////////////////////////////////////////////////////////

cg_object3D Robot::objects[Robot::N_OBJECTS] = {};
void Robot::loadObjects() {
	const static char* DIR_PATH = getResourceSubDirPath("objects");
	const static char* FILE_NAMES[Robot::N_OBJECTS] = {
		"rotation_axis_1.obj",
		"tilt_axis_1.obj",
		"tilt_axis_2.obj",
		"screw_head.obj",
		"kuka_logo.obj"
	};

	for (size_t i = 0; i < Robot::N_OBJECTS; i++) {
		Robot::objects[i].load(joinPath(DIR_PATH, FILE_NAMES[i]), false);
	}
}
 

void Robot::setObjectMaterials() {
	GLfloat spec = 200;  // 0 -> saturated color
	GLfloat shine = 300;
	GLfloat emis = 1; // 0 -> matt, 1->bright

	objects[Object::YawAxis1].setMaterial(BASE_COLOR, spec, shine, emis);
	objects[Object::TiltAxis1].setMaterial(BASE_COLOR, spec, shine, emis);
	objects[Object::TiltAxis2].setMaterial(BASE_COLOR, spec, shine, emis);
	objects[Object::ScrewHead].setMaterial(COLORS::GREY, 0.2, 0.5, 0);
	objects[Object::KukaLogo].setMaterial(COLORS::BLACK, spec, 50, emis);
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

	const static GLenum parameters[Robot::N_TEXTURES][4] = {
		{GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_MODULATE},
		{GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_BLEND}
	};

	for (size_t i = 0; i < Robot::N_TEXTURES; i++) {
		textures[i].load(joinPath(DIR_PATH, FILE_NAMES[i]), true);

		textures[i].setMinFilter(parameters[i][0]);
		textures[i].setMagFilter(parameters[i][1]);
		textures[i].setWrapMode(parameters[i][2]);
		textures[i].setEnvMode(parameters[i][3]);
	}
}


////////////////////////////////////////////////////////////
/// .Drawing
////////////////////////////////////////////////////////////

void Robot::draw() const {
	// draw target axis configuration coord system if applicable
	if (approachArbitraryAxisConfiguration_b && !approachHomePosition_b)
		drawTargetAxesConfigurationCoordSystem();

	// draw robot
	glPushMatrix();
		
		// base
		drawPedestal();
		drawBottom();

		// axes
		for (size_t i = 0; i < N_AXES; i++) {
			glTranslateByVec(relativeAxesStartPositionShiftVectors[i]);
			axes[i]->adjustGLModelMatrixAccordingly();
			axis2DrawFunction.at(axes[i])();
		}

		// tcp coord system if applicable
		if (drawTCPCoordSystem_b || drawTCPSpotlight_b) {
			glTranslateByVec(relativeAxesStartPositionShiftVectors[N_AXES]);

			if (drawTCPCoordSystem_b)
				drawShrunkCoordSystem();
			if (drawTCPSpotlight_b)
				drawTCPSpotlight();
		}

		glTranslateByVec(relativeAxesStartPositionShiftVectors[N_AXES]);

	glPopMatrix();
}


void Robot::drawTCPSpotlight() const {
	static cg_light spot3(1);

	spot3.setPosition(0, 0, 0, 1);
	spot3.setSpotlight(1.f, 0.f, 0.0f, 25.0f, 2.f);

	spot3.setAmbient(1, 1, 1, 1.0f);
	spot3.setDiffuse(1.0f, 1.0f, 0.1f, 1.0f);
	spot3.setSpecular(1.0f, 1.0f, 0.1f, 1.0f);

	spot3.draw();
}


void Robot::drawShrunkCoordSystem() {
	static const Extrema TCP_COORD_SYSTEM_EXTREMA(-1, 1);

	drawCoordSystem(TCP_COORD_SYSTEM_EXTREMA, TCP_COORD_SYSTEM_EXTREMA, TCP_COORD_SYSTEM_EXTREMA, 0.3);
}


void Robot::drawTargetAxesConfigurationCoordSystem() const {
	glPushMatrix();

		for (size_t i = 0; i < N_AXES; i++) {
			glTranslateByVec(relativeAxesStartPositionShiftVectors[i]);
			axes[i]->adjustGLModelMatrixTargetAngleAccordingly();
		}
		glTranslateByVec(relativeAxesStartPositionShiftVectors[N_AXES]);

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
		static const Material MATERIAL(
			RGBAParameter{ 0.4, 0.4, 0.4, 1.0 },
			RGBAParameter{ 0.4, 0.4, 0.4, 1.0 },
			RGBAParameter{ 1, 1, 1, 1 },
			32.f,
			RGBAParameter{ 0, 0, 0, 1 }
		);
		MATERIAL.set();

		// draw lower octPrism pedastel
		Z::translate(PEDASTEL_HEIGHT / 2);
		static const OctogonalPrism pedastel(PEDASTEL_HEIGHT, 0.5, 0.2);
			pedastel.draw(COLORS::BLACK, &COLORS::GREY);

		// draw octPrism block
		Z::translate(BLOCK_HEIGHT / 2);
		static const OctogonalPrism block(BLOCK_HEIGHT, 0.3, 0.3);
			block.draw(COLORS::BLACK, &COLORS::GREY);

		// draw upper black cylinder
		COLORS::BLACK.render();
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

		Color(.45f).render();

		textures[Texture::Knobs].bind();
		glEnable(GL_TEXTURE_2D);
			Cube::draw();
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
			Color(.4f).render();
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
/// ...Axes
////////////////////////////////////////////////////////////

const Vector3 Robot::relativeAxesStartPositionShiftVectors[Robot::N_AXES + 1] = {
	Vector3(0, Robot::PEDASTEL_HEIGHT + Robot::LOWER_STEEL_CYLINDER_HEIGHT, 0),
	Vector3(1.65, 1.63, 0.5),
	Vector3(0, 4.4, 0),
	Vector3(3.7635, 0, -0.25),
	Vector3(1.03, 0, 0),
};
const float Robot::PEDASTEL_HEIGHT = 3.f;
const float Robot::LOWER_STEEL_CYLINDER_HEIGHT = 1.6f;


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
		glTranslatef(1.65, 1.63, 0.2);
		X::rotate(90);
		
		BASE_COLOR.render();
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
						drawScrewCircle();
				glPopMatrix();
			glPopMatrix();

				// draw lower screw circle
				drawScrewCircle();

		glPopMatrix();

		// draw orange axes weight pedastel octPrism
		glTranslatef(0, -0.5, 0);
		X::rotate(180);
			textures[Texture::Steel].bind();
			glEnable(GL_TEXTURE_2D);
				static const OctogonalPrism weightPedastel(0.15, 0.6, 0.1);
					weightPedastel.draw(BASE_COLOR, &COLORS::BLACK);
			glDisable(GL_TEXTURE_2D);

		// draw weight
		Z::translate(0.09);
			drawAxisWeight();
	glPopMatrix();
}


void Robot::drawThirdAxis() const {
	static const float LENGTH = 3.9;
	static const float WIDTH = 0.5;

	// draw axis
	glPushMatrix();
		Z::rotate(-90);
		glTranslatef(-WIDTH / 2, 0, -LENGTH * 0.41);  // translate slightly to the right, up, forward 
		
		glPushMatrix();
			glScaleUniformly(5);

			textures[Texture::Steel].bind();
			glEnable(GL_TEXTURE_2D);
				objects[TiltAxis2].draw();
			glDisable(GL_TEXTURE_2D);
		
			glPopMatrix();

		glTranslatef(-0.05, WIDTH * 0.1, 0);

		glScaleUniformly(1.4);
		Z::rotate(181.5);  // for some reason logo not entirely parallel to axis
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
					drawAxisWeight();
			glPopMatrix();
		}
	glPopMatrix();

	// draw singular weight along y-axis
	glPushMatrix();
		glTranslatef(0.1, -0.04, -WIDTH);
		glScaleUniformly(0.8);
		X::rotate(-90);
		Z::rotate(45);
			drawAxisWeight();
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
				drawScrewCircle();
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
	glTranslateByVec(relativeAxesStartPositionShiftVectors[N_AXES].inverted());

	for (int i = N_AXES - 1; i >= 0; i--) {
		axes[i]->adjustGLModelMatrixInversely();
		glTranslateByVec(relativeAxesStartPositionShiftVectors[i].inverted());
	}
}
#pragma endregion
#pragma endregion