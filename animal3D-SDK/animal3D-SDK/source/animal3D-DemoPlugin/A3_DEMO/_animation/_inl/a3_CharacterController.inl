#ifdef __ANIMAL3D_CHARACTER_CONTROLLER_H
#ifndef __ANIMAL3D_CHARACTER_CONTROLLER_INL
#define __ANIMAL3D_CHARACTER_CONTROLLER_INL


//-----------------------------------------------------------------------------


inline a3ui32 a3characterControllerInit(a3_CharacterController* controller_out, a3_ClipController* controller, a3f32 jump, a3f32 walkThreshold, a3f32 runThreshold)
{
	controller_out->animController = controller;
	controller_out->jumpTrigger = jump;
	controller_out->maxWalkThreshold = walkThreshold;
	controller_out->maxRunThreshold = runThreshold;

	return 1;
}

inline a3ui32 a3characterControllerApplyInput(a3_DemoSceneObject* object, a3vec2* position, a3real rotation)
{
	object->position.x = +(position->x);
	object->position.y = +(position->y);

	object->euler.z = -a3trigValid_sind(rotation);

	return 1;
}

//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_CHARACTER_CONTROLLER_INL
#endif	// __ANIMAL3D_CHARACTER_CONTROLLER_H