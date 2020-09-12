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
	
	a3_KeyframeAnimationController.h
	Keyframe animation clip controller. Basically a frame index manager. Very 
	limited in what one can do with this; could potentially be so much more.
*/

/*
	Animation Framework Addons by Cameron Schneider and Scott Dagen
*/

#ifndef __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_H
#define __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_H


#include "a3_KeyframeAnimation.h"


//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
typedef struct a3_ClipController			a3_ClipController;
#endif	// __cplusplus


//-----------------------------------------------------------------------------

// clip controller (Cameron Schneider & Scott Dagen)
// metaphor: playhead
struct a3_ClipController
{
	a3byte name[a3keyframeAnimation_nameLenMax];

	// Index of clip to control in the referenced clip pool, aka which clip is currently playing
	a3ui32 clipIndex;

	// Current time relative to start of clip [0, currentClipDuration)
	a3f32 clipTime;

	// Normalized clip time, between 0 and 1
	a3f32 clipParameter;

	// Index of the current keyframe in the current clip's keyframe pool (essentially the progress in the clip)
	a3ui32 keyframeIndex; //keyframeIndex1 (also need more for Catmull, but we should just use math on the indices, and use a function to retrieve a pointer to the right keyframe to avoid those long statements)

	// TODO: MAKE KEYFRAME0PTR and KEYFRAME1PTR

	// Current time relative to current keyframe (between 0 and current keyframe's duration)
	a3f32 keyframeTime;

	// Normalized keyframe time, between 0 and 1
	a3f32 keyframeParameter;

	// Corresponds to the above enum
	a3i32 playbackDir;

	// Referenced pool of clips this controller has control over (we have the clip index above)
	const a3_ClipPool* clipPool;

	// Local modifier for playback speed, used per-controller
	a3f32 speedMod;

	a3boolean delayedPause;
};


//-----------------------------------------------------------------------------

// initialize clip controller (Cameron Schneider)
a3i32 a3clipControllerInit(a3_ClipController* clipCtrl_out, const a3byte ctrlName[a3keyframeAnimation_nameLenMax], const a3_ClipPool* clipPool, const a3ui32 clipIndex_pool);

// update clip controller (Cameron Schneider)
a3i32 a3clipControllerUpdate(a3_ClipController* clipCtrl, const a3real dt);

// set clip to play (Scott Dagen)
a3i32 a3clipControllerSetClip(a3_ClipController* clipCtrl, const a3_ClipPool* clipPool, const a3ui32 clipIndex_pool);

// get a specified keyframe from the current clip
a3i32 a3clipControllerGetKeyframeFromIndex(a3_ClipController* clipCtrl, const a3ui32 index, a3_Keyframe* keyframe_out);

// get a specified clip from the current clip pool
a3i32 a3clipControllerGetClipFromIndex(a3_ClipController* clipCtrl, const a3ui32 index, a3_Clip* clip_out);

// evaluate the current value at time
a3i32 a3clipControllerEvaluate(a3_ClipController const* clipCtrl, a3_Sample* sample_out);


//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#include "_inl/a3_KeyframeAnimationController.inl"


#endif	// !__ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_H