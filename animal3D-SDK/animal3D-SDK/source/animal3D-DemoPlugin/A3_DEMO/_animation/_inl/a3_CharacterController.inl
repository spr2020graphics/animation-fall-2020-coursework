#ifdef __ANIMAL3D_CHARACTER_CONTROLLER_H
#ifndef __ANIMAL3D_CHARACTER_CONTROLLER_INL
#define __ANIMAL3D_CHARACTER_CONTROLLER_INL

#include <stdio.h>
#include "..\a3_CharacterController.h"

//-----------------------------------------------------------------------------


inline a3ui32 a3characterControllerInit(a3_CharacterController* controller_out, a3_ClipController* controllers, a3_DemoSceneObject* obj, a3_HierarchyPoseGroup* poses, a3f32* jump, a3f32 walkThreshold)
{
	controller_out->animControllers = controllers;
	controller_out->jumpTrigger = jump;
	controller_out->poseGroup = poses;
	controller_out->maxWalkVelocity = walkThreshold;
	controller_out->currentVelocity = 0.0f;
	controller_out->object = obj;
	controller_out->isJumping = false;

	controller_out->activeAnimController = &controller_out->animControllers[0];

	return 1;
}

inline a3ui32 a3characterControllerApplyInput(a3_CharacterController* controller, a3vec2* position, a3real rotation, a3real dt)
{
	a3vec3 tempPos;
	tempPos.x = position->x;
	tempPos.y = position->y;
	tempPos.z = 0.0f;

	a3vec3 tempVel;
	a3real3QuotientS(tempVel.v, a3real3Diff(tempVel.v, tempPos.v, controller->object->position.v), dt);

	controller->currentVelocity = a3real3Length(tempVel.v);

	controller->object->position.x = +(position->x);
	controller->object->position.y = +(position->y);

	controller->object->euler.z = -a3trigValid_sind(rotation);

	//printf("%f \n", controller->currentVelocity);

	return 1;
}

inline a3ui32 a3characterControllerUpdate(a3_CharacterController* controller, a3_HierarchyPose* output, a3real dt)
{
	if (controller)
	{
		a3real u = controller->currentVelocity / controller->maxWalkVelocity;

		if (u > 1.0f)
		{
			u = 1.0f;
		}
		float walkDT = dt / getWalkScale(u); //dt is scaled to 1/0.75 at u = 1
		float runDT = dt * getRunScale(u); //dt is scaled to 0.75 at u = 0

		a3clipControllerUpdate(&controller->animControllers[0], dt);
		a3clipControllerUpdate(&controller->animControllers[1], walkDT);
		a3clipControllerUpdate(&controller->animControllers[2], runDT);
		a3clipControllerUpdate(&controller->animControllers[3], dt);

		if (*controller->jumpTrigger == 1.0f)
		{
			a3characterToggleIsJumping(controller);
			// jump
			*controller->jumpTrigger = 0.0f;

			//controller->activeAnimController = &controller->animControllers[3];
		}
		if (controller->jumpRemaining > 0.0f)
		{
			controller->jumpRemaining -= dt;
			if (controller->jumpRemaining <= 0.0f)
			{
				controller->isJumping = false;
			}
		}

		//we have a pose that's just the sampled idle pose
		a3_HierarchyPose result[1];

		a3hierarchyPoseOpCreate(result, controller->poseGroup->hierarchy->numNodes);
		a3hierarchyPoseOpIdentity(result, controller->poseGroup->hierarchy->numNodes);
		a3clipOpSampleClip(result, controller->poseGroup, &controller->animControllers[0]);
		//if we're walking, the pose is replaced with walking
		if (controller->currentVelocity > 0.01f)
		{
			a3characterControllerWalk(controller, result);
		}
		//if we're jumping, lerp between walk and jump

		if (controller->isJumping)
		{
			a3_HierarchyPose postJumpResult[1];
			a3hierarchyPoseOpCreate(postJumpResult, controller->poseGroup->hierarchy->numNodes);
			a3hierarchyPoseOpIdentity(postJumpResult, controller->poseGroup->hierarchy->numNodes);
			//a3characterControllerJump(controller, output, dt); //rewrite this function so we take _in_ walk run blend (as result) and output the final pose to the output variable
			a3characterControllerBlendJump(controller, postJumpResult, result, dt);

			a3hierarchyPoseOpCopy(result, postJumpResult, controller->poseGroup->hierarchy->numNodes);
		}

		a3hierarchyPoseOpCopy(output, result, controller->poseGroup->hierarchy->numNodes);
		//if (controller->currentVelocity > 0.0f && !controller->isJumping)
		//{
		//	// walk
		//	a3characterControllerWalk(controller, output);
		//	controller->activeAnimController = &controller->animControllers[1];
		//}
		//else if (controller->currentVelocity <= 0.01f)
		//{
		//	// idle
		//	controller->activeAnimController = &controller->animControllers[0];
		//}

		return 1;
	}

	return 0;
}

