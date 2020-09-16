#include "A3_DEMO/_animation/a3_KeyframeAnimationIO.h"
#include <string.h>
#include <stdio.h>


a3i32 a3clipParse(a3_DemoState* state, a3byte const* data)
{
	//@ clip_name use_clip_duration first_frame last_frame forward_trans forward_trans_dest reverse_trans reverse_trans_dest clip_duration
	//char clipName[a3keyframeAnimation_nameLenMax];
	//char forwardTransClipName[a3keyframeAnimation_nameLenMax];
	//char reverseTransClipName[a3keyframeAnimation_nameLenMax];
	a3boolean useClipDuration = false;
	a3ui16 firstFrame = 0;
	a3ui16 lastFrame = 0;
	//a3_ClipTransition forwardTrans, reverseTrans;
	a3real clipDuration = 0.0f;

	int lineIndex = 0;
	int characterIndex = -1;
	char* token = strtok((char*)data, " ");
	char* temp = strchr(token, ' ');	//https://stackoverflow.com/questions/3217629/how-do-i-find-the-index-of-a-character-within-a-string-in-c

	characterIndex = (int)(temp - token);

	while (token != NULL)
	{
		switch (lineIndex)
		{
		case 0:
			// Assign clip name
			
			break;
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			break;
		case 5:
			break;
		case 6:
			break;
		case 7:
			break;
		default:
			break;
		}
	}

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

	// Counts the number of clips and keyframes so we can initialize pools
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

		token = strtok(NULL, "\n");
	}

	state->clipCount = clipPoolSize;
	state->keyframeCount = keyframePoolSize;
	a3clipPoolCreate(state->clipPool, state->clipCount);

	token = strtok((char*)data, "\n");
	while (token != NULL)
	{
		switch (token[0])
		{
		case '@':
			a3clipParse(state, token);
			break;
		case '$':
			a3keyframeParse(state, token);
			break;
		default:
			break;
		}
		token = strtok(token + strlen(token) + 1, "\n");
	}

	return 0;
}