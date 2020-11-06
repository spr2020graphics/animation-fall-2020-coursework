/*
	Animation Framework Addons by Cameron Schneider and Scott Dagen
	a3_KeyframeAnimationIO.h: loads animation data from a file
*/
#include "a3_KeyframeAnimation.h"

#ifndef __ANIMAL3D_KEYFRAMEANIMATIONIO_H
#define __ANIMAL3D_KEYFRAMEANIMATIONIO_H

#include "A3_DEMO/a3_DemoState.h"
#pragma once

// Parse the given data to create keyframes and clips
a3i32 a3animationParseFile(a3_DemoState* demoState, a3byte const* data);

#endif