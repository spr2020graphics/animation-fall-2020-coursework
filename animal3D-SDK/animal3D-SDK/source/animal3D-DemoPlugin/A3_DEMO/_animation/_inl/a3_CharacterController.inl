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
			// jump
			a3characterControllerJump(controller, output);
			*controller->jumpTrigger = 0.0f;
		}
		else if (controller->currentVelocity > 0.0f && !controller->isJumping)
		{
			// walk
			a3characterControllerWalk(controller, output);
		}

		return 1;
	}

	return 0;
}

inline void a3characterToggleIsJumping()
{
	// this is just a stub from experimenting with "events"
}

inline a3ui32 a3characterControllerJump(a3_CharacterController* controller, a3_HierarchyPose* output)
{
	// just trigger animation for now, still need to actually modify the position

	

	controller->isJumping = true;

	return 1;
}

inline a3ui32 a3characterControllerWalk(a3_CharacterController* controller, a3_HierarchyPose* output)
{
	a3real u = controller->currentVelocity / controller->maxWalkVelocity;

	a3clipOpLerp(output, controller->poseGroup, &controller->animControllers[1], &controller->animControllers[2], u);

	return 1;
}

//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_CHARACTER_CONTROLLER_INL
#endif	// __ANIMAL3D_CHARACTER_CONTROLLER_H