inline void a3characterToggleIsJumping(a3_CharacterController* controller)
{
	if (!controller->isJumping)
	{
		controller->isJumping = true;
		controller->jumpRemaining = JUMP_DURATION;
		a3clipControllerSetClip(&controller->animControllers[3], controller->animControllers[3].clipPool, controller->jumpClipIndex, controller->animControllers[3].playback_step, controller->animControllers[3].playback_stepPerSec);
		controller->animControllers[3].clipTime_sec = 0.0f;
		controller->animControllers[3].clipParam = 0.0f;
		controller->animControllers[3].keyframeTime_sec = 0.0f;
		controller->animControllers[3].keyframeParam = 0.0f;
		controller->jumpTransitionVal = 0.0f;
		//set clip
	}
	// this is just a stub from experimenting with "events"
}

inline a3ui32 a3characterControllerJump(a3_CharacterController* controller, a3_HierarchyPose* output, a3real dt)
{
	// just trigger animation for now, still need to actually modify the position

	a3real transitionMultiplier = 1.5f;
	//if we have more than the time it takes to transition left in the jump, we're transitioning into it.
	if (controller->jumpRemaining >= 1.0f / transitionMultiplier)
	{
		controller->jumpTransitionVal += transitionMultiplier * dt;

		if (controller->jumpTransitionVal > 1.0f)
		{
			controller->jumpTransitionVal = 1.0f;
		}
	}
	else //if less than that time remains, we're exiting the jump so the u value decreases.
	{
		controller->jumpTransitionVal -= transitionMultiplier * dt;

		if (controller->jumpTransitionVal <= 0.0f)
		{
			controller->jumpTransitionVal = 0.0f;
		}
	}
	

	a3_HierarchyPose jumpResult[1];

	a3hierarchyPoseOpCreate(jumpResult, controller->poseGroup->hierarchy->numNodes);
	a3hierarchyPoseOpIdentity(jumpResult, controller->poseGroup->hierarchy->numNodes);

	//a3clipOpSampleClip(jumpResult, controller->poseGroup, &controller->animControllers[3]);
	a3clipOpLerp(output, controller->poseGroup, controller->activeAnimController, &controller->animControllers[3], controller->jumpTransitionVal);


	//controller->isJumping = true;

	return 1;
}

inline a3ui32 a3characterControllerBlendJump(a3_CharacterController* controller, a3_HierarchyPose* output, a3_HierarchyPose* blendInput, a3real dt)
{
	// just trigger animation for now, still need to actually modify the position

	a3real transitionMultiplier = 1.5f;
	//if we have more than the time it takes to transition left in the jump, we're transitioning into it.
	if (controller->jumpRemaining >= 1.0f / transitionMultiplier)
	{
		controller->jumpTransitionVal += transitionMultiplier * dt;

		if (controller->jumpTransitionVal > 1.0f)
		{
			controller->jumpTransitionVal = 1.0f;
		}
	}
	else //if less than that time remains, we're exiting the jump so the u value decreases.
	{
		controller->jumpTransitionVal -= transitionMultiplier * dt;

		if (controller->jumpTransitionVal <= 0.0f)
		{
			controller->jumpTransitionVal = 0.0f;
		}
	}


	a3_HierarchyPose jumpResult[1];

	a3hierarchyPoseOpCreate(jumpResult, controller->poseGroup->hierarchy->numNodes);
	a3hierarchyPoseOpIdentity(jumpResult, controller->poseGroup->hierarchy->numNodes);

	a3clipOpSampleClip(jumpResult, controller->poseGroup, &controller->animControllers[3]);
	a3hierarchyPoseOpLERP(output, blendInput, jumpResult, controller->jumpTransitionVal, controller->poseGroup->hierarchy->numNodes);
	//a3clipOpLerp(output, controller->poseGroup, controller->activeAnimController, &controller->animControllers[3], controller->jumpTransitionVal);


	//controller->isJumping = true;

	return 1;
}

inline a3ui32 a3characterControllerWalk(a3_CharacterController* controller, a3_HierarchyPose* output)
{
	a3real u = controller->currentVelocity / controller->maxWalkVelocity;

	if (u > 1.0f)
	{
		u = 1.0f;
	}

	a3clipOpLerp(output, controller->poseGroup, &controller->animControllers[1], &controller->animControllers[2], u);

	return 1;
}

inline float getWalkScale(float u)
{
	return (float)pow(0.75f, u);
}

inline float getRunScale(float u)
{
	return (float)pow(0.75f, 1.0f - u);
}

//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_CHARACTER_CONTROLLER_INL
#endif	// __ANIMAL3D_CHARACTER_CONTROLLER_H