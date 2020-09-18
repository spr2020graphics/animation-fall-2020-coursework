/*
	Animation Framework Addons by Cameron Schneider and Scott Dagen
	a3_KeyframeAnimationIO.h: loads animation data from a file, parsing clips and keyframes based on a decided format.
*/


#include "A3_DEMO/_animation/a3_KeyframeAnimationIO.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


// Parse a clip line, denoted by the '@' symbol in the file. clipIndex must be passed in, since it can't be included in the file
a3i32 a3clipParse(a3_DemoState* state, a3byte const* data, const a3ui32 clipIndex)
{
	//create a copy of the passed-in data to avoid modifying it (strtok has side effects)
	char* parseDataCopy = malloc(strlen((char*)data) * sizeof(char));
	memcpy(parseDataCopy, (char*)data, strlen((char*)data));


	//variables for clip
	//@ clip_name use_clip_duration first_frame last_frame forward_trans forward_trans_dest reverse_trans reverse_trans_dest clip_duration
	char* clipName = calloc(a3keyframeAnimation_nameLenMax, sizeof(char));
	a3boolean useClipDuration = false;
	a3ui16 firstFrame = 0;
	a3ui16 lastFrame = 0;
	a3_ClipTransition forwardTrans, reverseTrans;
	a3real clipDuration = 0.0f;

	// 'lineIndex' is what variable we're looking at in the file
	int lineIndex = 0;
	char* token = strtok((char*)parseDataCopy, " ");

	// There are only 9 variables to account for in the file
	while (token != NULL && lineIndex <= 9)
	{
		//this chops off the \r that appears after the last data entry in a line. It's unclear why it exists but this gets rid of it.
		char* s = strchr(token, '\r');
		if (s != NULL)
		{
			s[0] = '\0';
		}
		switch (lineIndex)
		{
		case 1: // Assign clip name
			memcpy(clipName, token, strlen(token));
			break;
		case 2:	// Determine if we are using the clip's duration or the keyframe durations
			useClipDuration = strcmp(token, "true") == 0;
			break;
		case 3:	// First keyframe index
			firstFrame = atoi(token);
			break;
		case 4:	// Last keyframe index
			lastFrame = atoi(token);
			break;
		case 5:	// Forward transition type
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
		case 6: // Forward transition destination
			memcpy(forwardTrans.targetClipName, token, strlen(token));
			break;
		case 7:	// Reverse transition type
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
		case 8: // Reverse transition destination
			memcpy(reverseTrans.targetClipName, token, strlen(token));
			break;
		case 9: // Assign clip duration if we are using it
		{
			if (useClipDuration)
			{
				clipDuration = (float)atof(token);
			}
		}
			break;
		default:
			break;
		}

		token = strtok(token + strlen(token) + 1, " ");	// token + strlen(token) + 1 moves us to the next line in the file without breaking data
		lineIndex++;
	}

	// Initialize the clip and its transitions
	a3clipInit(state->clipPool->clipArray + clipIndex, clipName, state->keyPool, firstFrame, lastFrame);	//Need to override this with transition setup

	// Calculate the duration based on if the user specifies to use the clip duration or total keyframe durations
	if (useClipDuration)
	{
		a3clipCalculateDuration(state->clipPool->clipArray + clipIndex);
	}
	else
	{
		a3clipDistributeDuration(state->clipPool->clipArray + clipIndex, clipDuration);
	}

	return 0;
}

a3i32 a3keyframeParse(a3_DemoState* state, a3byte const* data)
{
	//create a copy of the passed-in data to avoid modifying it (strtok has side effects)
	char* parseDataCopy = calloc(strlen((char*)data), sizeof(char));
	memcpy(parseDataCopy, (char*)data, strlen((char*)data));

	// Variables to extract
	a3ui32 index;
	a3f32 duration;
	a3_Sample sample;

	// lineIndex is not actually a line, but the variable index
	int lineIndex = 0;
	char* token = strtok((char*)parseDataCopy, " ");

	while (token != NULL && lineIndex <= 3)
	{
		//this chops off the \r that appears after the last data entry in a line. It's unclear why it exists but this gets rid of it.
		char* s = strchr(token, '\r');
		if (s != NULL)
		{
			s[0] = '\0';
		}
		switch (lineIndex)
		{
		case 1:	// Assign keyframe duration
			duration = (float)atof(token);
			break;
		case 2:	// Assign keyframe index
			index = atoi(token);
			break;
		case 3:	// Assign the sample value
			sample.value = (float)atof(token);
			break;
		default:
			break;
		}

		token = strtok(token + strlen(token) + 1, " ");
		lineIndex++;
	}

	// Initialize the keyframe
	a3keyframeInit(state->keyPool->keyframeArray + index, duration, &sample);

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

	// Counts the number of clips and keyframes so we can initialize pools before creating clips/keyframes
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

	// Create both kinds of pools
	state->clipCount = clipPoolSize;
	state->keyframeCount = keyframePoolSize;
	a3clipPoolCreate(state->clipPool, state->clipCount);
	a3keyframePoolCreate(state->keyPool, state->keyframeCount);
	token = strtok((char*)data, "\n");

	a3ui32 currentClipIndex = 0;	//Needed only for clips, since their indices aren't specified in-file

	// Now that we have the clipPool and keyframePool created, we can actually re-iterate and create the actual clips/keyframes
	while (token != NULL)
	{
		switch (token[0])
		{
		case '@':
			a3clipParse(state, token, currentClipIndex);
			currentClipIndex++;
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