#include "A3_DEMO/_animation/a3_KeyframeAnimationIO.h"

a3i32 a3clipParse(a3_DemoState* state, a3byte const* data)
{
	return 0;
}

a3i32 a3keyframeParse(a3_DemoState* state, a3byte const* data)
{
	return 0;
}

a3i32 a3animationParseFile(a3_DemoState* state, a3byte const* data)
{
	int clipPoolSize = 0;
	state->clipCount = clipPoolSize;
	a3clipPoolCreate(state->clipPool, state->clipCount);
	return 0;
}