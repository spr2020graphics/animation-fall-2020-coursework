#include "A3_DEMO/_animation/a3_KeyframeAnimationIO.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


a3i32 a3clipParse(a3_DemoState* state, a3byte const* data)
{
	//create a copy of the passed-in data to avoid modifying it (strtok has side effects)
	char* parseDataCopy = malloc(strlen((char*)data) * sizeof(char));
	memcpy(parseDataCopy, (char*)data, strlen((char*)data));

	//variables for clip
	//@ clip_name use_clip_duration first_frame last_frame forward_trans forward_trans_dest reverse_trans reverse_trans_dest clip_duration
	char* clipName = calloc(a3keyframeAnimation_nameLenMax, sizeof(char));
	//char forwardTransClipName[a3keyframeAnimation_nameLenMax];
	//char reverseTransClipName[a3keyframeAnimation_nameLenMax];
	a3boolean useClipDuration = false;
	a3ui16 firstFrame = 0;
	a3ui16 lastFrame = 0;
	a3_ClipTransition forwardTrans, reverseTrans;
	a3real clipDuration = 0.0f;

	int lineIndex = 0;
	char* token = strtok((char*)parseDataCopy, " ");

	while (token != NULL && lineIndex < 9)
	{
		switch (lineIndex)
		{
		case 1:
			// Assign clip name
			memcpy(clipName, token, strlen(token));
			break;
		case 2:
			useClipDuration = strcmp(token, "true") == 0;
			break;
		case 3:
			firstFrame = atoi(token);
			break;
		case 4:
			lastFrame = atoi(token);
			break;
		case 5:
		{
			if (strcmp(token, "|") == 0)
			{
				forwardTrans.transition = a3clipTransitionPause;
			}
			else if (strcmp(token, ">") == 0)
			{
				forwardTrans.transition = a3clipTransitionForward;
			}
			else if (strcmp(token, ">|") == 0)
			{
				forwardTrans.transition = a3clipTransitionForwardPause;
			}
			else if (strcmp(token, "<") == 0)
			{
				forwardTrans.transition = a3clipTransitionReverse;
			}
			else if (strcmp(token, "<|") == 0)
			{
				forwardTrans.transition = a3clipTransitionReversePause;
			}
			else if (strcmp(token, ">>") == 0)
			{
				forwardTrans.transition = a3clipTransitionForwardSkip;
			}
			else if (strcmp(token, ">>|") == 0)
			{
				forwardTrans.transition = a3clipTransitionForwardFrame;
			}
			else if (strcmp(token, "<<") == 0)
			{
				forwardTrans.transition = a3clipTransitionReverseSkip;
			}
			else if (strcmp(token, "<<|") == 0)
			{
				forwardTrans.transition = a3clipTransitionReverseFrame;
			}
		}
			break;
		case 6: //destination
			break;
		case 7:
		{
			if (strcmp(token, "|") == 0)
			{
				reverseTrans.transition = a3clipTransitionPause;
			}
			else if (strcmp(token, ">") == 0)
			{
				reverseTrans.transition = a3clipTransitionForward;
			}
			else if (strcmp(token, ">|") == 0)
			{
				reverseTrans.transition = a3clipTransitionForwardPause;
			}
			else if (strcmp(token, "<") == 0)
			{
				reverseTrans.transition = a3clipTransitionReverse;
			}
			else if (strcmp(token, "<|") == 0)
			{
				reverseTrans.transition = a3clipTransitionReversePause;
			}
			else if (strcmp(token, ">>") == 0)
			{
				reverseTrans.transition = a3clipTransitionForwardSkip;
			}
			else if (strcmp(token, ">>|") == 0)
			{
				reverseTrans.transition = a3clipTransitionForwardFrame;
			}
			else if (strcmp(token, "<<") == 0)
			{
				reverseTrans.transition = a3clipTransitionReverseSkip;
			}
			else if (strcmp(token, "<<|") == 0)
			{
				reverseTrans.transition = a3clipTransitionReverseFrame;
			}
		}
			break;
		case 8: //destination
			break;
		case 9: //destination
			clipDuration = (float)atof(token);
			break;
		default:
			break;
		}

		token = strtok(token + strlen(token) + 1, " ");
		lineIndex++;
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