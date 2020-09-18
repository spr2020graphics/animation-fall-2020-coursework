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
	
	a3_KeyframeAnimation.h
	Data structures for fundamental keyframe animation.
*/

/*
	Animation Framework Addons by Cameron Schneider and Scott Dagen
*/

#ifndef __ANIMAL3D_KEYFRAMEANIMATION_H
#define __ANIMAL3D_KEYFRAMEANIMATION_H


#include "animal3D-A3DM/a3math/a3vector.h"
#include "animal3D-A3DM/a3math/a3interpolation.h"


//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
typedef union a3_Sample						a3_Sample;
typedef struct a3_Keyframe					a3_Keyframe;
typedef struct a3_KeyframePool				a3_KeyframePool;
typedef struct a3_Clip						a3_Clip;
typedef struct a3_ClipPool					a3_ClipPool;
typedef struct a3_ClipTransition			a3_ClipTransition;
typedef enum a3_ClipTransitionBehavior		a3_ClipTransitionBehavior;
#endif	// __cplusplus
//-----------------------------------------------------------------------------

// constant values
enum
{
	a3keyframeAnimation_nameLenMax = 32,
};

enum a3_ClipTransitionBehavior
{
	a3clipTransitionPause,
	a3clipTransitionForward,
	a3clipTransitionForwardPause, //pause at start of first frame
	a3clipTransitionReverse,
	a3clipTransitionReversePause, //pause at "end" of last frame
	a3clipTransitionForwardSkip, //play from second frame
	a3clipTransitionForwardFrame, //play only first frame
	a3clipTransitionReverseSkip, //play reverse from second-to-last
	a3clipTransitionReverseFrame //play only last frame
};


// generic sample: value of function at time
union a3_Sample
{
	// Uses union & data struct to encapsulate time and value in sample (x, y)
	a3vec2 sample;

	struct  
	{
		a3real time, value;
	};
};


// description of single keyframe (Scott Dagen)
// metaphor: moment
struct a3_Keyframe
{
	// index in keyframe pool
	a3ui32 index;
	// positive float specifying how long this keyframe is active
	a3f32 duration;
	// reciprocal of duration.
	a3f32 durInv;

	// Actual data for the keyframe (more to be done)
	// Handle is going to be an arbitrary 'point' off to the side of an actual keyframe to form a curve (a control point)
	a3_Sample sample, handle;
};

// pool of keyframe descriptors (Cameron Schneider)
struct a3_KeyframePool
{
	// array of keyframes
	a3_Keyframe *keyframeArray;

	// number of keyframes
	a3ui32 count;
};


// allocate keyframe pool (Cameron Schneider)
a3i32 a3keyframePoolCreate(a3_KeyframePool* keyframePool_out, const a3ui32 count);

// release keyframe pool (Cameron Schneider)
a3i32 a3keyframePoolRelease(a3_KeyframePool* keyframePool);

// initialize keyframe (Scott Dagen)
a3i32 a3keyframeInit(a3_Keyframe* keyframe_out, const a3real duration, const a3_Sample* samp);


//-----------------------------------------------------------------------------

struct a3_ClipTransition
{
	//null = pause
	a3_ClipPool* targetClipPool;

	a3byte targetClipName[a3keyframeAnimation_nameLenMax];

	a3_ClipTransitionBehavior transition;

	a3f32 cachedOverstep;
};

// description of single clip (Scott Dagen)
// metaphor: timeline
struct a3_Clip
{
	// clip name
	a3byte name[a3keyframeAnimation_nameLenMax];

	// index in clip pool
	a3ui32 index;

	//positive float specifying how long the clip runs for.
	a3f32 duration;

	//reciprocal of duration
	a3f32 durInv;

	// Number of keyframes used by this clip
	a3ui32 keyframeCount;

	//indices of the first and last keyframes
	a3ui32 firstKeyframeIndex, lastKeyframeIndex;

	a3_ClipTransition forwardTransition, reverseTransition;

	const a3_KeyframePool* keyframes;
};

// group of clips (Cameron Schneider)
struct a3_ClipPool
{
	// array of clips
	a3_Clip* clipArray;

	// number of clips
	a3ui32 count;
};


// allocate clip pool (Cameron Schneider)
a3i32 a3clipPoolCreate(a3_ClipPool* clipPool_out, const a3ui32 count);

// release clip pool (Cameron Schneider)
a3i32 a3clipPoolRelease(a3_ClipPool* clipPool);

// initialize clip with first and last indices (Scott Dagen)
a3i32 a3clipInit(a3_Clip* clip_out, const a3byte clipName[a3keyframeAnimation_nameLenMax], const a3_KeyframePool* keyframePool, const a3ui32 firstKeyframeIndex, const a3ui32 finalKeyframeIndex);

// get clip index from pool (Scott Dagen)
a3i32 a3clipGetIndexInPool(const a3_ClipPool* clipPool, const a3byte clipName[a3keyframeAnimation_nameLenMax]);

// calculate clip duration as sum of keyframes' durations (Scott Dagen)
a3i32 a3clipCalculateDuration(a3_Clip* clip);

// calculate keyframes' durations by distributing clip's duration (Scott Dagen)
a3i32 a3clipDistributeDuration(a3_Clip* clip, const a3real newClipDuration);

// lerp between 3D vectors
a3vec3 a3vecLerp(a3vec3 v1, a3vec3 v2, a3real u);

//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#include "_inl/a3_KeyframeAnimation.inl"


#endif	// !__ANIMAL3D_KEYFRAMEANIMATION_H
