

#ifndef __ANIMAL3D_CHARACTER_CONTROLLER_H
#define __ANIMAL3D_CHARACTER_CONTROLLER_H


#include "a3_KeyframeAnimationController.h"


#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
typedef struct a3_CharacterController		a3_CharacterController;
#endif	// __cplusplus


//-----------------------------------------------------------------------------

struct a3_CharacterController
{
	// reference to the animation controller for this character
	a3_ClipController* animController;

	// similar to the former "branchTrigger" from lab 4, this is the input container for the jump input value, and will also trigger a transition into the jump animation
	a3f32 jumpTrigger;

	// this is an input threshold to trigger the walk cycle (I think we'll use tri-lerp and some fancy normalization logic to get the weights)
	a3f32 maxWalkThreshold;

	// this is an input threshold to trigger the run cycle (greater than the walkThreshold)
	a3f32 maxRunThreshold;
};

// init a character controller
a3ui32 a3characterControllerInit(a3_CharacterController* controller_out, a3_ClipController* controller, a3f32 jump, a3f32 walkThreshold, a3f32 runThreshold);

// apply position and rotation input to an object
a3ui32 a3characterControllerApplyInput(a3_DemoSceneObject* object, a3vec2* position, a3real rotation);

//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#include "_inl/a3_CharacterController.inl"


#endif	// !__ANIMAL3D_CHARACTER_CONTROLLER_H