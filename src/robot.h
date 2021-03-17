#pragma once

#include "wavefront.h"
#include "glutils.h"
#include "utils.h"
#include "image.h"

#include <map>
#include <vector>


#pragma region AxisParameterState
/*
*	Interface for structs encapsulating singular robot axis parameter modifyable by
*	user at runtime and the entirety of inherent attributes
*/
struct AxisParameterState: public ValueAbstraction<float> {
public:
	const float startValue;
	AxisParameterState(float startValue, Extrema&& limits);

	bool limitReached() const;
	void reset();

	void updateLimitReached();
	void clipValue();
protected:
	bool limitReached_b;
	
	const Extrema limits;
};


struct AngleState : public AxisParameterState {
	const char incrementationKey, decrementationKey;
	const int incrementationStepCoeff, decrementationStepCoeff;
	AngleState(float startValue, Extrema&& limits, char incrementationKey, char decrementationKey, bool invertedControl = false);

	/// Samples hardware randomized value within value limits
	int drawArbitraryValue() const;
	void setArbitrarily();
};


struct VelocityState: public AxisParameterState {
	VelocityState(float max, char identificationKey);
	const char identificationKey;
};
#pragma endregion



#pragma region Axis
struct Axis {
	AngleState angle;
	VelocityState velocity;

	Axis(AngleState&& angle, VelocityState&& velocity, glRotationFunction rotationFunction);
	virtual ~Axis();

	void update();
	void reset();

	void adjustGLModelMatrixAccordingly() const;
	void adjustGLModelMatrixInversely() const;
	void adjustGLModelMatrixTargetAngleAccordingly() const;

	struct TargetAngle : public ValueAbstraction<float> {
		enum State {
			Disabled,
			YetToBeReached,
			Reached
		};

		bool approachManner;  // 0=decrementally, 1=incrementally
		State state;
		TargetAngle();

		void reset();
		void updateState(float currentAngle);
	};

	TargetAngle targetAngle;
	void setTargetAngleParameters(bool targetHomePosition = false);
protected:
	void updateVelocity();
	void updateAngle();
	/// Adjusts angle state wrt specific Axis kind, i.e. subclass, post update
	virtual void adjustAngle() = 0;

	/// Determines targetAngleApproachManner granting the quickest attainment
	virtual void determineTargetAngleApproachManner() = 0;
	void approachTargetAngle();
private:
	const glRotationFunction rotate;
};


struct YawAxis : public Axis {
	using Axis::Axis;
private:
	void determineTargetAngleApproachManner();
	void adjustAngle();
};


struct TiltAxis : public Axis {
	using Axis::Axis;
private:
	void determineTargetAngleApproachManner();
	void adjustAngle();
};
#pragma endregion


std::vector<Vector2> discrete2DCircleRadiusPoints(float radius, int nPoints);


class Robot{
public:
	static void loadObjects();
	static void setObjectMaterials();

	static void loadTextures();

	Robot();
	~Robot();

	void draw() const;
	void update();
	void reset();
	void setArbitraryAxesConfiguration();

	void initializeHomePositionApproach();
	void initializeArbitraryAxisConfigurationApproach();

	/* ------------Toggling----------------- */

	void toggleTCPCoordSystem();
	void toggleAxesStatesDisplay();
	void toggleInfiniteArbitraryAxisConfigurationApproachMode();

	/* ------------Camera----------------- */

	void attachCameraToTCP() const;
	void attachCameraToTCPReversely() const;

	/* -------------Text----------------- */

	bool textToBeDisplayed() const;
	void displayText() const;
private:
	static const Color BASE_COLOR;

	bool approachArbitraryAxisConfigurationInfinitely_b;
	bool approachArbitraryAxisConfiguration_b;
	bool approachHomePosition_b;

	bool drawTCPCoordSystem_b, drawTCPCoordSystemPrevious_b;
	bool displayAxesStates_b;

	/* -----------------TextDisplay----------------- */

	void displayAxesStates() const;
	void displayInfiniteAutomaticConfigurationApproachModeText() const;

	/* -----------------CoordSystem----------------- */

	static void drawShrunkCoordSystem();
	void drawTargetAxesConfigurationCoordSystem() const;

	/* ------------Textures----------------- */

	static const int N_TEXTURES = 2;
	static cg_image textures[N_TEXTURES];

	enum Texture {
		Knobs,
		Steel
	};

	/* ------------Objects----------------- */

	const static int N_OBJECTS = 5;
	static cg_object3D objects[N_OBJECTS];

	enum Object {
		YawAxis1,
		ScrewHead,
		TiltAxis1,
		TiltAxis2,
		KukaLogo
	};

	/* -------------RepeatedlyUsedParts------------------ */

	void drawAxisWeight() const;
	void drawScrewCircle() const;
	static const std::vector<Vector2> SCREW_CIRCLE_POSITIONS;

	/* ------------------Components------------------- */

	void drawPedestal() const;
	static const float PEDASTEL_HEIGHT;

	void drawBottom() const;
	static const float LOWER_STEEL_CYLINDER_HEIGHT;

		/* --------------AXES------------------ */

	static const int N_AXES = 4;
	const static Vector3 relativeAxesStartPositionShiftVectors[N_AXES + 1];
	
	Axis* axes[N_AXES];
	std::map<Axis*, std::function<const void()>> axis2DrawFunction;

	void drawFirstAxis() const;
	void drawSecondAxis() const;
	void drawThirdAxis() const;
	void drawFourthAxis() const;

	/* ------------------Camera------------------- */

	void approachSpatialTCPConfigurationInversely() const;
};