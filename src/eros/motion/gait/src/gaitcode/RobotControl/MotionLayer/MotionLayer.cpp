#include "MotionLayer.h"
#include <globals.h>

// The motion layer generates joint targets.
// Update() it with every tick of the robot control and call actuate() to have the joint targets generated.
// The way it works internally is that the motion layer has several motions that can control the robot.
// The motions execute their joint level and abstract functions to generate motion targets at joint level or
// abstract level. In order to free the motion layer and the individual motions from having access to the
// robot model, they are allowed to generate abstract and joint level targets in parallel, which are then
// _added_ to the final joint level targets. Having access to the robot model is undesired because it can
// change during runtime when the robot changes and then all components having access to it have to be updated
// too.

namespace gait
{
	MotionLayer::MotionLayer()
	{
		walking = false;
		robotDown = false;
	}

	void MotionLayer::init()
	{
		fall.init();
		halt.init();
		gait.init();
		motionInterfaceTest.init();
		motionTest.init();
	}

	void MotionLayer::update()
	{
		// Activate walking if we are halted and walk was commanded.
		if (!walking && command.walk)
			walking = true;

		// If we are walking and halt is commanded, we have to wait for the right gait phase.
		if (walking && !command.walk && gait.effectiveGCV.norm() < config.stoppingVelocity &&
			((gait.gaitPhase >= 0 - 0.05 && gait.gaitPhase < 0 + 0.05) || (gait.gaitPhase >= PI - 0.05) || (gait.gaitPhase <= -PI + 0.05)))
		{
			walking = false;
			gait.reset();
		}

		// Fall detection.
	//	if (not robotDown and state.fusedAngle.norm() > config.fallProtectionRelaxAngle*PI/180.0)
	//	{
	//		robotDown = true;
	//		emit debugOut("FALLING");
	//	}

		// Update internal states.
		halt.update();
		if (walking)
			gait.update();

		motionInterfaceTest.update();
		motionTest.update();
	}

	Action MotionLayer::actuate()
	{
		Action action;

		// Fall override.
		if (robotDown)
			return fall.actuate();

		// The joint test action allows to move the robot using sliders on the gui.
		if (command.motionInterfaceTest)
			return motionInterfaceTest.actuate();

		if (command.motionTest)
			return motionTest.actuate();

		// Walking.
		if (walking)
			return gait.actuate();

		// The default action is halt. It generates the "center" halt position.
		return halt.actuate();
	}

}