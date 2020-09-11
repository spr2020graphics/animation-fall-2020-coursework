/*
	Copyright 2011-2020 Daniel S. Buckstein

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	animal3D SDK: Minimal 3D Animation Framework
	By Daniel S. Buckstein
	
	a3_KeyframeAnimation.c
	Implementation of keyframe animation interfaces.
*/

/*
	Animation Framework Addons by Scott Dagen and Cameron Schneider
*/

#include "../a3_KeyframeAnimation.h"

#include <stdlib.h>
#include <string.h>


// macros to help with names
#define A3_CLIP_DEFAULTNAME		("unnamed clip")
#define A3_CLIP_SEARCHNAME		((clipName && *clipName) ? clipName : A3_CLIP_DEFAULTNAME)


//-----------------------------------------------------------------------------

// allocate keyframe pool
a3i32 a3keyframePoolCreate(a3_KeyframePool* keyframePool_out, const a3ui32 count)
{
	if (count < 0)
	{
		// Don't init with negative numbers please.
		return -1;
	}

	keyframePool_out->count = count;
	keyframePool_out->keyframeArray = malloc(sizeof(a3_Keyframe) * count);

	//loop through all frames to create, then init them with 0. Each frame needs a re-init later (probably, unless you want a lot of 0 keyframes)
	for (a3ui32 i = 0; i < count; i++)
	{
		if (keyframePool_out->keyframeArray != NULL)
		{
			keyframePool_out->keyframeArray[i].index = i; //intellisense error for buffer overrun
			a3keyframeInit(&keyframePool_out->keyframeArray[i], 1, 0);
		}
	}
	return 1;
}

// release keyframe pool
a3i32 a3keyframePoolRelease(a3_KeyframePool* keyframePool)
{
	free(keyframePool->keyframeArray);
	free(keyframePool);

	return 1;
}

// initialize keyframe with default values. Passing in a null keyframe returns -1.
a3i32 a3keyframeInit(a3_Keyframe* keyframe_out, const a3real duration, const a3ui32 value_x)
{
	if (keyframe_out == NULL)
	{
		return -1;
	}
	keyframe_out->duration = duration;
	keyframe_out->durInv = 1.0f / duration;
	keyframe_out->data = value_x;
	return 1;
}


// allocate clip pool
a3i32 a3clipPoolCreate(a3_ClipPool* clipPool_out, const a3ui32 count)
{
	if (count < 0)
	{
		return -1;
	}

	clipPool_out->count = count;
	clipPool_out->clipArray = malloc(sizeof(a3_Clip) * count);

	//loop through all clips to create and init with default values. Needs another init call later for anything to work.
	for (a3ui32 i = 0; i < count; i++)
	{
		if (clipPool_out->clipArray != NULL)
		{
			clipPool_out->clipArray[i].index = i; //intellisense error for buffer overrun
			a3clipInit(&clipPool_out->clipArray[i], A3_CLIP_DEFAULTNAME, NULL, 0, 0);
		}
	}

	return 1;
}

// release clip pool
a3i32 a3clipPoolRelease(a3_ClipPool* clipPool)
{
	free(clipPool->clipArray);
	free(clipPool);

	return 1;
}

// initialize clip with first and last indices
a3i32 a3clipInit(a3_Clip* clip_out, const a3byte clipName[a3keyframeAnimation_nameLenMax], const a3_KeyframePool* keyframePool, const a3ui32 firstKeyframeIndex, const a3ui32 finalKeyframeIndex)
{
	if (clip_out == NULL)
	{
		return -1;
	}
	memcpy(clip_out->name, clipName, a3keyframeAnimation_nameLenMax); //copies the clipName into clip_out->name. The first way we tried this yielded an error.
	clip_out->keyframes = keyframePool;
	clip_out->firstKeyframeIndex = firstKeyframeIndex;
	clip_out->lastKeyframeIndex = finalKeyframeIndex;
	clip_out->keyframeCount = finalKeyframeIndex - firstKeyframeIndex + 1; //need to include both!
	return 1;
}

// get clip index from pool
a3i32 a3clipGetIndexInPool(const a3_ClipPool* clipPool, const a3byte clipName[a3keyframeAnimation_nameLenMax])
{
	a3ui32 poolCount = clipPool->count;

	for (a3ui32 i = 0; i < poolCount; i++)
	{
		if (strcmp(clipPool->clipArray[i].name, clipName) == 0)
		{
			return clipPool->clipArray[i].index;
		}
	}

	return -1;
}


//-----------------------------------------------------------------------------
