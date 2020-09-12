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

/*
	Animation Framework Addons by Cameron Schneider and Scott Dagen
*/

#ifdef __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_H
#ifndef __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_INL
#define __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_INL

//-----------------------------------------------------------------------------

// update clip controller
inline a3i32 a3clipControllerUpdate(a3_ClipController* clipCtrl, const a3real dt)
{
	a3boolean resolved = false;

	// Pre-resolution
	a3real directionalDT = clipCtrl->playbackDir * dt * clipCtrl->speedMod;	// playbackDir is -1, 0, or 1, this moves time in the right direction. speedMod is a float in the range[0, infinity)
	clipCtrl->clipTime += directionalDT;
	clipCtrl->keyframeTime += directionalDT;

	//Resolution
	while (!resolved)
	{
		// The order of these cases matters.

		// Pause case
		if (directionalDT == 0.0f)	// This will always be 0 if the playbackDir is 0 (pause)
		{
			resolved = true;
		}
		else if (clipCtrl->clipTime >= clipCtrl->clipPool->clipArray[clipCtrl->clipIndex].duration)	// FORWARD: Playhead at or past clip end (if current clip time >= clip duration)
		{
			// NEEDS TRANSITION BEHAVIOR
			// Reset keyframeIndex to firstKeyframeIndex, use overstep to 'loop' to beginning, reset keyframeTime and clipTime accordingly

			a3real overstep = clipCtrl->clipTime - clipCtrl->clipPool->clipArray[clipCtrl->clipIndex].duration;

			a3_ClipTransition forwardTrans = clipCtrl->clipPool->clipArray[clipCtrl->clipIndex].forwardTransition;

			switch (forwardTrans.transition)
			{
			case a3clipTransitionPause:	  // Pause at the end
			{
				a3_Keyframe frame;	// This is a temporary variable, doesn't need to be a pointer
				a3clipControllerGetKeyframeFromIndex(clipCtrl, clipCtrl->keyframeIndex, &frame);

				clipCtrl->keyframeIndex = clipCtrl->clipPool->clipArray[clipCtrl->clipIndex].lastKeyframeIndex;
				clipCtrl->keyframeTime = frame.duration;
				clipCtrl->clipTime = clipCtrl->clipPool->clipArray[clipCtrl->clipIndex].duration;
				clipCtrl->playbackDir = 0;
				break;
			}
			case a3clipTransitionForwardFrame:	  // Forward pause at end of first frame, plays only first frame
				clipCtrl->delayedPause = true;	  // Set delayed pause so we play through the first frame, and pause later

			case a3clipTransitionForward:	// Keep playing from beginning, add overstep

				clipCtrl->clipPool = forwardTrans.targetClipPool;
				clipCtrl->clipIndex = forwardTrans.targetClipIndex;
				clipCtrl->keyframeIndex = clipCtrl->clipPool->clipArray[clipCtrl->clipIndex].firstKeyframeIndex;
				clipCtrl->keyframeTime = overstep;
				clipCtrl->clipTime = overstep;
				clipCtrl->playbackDir = 1;
				break;
			case a3clipTransitionForwardPause:	  // Pause at start of first frame

				clipCtrl->keyframeIndex = clipCtrl->clipPool->clipArray[clipCtrl->clipIndex].firstKeyframeIndex;
				clipCtrl->keyframeTime = 0.0f;
				clipCtrl->clipTime = 0.0f;
				clipCtrl->playbackDir = 0;
				break;
			case a3clipTransitionForwardSkip:	// Play from start of second frame
			{
				clipCtrl->clipPool = forwardTrans.targetClipPool;
				clipCtrl->clipIndex = forwardTrans.targetClipIndex;

				a3_Keyframe firstFrame, secondFrame;	// This is a temporary variable, doesn't need to be a pointer
				a3ui32 secondIndex = clipCtrl->clipPool->clipArray[clipCtrl->clipIndex].firstKeyframeIndex + 1;
				a3clipControllerGetKeyframeFromIndex(clipCtrl, secondIndex, &secondFrame);
				a3clipControllerGetKeyframeFromIndex(clipCtrl, clipCtrl->clipPool->clipArray[clipCtrl->clipIndex].firstKeyframeIndex, &firstFrame);

				clipCtrl->keyframeIndex = secondIndex;
				clipCtrl->keyframeTime = 0.0f;
				clipCtrl->clipTime = firstFrame.duration + overstep;
				clipCtrl->playbackDir = 1;
				break;
			}
			}

			resolved = true;
		}
		else if (clipCtrl->keyframeTime >= clipCtrl->clipPool->clipArray[clipCtrl->clipIndex].keyframes->keyframeArray[clipCtrl->keyframeIndex].duration)	// FORWARD: Playhead enters next keyframe (if current keyframeTime >= current keyframe duration)
		{
			// increment keyframeIndex, reset clipCtrl->keyframeTime to just 0.0f;

			if (clipCtrl->delayedPause)
			{
				clipCtrl->playbackDir = 0;
			}

			clipCtrl->keyframeIndex++;

			clipCtrl->keyframeTime = 0.0f;

			resolved = true;
		}
		else if (clipCtrl->clipTime < 0.0f)	// REVERSE: Playhead has reached/passed the beginning of the clip
		{
			// NEEDS TRANSITION BEHAVIOR
			// reset keyframeIndex to lastKeyframeIndex, use overstep to 'loop' to end, reset keyframeTime and clipTime accordingly

			a3real overstep = 0.0f - clipCtrl->clipTime; //positive because clipTime is negative
			a3_ClipTransition* revTransition = &clipCtrl->clipPool->clipArray[clipCtrl->clipIndex].reverseTransition;
			switch (revTransition->transition)
			{
			case a3clipTransitionPause:
			{
				a3_Keyframe frame;	// This is a temporary variable, doesn't need to be a pointer
				a3clipControllerGetKeyframeFromIndex(clipCtrl, clipCtrl->keyframeIndex, &frame);

				clipCtrl->keyframeIndex = clipCtrl->clipPool->clipArray[clipCtrl->clipIndex].firstKeyframeIndex;
				clipCtrl->keyframeTime = 0;
				clipCtrl->clipTime = 0;
				clipCtrl->playbackDir = 0;
				break;
			}
			case a3clipTransitionReverseFrame:
				clipCtrl->delayedPause = true;
			case a3clipTransitionReverse:
			{
				clipCtrl->clipPool = revTransition->targetClipPool;
				clipCtrl->clipIndex = revTransition->targetClipIndex;
				clipCtrl->keyframeIndex = clipCtrl->clipPool->clipArray[clipCtrl->clipIndex].lastKeyframeIndex;
				a3_Keyframe frame;    // This is a temporary variable, doesn't need to be a pointer
				a3clipControllerGetKeyframeFromIndex(clipCtrl, clipCtrl->keyframeIndex, &frame);
				clipCtrl->keyframeTime = frame.duration - overstep;
				clipCtrl->clipTime = clipCtrl->clipPool->clipArray[clipCtrl->clipIndex].duration - overstep;
				clipCtrl->playbackDir = -1;
				break;
			}
			case a3clipTransitionReversePause:
			{
				clipCtrl->clipPool = revTransition->targetClipPool;
				clipCtrl->clipIndex = revTransition->targetClipIndex;
				clipCtrl->keyframeIndex = clipCtrl->clipPool->clipArray[clipCtrl->clipIndex].lastKeyframeIndex;
				a3_Keyframe frame;    // This is a temporary variable, doesn't need to be a pointer
				a3clipControllerGetKeyframeFromIndex(clipCtrl, clipCtrl->keyframeIndex, &frame);
				clipCtrl->keyframeTime = frame.duration;
				clipCtrl->clipTime = clipCtrl->clipPool->clipArray[clipCtrl->clipIndex].duration;
				clipCtrl->playbackDir = 0;
				break;
			}
			case a3clipTransitionReverseSkip:
			{
				clipCtrl->clipPool = revTransition->targetClipPool;
				clipCtrl->clipIndex = revTransition->targetClipIndex;
				clipCtrl->keyframeIndex = clipCtrl->clipPool->clipArray[clipCtrl->clipIndex].lastKeyframeIndex - 1;
				a3_Keyframe frame;    // This is a temporary variable, doesn't need to be a pointer
				a3clipControllerGetKeyframeFromIndex(clipCtrl, clipCtrl->keyframeIndex, &frame);
				clipCtrl->keyframeTime = frame.duration - overstep;
				clipCtrl->clipTime = clipCtrl->clipPool->clipArray[clipCtrl->clipIndex].duration;
				clipCtrl->playbackDir = 0;
				break;
			}
			}

			resolved = true;
		}
		else if (clipCtrl->keyframeTime < 0.0f)	// REVERSE: Playhead enters previous keyframe
		{
			// decrement keyframeIndex, reset clipCtrl->keyframeTime to the new keyframe's duration

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


inline a3i32 a3clipControllerGetKeyframeFromIndex(a3_ClipController* clipCtrl, const a3ui32 index, a3_Keyframe* keyframe_out)
{
	if (index >= clipCtrl->clipPool->clipArray[clipCtrl->clipIndex].firstKeyframeIndex && index <= clipCtrl->clipPool->clipArray[clipCtrl->clipIndex].lastKeyframeIndex)
	{
		*keyframe_out = clipCtrl->clipPool->clipArray[clipCtrl->clipIndex].keyframes->keyframeArray[index];
		return 1;
	}
	else
	{
		// specified index not in current clip's range
		return -1;
	}
}

inline a3i32 a3clipControllerGetClipFromIndex(a3_ClipController* clipCtrl, const a3ui32 index, a3_Clip* clip_out)
{
	if (index >= 0 && index < clipCtrl->clipPool->count)
	{
		*clip_out = clipCtrl->clipPool->clipArray[index];
		return 1;
	}
	else
	{
		// invalid index
		return -1;
	}
}


//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_INL
#endif	// __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_H