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
	inline definitions for keyframe animation controller.
*/

#ifdef __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_H
#ifndef __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_INL
#define __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_INL


//-----------------------------------------------------------------------------

// update clip controller
inline a3i32 a3clipControllerUpdate(a3_ClipController* clipCtrl, const a3real dt)
{
	 //https://stackoverflow.com/questions/1608318/is-bool-a-native-c-type
	_Bool resolved = false;	// 'bool' didn't work, probably cause it's actually an STD macro

	// Pre-resolution
	a3real directionalDT = clipCtrl->playbackDir * dt * clipCtrl->speedMod;	// playbackDir is -1, 0, or 1, this moves time in the right direction. speedMod is a float in the range[0, infinity)
	clipCtrl->clipTime += directionalDT;
	clipCtrl->keyframeTime += directionalDT;

	//Resolution
	while (!resolved)
	{
		// The order of these cases matters.

		// Pause case
		if (dt == 0.0f)
		{
			resolved = true;
		}
		else if (clipCtrl->clipTime >= clipCtrl->clipPool->clipArray[clipCtrl->clipIndex].duration)	// FORWARD: Playhead at or past clip end (if current clip time >= clip duration)
		{
			// Might need to do other work here, like cache the difference/overstep and handle 'looping' to beginning
			// Reset keyframeIndex to firstKeyframeIndex, use overstep to 'loop' to beginning, reset keyframeTime and clipTime accordingly

			a3real overstep = clipCtrl->clipTime - clipCtrl->clipPool->clipArray[clipCtrl->clipIndex].duration;

			clipCtrl->keyframeIndex = clipCtrl->clipPool->clipArray[clipCtrl->clipIndex].firstKeyframeIndex;
			clipCtrl->keyframeTime = overstep;
			clipCtrl->clipTime = overstep;

			resolved = true;
		}
		else if (clipCtrl->keyframeTime >= clipCtrl->clipPool->clipArray[clipCtrl->clipIndex].keyframes->keyframeArray[clipCtrl->keyframeIndex].duration)	// FORWARD: Playhead enters next keyframe (if current keyframeTime >= current keyframe duration)
		{
			// Might need to do other stuff here, not sure what
			// increment keyframeIndex, reset clipCtrl->keyframeTime to 0.0f + overstep? or just 0.0f;

			clipCtrl->keyframeIndex++;
			
			clipCtrl->keyframeTime = 0.0f;

			resolved = true;
		}
		else if (clipCtrl->clipTime < 0.0f)	// REVERSE: Playhead has reached/passed the beginning of the clip
		{
			// Might need to do other work here, like cache the difference/overstep and handle 'looping' to end
			// reset keyframeIndex to lastKeyframeIndex, use overstep to 'loop' to end, reset keyframeTime and clipTime accordingly

			a3real overstep = 0.0f - clipCtrl->clipTime;

			clipCtrl->keyframeIndex = clipCtrl->clipPool->clipArray[clipCtrl->clipIndex].lastKeyframeIndex;
			clipCtrl->keyframeTime = clipCtrl->clipPool->clipArray[clipCtrl->clipIndex].keyframes->keyframeArray[clipCtrl->keyframeIndex].duration - overstep;
			clipCtrl->clipTime = clipCtrl->clipPool->clipArray[clipCtrl->clipIndex].duration - overstep;

			resolved = true;
		}
		else if (clipCtrl->keyframeTime < 0.0f)	// REVERSE: Playhead enters previous keyframe
		{
			// Might need to do other stuff here, not sure what
			// decrement keyframeIndex, reset clipCtrl->keyframeTime to the new keyframe's duration - overstep? or just duration

			clipCtrl->keyframeIndex--;
			clipCtrl->keyframeTime = clipCtrl->clipPool->clipArray[clipCtrl->clipIndex].keyframes->keyframeArray[clipCtrl->keyframeIndex].duration;

			resolved = true;
		}
		else if (directionalDT < 0.0f)	// REVERSE: Playhead moving backward in time, but not to next keyframe
		{
			resolved = true;
		}
		else if (directionalDT > 0.0f)	// FORWARD: Playhead moving forward in time, but not to next keyframe
		{
			resolved = true;
		}
	}

	// Post-resolution (normalize parameters)
	clipCtrl->clipParameter = clipCtrl->clipTime * clipCtrl->clipPool->clipArray[clipCtrl->clipIndex].durInv;
	clipCtrl->keyframeParameter = clipCtrl->keyframeTime * clipCtrl->clipPool->clipArray[clipCtrl->clipIndex].keyframes->keyframeArray[clipCtrl->keyframeIndex].durInv;


	return 1;
}

// set clip to play
inline a3i32 a3clipControllerSetClip(a3_ClipController* clipCtrl, const a3_ClipPool* clipPool, const a3ui32 clipIndex_pool)
{
	clipCtrl->clipPool = clipPool;
	clipCtrl->clipIndex = clipIndex_pool;

	clipCtrl->clipTime = 0;
	clipCtrl->clipParameter = 0;

	//set keyframe info, including grabbing the first keyframe index
	clipCtrl->keyframeIndex = clipCtrl->clipPool->clipArray[clipCtrl->clipIndex].firstKeyframeIndex;
	clipCtrl->keyframeTime = 0;
	clipCtrl->keyframeParameter = 0;

	clipCtrl->playbackDir = 0;
	return -1;
}


//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_INL
#endif	// __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_H