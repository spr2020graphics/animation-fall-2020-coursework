#ifdef __ANIMAL3D_CHARACTER_CONTROLLER_H
#ifndef __ANIMAL3D_CHARACTER_CONTROLLER_INL
#define __ANIMAL3D_CHARACTER_CONTROLLER_INL

#include <stdio.h>

//-----------------------------------------------------------------------------


inline a3ui32 a3characterControllerInit(a3_CharacterController* controller_out, a3_ClipController* controller, a3_DemoSceneObject* obj, a3f32 jump, a3f32 walkThreshold, a3f32 runThreshold)
{
	controller_out->animController = controller;
	controller_out->jumpTrigger = jump;
	controller_out->maxWalkVelocity = walkThreshold;
	controller_out->maxRunVelocity = runThreshold;
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

	return 1;
}

inline void a3characterToggleIsJumping()
{
	// this is just a stub from experimenting with "events"
}

inline a3ui32 a3characterControllerJump(a3_CharacterController* controller, a3f32 blendVal)
{
	// just trigger animation for now, still need to actually modify the position
	a3clipControllerSetClip(controller->animController, controller->animController->clipPool, controller->jumpClipIndex, controller->animController->playback_step, controller->animController->playback_stepPerSec);
	controller->isJumping = true;

	return 1;
}

inline a3ui32 a3characterControllerWalk(a3_CharacterController* controller, a3f32 blendVal)
{
	a3clipControllerSetClip(controller->animController, controller->animController->clipPool, controller->walkClipIndex, controller->animController->playback_step, controller->animController->playback_stepPerSec);

	return 1;
}

inline a3ui32 a3characterControllerRun(a3_CharacterController* controller, a3f32 blendVal)
{
	a3clipControllerSetClip(controller->animController, controller->animController->clipPool, controller->runClipIndex, controller->animController->playback_step, controller->animController->playback_stepPerSec);

	return 1;
}

inline a3ui32 a3characterControllerIdle(a3_CharacterController* controller, a3f32 blendVal)
{
	a3clipControllerSetClip(controller->animController, controller->animController->clipPool, controller->idleClipIndex, controller->animController->playback_step, controller->animController->playback_stepPerSec);

	return 1;
}
//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_CHARACTER_CONTROLLER_INL
#endif	// __ANIMAL3D_CHARACTER_CONTROLLER_H