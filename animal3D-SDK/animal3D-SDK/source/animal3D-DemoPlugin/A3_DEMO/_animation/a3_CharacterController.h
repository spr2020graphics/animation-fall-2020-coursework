/*
	Animation Framework Addons by Scott Dagen and Cameron Schneider
	a3_CharacterController.h: Handles input and translates that into animation. Has a pseudo-blend tree that will be swapped out later.
*/

#ifndef __ANIMAL3D_CHARACTER_CONTROLLER_H
#define __ANIMAL3D_CHARACTER_CONTROLLER_H


#include "a3_KeyframeAnimationController.h"
#include "../_animation/a3_HierarchyBlendNode.h"

#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
typedef struct a3_CharacterController		a3_CharacterController;
#endif	// __cplusplus

#define JUMP_DURATION 41.0f/30.0f;
//-----------------------------------------------------------------------------

struct a3_CharacterController
{
	a3_HierarchyBlendTree blendTree[1];
	// reference to the set of animation controllers for this character (0 = idle, 1 = walk, 2 = run, 3 = jump)
	a3_ClipController* animControllers;

	// pointer to the active animation controller, used to blend between "any" state and jump
	a3_ClipController* activeAnimController;

	// a reference to the set of poses for this character
	a3_HierarchyPoseGroup* poseGroup;

	// reference to the actual scene object this is controlling
	a3_DemoSceneObject* object;

	// similar to the former "branchTrigger" from lab 4, this is the input container for the jump input value, and will also trigger a transition into the jump animation
	a3f32* jumpTrigger;
	a3boolean isJumping;
	a3f32 jumpRemaining; //between 41/30 and 0
	a3f32 jumpDuration;

	// this is an input threshold to trigger the walk cycle
	a3f32 maxWalkVelocity;

	// the character's current velocity
	a3f32 currentVelocity;

	a3f32 normalizedVelocity;

	// used to transition into a jump
	a3f32 jumpTransitionVal;

	// so we can easily access these clips in the controller
	a3ui32 walkClipIndex, runClipIndex, jumpClipIndex, idleClipIndex;
};

// init a character controller
a3ui32 a3characterControllerInit(a3_CharacterController* controller_out, a3_ClipController* controller, a3_DemoSceneObject* obj, a3_HierarchyPoseGroup* poses, a3f32* jump, a3f32 walkThreshold);

// apply position and rotation input to an object
a3ui32 a3characterControllerApplyInput(a3_CharacterController* controller, a3vec2* position, a3real rotation, a3real dt);

a3ui32 a3characterControllerUpdate(a3_CharacterController* controller, a3_HierarchyPose* output, a3real dt);

// play jump anim + modify vertical position
a3ui32 a3characterControllerJump(a3_CharacterController* controller, a3_HierarchyPose* output, a3real dt);
// version of controllerJump that blends with a given input.
a3ui32 a3characterControllerBlendJump(a3_CharacterController* controller, a3_HierarchyPose* output, a3_HierarchyPose* blendInput, a3real dt);

// utility to set the isJumping field of a character
void a3characterToggleIsJumping(a3_CharacterController* controller);

a3ui32 a3characterControllerWalk(a3_CharacterController* controller, a3_HierarchyPose* output);

// see https://www.desmos.com/calculator/kcge9sqa4u
float getWalkScale(float u);
// see https://www.desmos.com/calculator/kcge9sqa4u
float getRunScale(float u);


//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#include "_inl/a3_CharacterController.inl"


#endif	// !__ANIMAL3D_CHARACTER_CONTROLLER_H