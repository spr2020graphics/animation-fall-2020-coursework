#ifdef __ANIMAL3D_CHARACTER_CONTROLLER_H
#ifndef __ANIMAL3D_CHARACTER_CONTROLLER_INL
#define __ANIMAL3D_CHARACTER_CONTROLLER_INL

#include <stdio.h>

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
		for (a3ui32 i = 0; i < 4; i++)
		{
			a3clipControllerUpdate(&controller->animControllers[i], dt);
		}

		if (*controller->jumpTrigger == 1.0f)
		{
			a3characterToggleIsJumping(controller);
			// jump
			*controller->jumpTrigger = 0.0f;

			controller->activeAnimController = &controller->animControllers[3];
		}
		if (controller->jumpRemaining > 0.0f)
		{
			controller->jumpRemaining -= dt;
			if (controller->jumpRemaining <= 0.0f)
			{
				controller->isJumping = false;
			}
		}
		if (controller->isJumping)
		{
			a3characterControllerJump(controller, output);
		}
		if (controller->currentVelocity > 0.0f && !controller->isJumping)
		{
			// walk
			a3characterControllerWalk(controller, output);
			controller->activeAnimController = &controller->animControllers[1];
		}
		else
		{
			// idle
			controller->activeAnimController = &controller->animControllers[0];
		}

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
		//set clip
	}
	// this is just a stub from experimenting with "events"
}

inline a3ui32 a3characterControllerJump(a3_CharacterController* controller, a3_HierarchyPose* output)
{
	// just trigger animation for now, still need to actually modify the position

	a3real u = 1.0f;

	a3clipOpLerp(output, controller->poseGroup, controller->activeAnimController, &controller->animControllers[3], u);

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

//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_CHARACTER_CONTROLLER_INL
#endif	// __ANIMAL3D_CHARACTER_CONTROLLER_H