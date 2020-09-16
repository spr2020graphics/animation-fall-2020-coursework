#include "A3_DEMO/_animation/a3_KeyframeAnimationIO.h"


a3i32 a3clipParse(a3_DemoState* state, a3byte const* data)
{
	return 0;
}

a3i32 a3keyframeParse(a3_DemoState* state, a3byte const* data)
{
	return 0;
}

/// <summary>
/// Entry point for parsing animation data
/// </summary>
/// <param name="state">The program's state data</param>
/// <param name="data">File data</param>
/// <returns></returns>
a3i32 a3animationParseFile(a3_DemoState* state, a3byte const* data)
{
	int clipPoolSize = 0;
	int keyframePoolSize = 0;

	char* token;

	// Just a delimiter function, using it to pull out whole lines, since the material file is formatted in a certain way
	token = strtok((char*)data, "\n");

	while (token != NULL)
	{
		switch (token[0])
		{
		case '#':
			break;
		case '@':
			clipPoolSize++;
			break;
		case '$':
			keyframePoolSize++;
			break;
		default:
			break;
		}
	}

	state->clipCount = clipPoolSize;
	a3clipPoolCreate(state->clipPool, state->clipCount);
	return 0;
}