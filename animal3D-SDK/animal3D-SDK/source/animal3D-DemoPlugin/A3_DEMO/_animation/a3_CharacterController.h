

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

	// reference to the actual scene object this is controlling
	a3_DemoSceneObject* object;

	// similar to the former "branchTrigger" from lab 4, this is the input container for the jump input value, and will also trigger a transition into the jump animation
	a3f32 jumpTrigger;
	a3boolean isJumping;

	// this is an input threshold to trigger the walk cycle
	a3f32 maxWalkThreshold;

	// this is an input threshold to trigger the run cycle (greater than the walkThreshold)
	a3f32 maxRunThreshold;

	// so we can easily access these clips in the controller
	a3ui32 walkClipIndex, runClipIndex, jumpClipIndex, idleClipIndex;
};

// init a character controller
a3ui32 a3characterControllerInit(a3_CharacterController* controller_out, a3_ClipController* controller, a3_DemoSceneObject* obj, a3f32 jump, a3f32 walkThreshold, a3f32 runThreshold);

// apply position and rotation input to an object
a3ui32 a3characterControllerApplyInput(a3_CharacterController* controller, a3vec2* position, a3real rotation);

// play jump anim + modify vertical position
a3ui32 a3characterControllerJump(a3_CharacterController* controller, a3f32 blendVal);

// utility to set the isJumping field of a character
void a3characterToggleIsJumping();

a3ui32 a3characterControllerWalk(a3_CharacterController* controller, a3f32 blendVal);

a3ui32 a3characterControllerRun(a3_CharacterController* controller, a3f32 blendVal);

a3ui32 a3characterControllerIdle(a3_CharacterController* controller, a3f32 blendVal);

//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#include "_inl/a3_CharacterController.inl"


#endif	// !__ANIMAL3D_CHARACTER_CONTROLLER_H