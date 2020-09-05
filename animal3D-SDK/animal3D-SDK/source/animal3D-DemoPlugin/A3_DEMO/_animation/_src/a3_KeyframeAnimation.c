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

	if (keyframePool_out != NULL)
	{
		a3keyframePoolRelease(keyframePool_out);
	}

	keyframePool_out = (a3_KeyframePool*)malloc(sizeof(a3_KeyframePool));
	keyframePool_out->count = count;
	keyframePool_out->keyframeArray = malloc(sizeof(a3_Keyframe) * count);

	for (a3ui32 i = 0; i < count; i++)
	{
		keyframePool_out->keyframeArray[i].duration = 1.0f;
		keyframePool_out->keyframeArray[i].durInv = 1.0f / 1.0f;
		keyframePool_out->keyframeArray[i].index = i;
		keyframePool_out->keyframeArray[i].data = 0;
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

// initialize keyframe
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

	if (clipPool_out != NULL)
	{
		a3clipPoolRelease(clipPool_out);
	}

	clipPool_out = (a3_ClipPool*)malloc(sizeof(a3_ClipPool));
	clipPool_out->count = count;
	clipPool_out->clipArray = malloc(sizeof(a3_Clip) * count);

	for (a3ui32 i = 0; i < count; i++)
	{
		a3clipInit(&clipPool_out->clipArray[i], A3_CLIP_DEFAULTNAME, clipPool_out->clipArray[i].keyframePool, clipPool_out->clipArray[i].firstKeyframeIndex, clipPool_out->clipArray[i].lastKeyframeIndex);
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
	memcpy(clip_out->name, clipName, a3keyframeAnimation_nameLenMax);
	clip_out->keyframePool = keyframePool;
	clip_out->firstKeyframeIndex = firstKeyframeIndex;
	clip_out->lastKeyframeIndex = finalKeyframeIndex;
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
