/*
	Animation Framework Addons by Scott Dagen and Cameron Schneider
	a3_HierarchyBlendNode.h: implementations for blend trees and blend nodes.
*/

#include "..\a3_HierarchyBlendNode.h"
#include <stdlib.h>
#include <string.h>
#include "..\a3_HierarchyStateBlend.h"
#include "..\..\a3_DemoState.h"

/// <summary>
/// Identity
/// </summary>
/// <param name="node_inout"></param>
void a3hierarchyBlendExec0C(a3_HierarchyBlendNode* node_inout)
{
	typedef void (*a3_HierarchyBlendOp0C)(a3_HierarchyPose* pose, a3ui32 numNodes); //function pointer to a node op
	a3_HierarchyBlendOp0C op = (a3_HierarchyBlendOp0C)node_inout->operation;
	op(node_inout->state_out->animPose,
		node_inout->state_out->hierarchy->numNodes);
}

/// <summary>
/// Copy, Const, Negate, Concat, Convert, Revert
/// </summary>
/// <param name="node_inout"></param>
void a3hierarchyBlendExec1C(a3_HierarchyBlendNode* node_inout)
{
	typedef void (*a3_HierarchyBlendOp1C)(a3_HierarchyPose* pose, a3_HierarchyPose* control0, a3ui32 numNodes); //function pointer to a node op
	a3_HierarchyBlendOp1C op = (a3_HierarchyBlendOp1C)node_inout->operation;
	op(node_inout->state_out->animPose,
		node_inout->controlStates[0]->animPose,
		node_inout->state_out->hierarchy->numNodes);
}

/// <summary>
/// Scale, Bidirectional scale
/// </summary>
/// <param name="node_inout"></param>
void a3hierarchyBlendExec1C1I(a3_HierarchyBlendNode* node_inout)
{
	typedef void (*a3_HierarchyBlendOp1C1I)(a3_HierarchyPose* pose, a3_HierarchyPose* control0, a3f32 uVal0, a3ui32 numNodes); //function pointer to a node op
	a3_HierarchyBlendOp1C1I op = (a3_HierarchyBlendOp1C1I)node_inout->operation;
	float* uVal = node_inout->uVals[0];
	op(node_inout->state_out->animPose,
		node_inout->controlStates[0]->animPose,
		*uVal,
		node_inout->state_out->hierarchy->numNodes);
}

/// <summary>
/// Deconcat
/// </summary>
/// <param name="node_inout"></param>
void a3hierarchyBlendExec2C(a3_HierarchyBlendNode* node_inout)
{
	typedef void (*a3_HierarchyBlendOp2C)(a3_HierarchyPose* pose, a3_HierarchyPose* control0, a3_HierarchyPose* control1, a3ui32 numNodes); //function pointer to a node op
	a3_HierarchyBlendOp2C op = (a3_HierarchyBlendOp2C)node_inout->operation;
	op(node_inout->state_out->animPose,
		node_inout->controlStates[0]->animPose,
		node_inout->controlStates[1]->animPose,
		node_inout->state_out->hierarchy->numNodes);
}

/// <summary>
/// Nearest, LERP, EaseInOut
/// </summary>
/// <param name="node_inout"></param>
void a3hierarchyBlendExec2C1I(a3_HierarchyBlendNode* node_inout)
{
	typedef void (*a3_HierarchyBlendOp2C1I)(a3_HierarchyPose* pose,
		a3_HierarchyPose* control0, a3_HierarchyPose* control1, a3f32 uVal0, a3ui32 numNodes); //function pointer to a node op
	a3_HierarchyBlendOp2C1I op = (a3_HierarchyBlendOp2C1I)node_inout->operation;
	float* uVal0 = node_inout->uVals[0];
	op(node_inout->state_out->animPose,
		node_inout->controlStates[0]->animPose,
		node_inout->controlStates[1]->animPose,
		*uVal0,
		node_inout->state_out->hierarchy->numNodes);
}

/// <summary>
/// Triangular
/// </summary>
/// <param name="node_inout"></param>
void a3hierarchyBlendExec3C2I(a3_HierarchyBlendNode* node_inout)
{
	typedef void (*a3_HierarchyBlendOp3C2I)(a3_HierarchyPose* pose,
		a3_HierarchyPose* control0, a3_HierarchyPose* control1, a3_HierarchyPose* control2,
		a3f32 uVal0, a3f32 uVal1,
		a3ui32 numNodes); //function pointer to a node op
	a3_HierarchyBlendOp3C2I op = (a3_HierarchyBlendOp3C2I)node_inout->operation;
	float* uVal0 = node_inout->uVals[0];
	float* uVal1 = node_inout->uVals[1];
	op(node_inout->state_out->animPose,
		node_inout->controlStates[0]->animPose,
		node_inout->controlStates[1]->animPose,
		node_inout->controlStates[2]->animPose,
		*uVal0, *uVal1,
		node_inout->state_out->hierarchy->numNodes);
}

/// <summary>
/// Cubic
/// </summary>
/// <param name="node_inout"></param>
void a3hierarchyBlendExec4C1I(a3_HierarchyBlendNode* node_inout)
{
	typedef void (*a3_HierarchyBlendOp4C1I)(a3_HierarchyPose* pose,
		a3_HierarchyPose* control0, a3_HierarchyPose* control1, a3_HierarchyPose* control2, a3_HierarchyPose* control3,
		a3f32 uVal0,
		a3ui32 numNodes); //function pointer to a node op

	a3_HierarchyBlendOp4C1I op = (a3_HierarchyBlendOp4C1I)node_inout->operation;
	float* uVal0 = node_inout->uVals[0];

	op(node_inout->state_out->animPose,
		node_inout->controlStates[0]->animPose,
		node_inout->controlStates[1]->animPose,
		node_inout->controlStates[2]->animPose,
		node_inout->controlStates[3]->animPose,
		*uVal0,
		node_inout->state_out->hierarchy->numNodes);
}

/// <summary>
/// Binearest, Bilinear
/// </summary>
/// <param name="node_inout"></param>
void a3hierarchyBlendExec4C3I(a3_HierarchyBlendNode* node_inout)
{
	typedef void (*a3_HierarchyBlendOp4C3I)(a3_HierarchyPose* pose,
		a3_HierarchyPose* control0, a3_HierarchyPose* control1, a3_HierarchyPose* control2, a3_HierarchyPose* control3,
		a3f32 uVal0, a3f32 uVal1, a3f32 uVal2,
		a3ui32 numNodes); //function pointer to a node op

	a3_HierarchyBlendOp4C3I op = (a3_HierarchyBlendOp4C3I)node_inout->operation;
	float* uVal0 = node_inout->uVals[0];
	float* uVal1 = node_inout->uVals[1];
	float* uVal2 = node_inout->uVals[2];
	op(node_inout->state_out->animPose,
		node_inout->controlStates[0]->animPose,
		node_inout->controlStates[1]->animPose,
		node_inout->controlStates[2]->animPose,
		node_inout->controlStates[3]->animPose,
		*uVal0, *uVal1, *uVal2,
		node_inout->state_out->hierarchy->numNodes);
}

/// <summary>
/// Bicubic
/// </summary>
/// <param name="node_inout"></param>
void a3hierarchyBlendExec16C5I(a3_HierarchyBlendNode* node_inout)
{
	typedef void (*a3_HierarchyBlendOp16C5I)(a3_HierarchyPose* pose,
		a3_HierarchyPose* control00, a3_HierarchyPose* control01, a3_HierarchyPose* control02, a3_HierarchyPose* control03,
		a3_HierarchyPose* control10, a3_HierarchyPose* control11, a3_HierarchyPose* control12, a3_HierarchyPose* control13,
		a3_HierarchyPose* control20, a3_HierarchyPose* control21, a3_HierarchyPose* control22, a3_HierarchyPose* control23,
		a3_HierarchyPose* control30, a3_HierarchyPose* control31, a3_HierarchyPose* control32, a3_HierarchyPose* control33,
		a3f32 uVal0, a3f32 uVal1, a3f32 uVal2, a3f32 uVal3, a3f32 uVal4, a3ui32 numNodes); //function pointer to a node op

	float* uVal0 = node_inout->uVals[0];
	float* uVal1 = node_inout->uVals[1];
	float* uVal2 = node_inout->uVals[2];
	float* uVal3 = node_inout->uVals[3];
	float* uVal4 = node_inout->uVals[4];

	a3_HierarchyBlendOp16C5I op = (a3_HierarchyBlendOp16C5I)node_inout->operation;
	op(node_inout->state_out->animPose,
		node_inout->controlStates[0 * 4 + 0]->animPose, node_inout->controlStates[0 * 4 + 1]->animPose, node_inout->controlStates[0 * 4 + 2]->animPose, node_inout->controlStates[0 * 4 + 3]->animPose,
		node_inout->controlStates[1 * 4 + 0]->animPose, node_inout->controlStates[1 * 4 + 1]->animPose, node_inout->controlStates[1 * 4 + 2]->animPose, node_inout->controlStates[1 * 4 + 3]->animPose,
		node_inout->controlStates[2 * 4 + 0]->animPose, node_inout->controlStates[2 * 4 + 1]->animPose, node_inout->controlStates[2 * 4 + 2]->animPose, node_inout->controlStates[2 * 4 + 3]->animPose,
		node_inout->controlStates[3 * 4 + 0]->animPose, node_inout->controlStates[3 * 4 + 1]->animPose, node_inout->controlStates[3 * 4 + 2]->animPose, node_inout->controlStates[3 * 4 + 3]->animPose,
		*uVal0, *uVal1, *uVal2, *uVal3, *uVal4,
		node_inout->state_out->hierarchy->numNodes);
}

/// <summary>
/// Negate, Copy
/// </summary>
/// <param name="node_inout"></param>
void a3hierarchyClipExec1C(a3_HierarchyBlendNode* node_inout)
{
	typedef void (*a3_ClipOp1C)(a3_HierarchyPose* pose, a3_HierarchyPoseGroup* poseGroup, a3_ClipController* c0); //function pointer to a node op
	a3_ClipOp1C op = (a3_ClipOp1C)node_inout->operation;
	a3clipOpSampleClip(node_inout->controlStates[0]->animPose, node_inout->poseGroup, node_inout->clipControllers[0]);
	op(node_inout->state_out->animPose,
		node_inout->poseGroup,
		node_inout->clipControllers[0]);
}

/// <summary>
/// Scale
/// </summary>
/// <param name="node_inout"></param>
void a3hierarchyClipExec1C1I(a3_HierarchyBlendNode* node_inout)
{
	typedef void (*a3_ClipOp1C1I)(a3_HierarchyPose* pose, a3_HierarchyPoseGroup* poseGroup, a3_ClipController* c0, a3f32 uVal); //function pointer to a node op
	a3_ClipOp1C1I op = (a3_ClipOp1C1I)node_inout->operation;
	float* uVal = node_inout->uVals[0];
	a3clipOpSampleClip(node_inout->controlStates[0]->animPose, node_inout->poseGroup, node_inout->clipControllers[0]);
	op(node_inout->state_out->animPose,
		node_inout->poseGroup,
		node_inout->clipControllers[0],
		*uVal);
}

/// <summary>
/// Add
/// </summary>
/// <param name="node_inout"></param>
void a3hierarchyClipExec2C(a3_HierarchyBlendNode* node_inout)
{
	typedef void (*a3_ClipOp2C)(a3_HierarchyPose* pose, a3_HierarchyPoseGroup* poseGroup, a3_ClipController* c0, a3_ClipController* c1); //function pointer to a node op
	a3_ClipOp2C op = (a3_ClipOp2C)node_inout->operation;
	a3clipOpSampleClip(node_inout->controlStates[0]->animPose, node_inout->poseGroup, node_inout->clipControllers[0]);
	a3clipOpSampleClip(node_inout->controlStates[1]->animPose, node_inout->poseGroup, node_inout->clipControllers[1]);
	op(node_inout->state_out->animPose,
		node_inout->poseGroup,
		node_inout->clipControllers[0],
		node_inout->clipControllers[1]);
}

/// <summary>
/// Lerp
/// </summary>
/// <param name="node_inout"></param>
void a3hierarchyClipExec2C1I(a3_HierarchyBlendNode* node_inout)
{
	typedef void (*a3_ClipOp2C1I)(a3_HierarchyPose* pose, a3_HierarchyPoseGroup* poseGroup, a3_ClipController* c0, a3_ClipController* c1, a3f32 uVal); //function pointer to a node op
	a3_ClipOp2C1I op = (a3_ClipOp2C1I)node_inout->operation;
	float* uVal0 = node_inout->uVals[0];
	a3clipOpSampleClip(node_inout->controlStates[0]->animPose, node_inout->poseGroup, node_inout->clipControllers[0]);
	a3clipOpSampleClip(node_inout->controlStates[1]->animPose, node_inout->poseGroup, node_inout->clipControllers[1]);
	op(node_inout->state_out->animPose,
		node_inout->poseGroup,
		node_inout->clipControllers[0],
		node_inout->clipControllers[1],
		*uVal0);
}

/// <summary>
/// Create blend node given a LOT of data
/// </summary>
/// <param name="refTree"></param>
/// <param name="blendNode_out"></param>
/// <param name="type"></param>
/// <param name="clipCount"></param>
/// <param name="clipNames"></param>
/// <param name="controlNodeCount"></param>
/// <param name="controlNodes"></param>
/// <param name="inputCount"></param>
/// <param name="uValPtrs"></param>
/// <returns></returns>
a3i32 a3hierarchyBlendNodeCreateInternal(a3_HierarchyBlendTree* refTree, a3_HierarchyBlendNode* blendNode_out, NodeType type,
	a3ui32 clipCount, a3byte** clipNames,
	a3ui32 controlNodeCount, a3ui32* controlNodes,
	a3ui32 inputCount, a3f32** uValPtrs)
{
	blendNode_out->nodeType = type;

	//names of source clips
	for (a3ui32 i = 0; i < clipCount; i++)
	{
		blendNode_out->clipNames[i] = clipNames[i];
	}
	blendNode_out->clipCount = clipCount;
	for (a3ui32 i = 0; i < inputCount; i++) //pointers to u vals. Useful for character control
	{
		blendNode_out->uVals[i] = uValPtrs[i];
	}
	blendNode_out->uValCount = inputCount;
	for (a3ui32 i = 0; i < controlNodeCount; i++)
	{
		//this information will be needed in binding. Establishes a list of children technically
		blendNode_out->controlNodeIndices[i] = controlNodes[i];
	}
	blendNode_out->controlNodeCount = controlNodeCount;

	switch (type)
	{
	case addClip:
		blendNode_out->exec = &a3hierarchyClipExec2C;
		blendNode_out->operation = &a3clipOpAdd;
		break;
	case lerpClip:
		blendNode_out->exec = &a3hierarchyClipExec2C1I;
		blendNode_out->operation = &a3clipOpLerp;
		break;
	case scaleClip:
		blendNode_out->exec = &a3hierarchyClipExec1C1I;
		blendNode_out->operation = &a3clipOpScale;
		break;
	case negateClip:
		blendNode_out->exec = &a3hierarchyClipExec1C;
		blendNode_out->operation = &a3clipOpNegate;
		break;
	case copyClip:
		blendNode_out->exec = &a3hierarchyClipExec1C;
		blendNode_out->operation = &a3clipOpSampleClip;
		break;
	case identity:
	case init: //too hard
		blendNode_out->exec = &a3hierarchyBlendExec0C;
		blendNode_out->operation = &a3hierarchyPoseOpIdentity;
		break;
	case copy:
		blendNode_out->exec = &a3hierarchyBlendExec1C;
		blendNode_out->operation = &a3hierarchyPoseOpCopy;
		break;
	case negate:
		blendNode_out->exec = &a3hierarchyBlendExec1C;
		blendNode_out->operation = &a3hierarchyPoseOpNegate;
		break;
	case concat:
		blendNode_out->exec = &a3hierarchyBlendExec1C;
		blendNode_out->operation = &a3hierarchyPoseOpConcat;
		break;
	case convert:
		blendNode_out->exec = &a3hierarchyBlendExec1C;
		blendNode_out->operation = &a3hierarchyPoseOpConvert;
		break;
	case revert:
		blendNode_out->exec = &a3hierarchyBlendExec1C;
		blendNode_out->operation = &a3hierarchyPoseOpRevert;
		break;
	case scale:
		blendNode_out->exec = &a3hierarchyBlendExec1C1I;
		blendNode_out->operation = &a3hierarchyPoseOpScale;
		break;
	case biscale:
		blendNode_out->exec = &a3hierarchyBlendExec1C1I;
		blendNode_out->operation = &a3hierarchyPoseOpBiDirectionalScale;
		break;
	case deconcat:
		blendNode_out->exec = &a3hierarchyBlendExec2C;
		blendNode_out->operation = &a3hierarchyPoseOpDeconcat;
		break;
	case nearest:
		blendNode_out->exec = &a3hierarchyBlendExec2C1I;
		blendNode_out->operation = &a3hierarchyPoseOpNearest;
		break;
	case lerp:
		blendNode_out->exec = &a3hierarchyBlendExec2C1I;
		blendNode_out->operation = &a3hierarchyPoseOpLERP;
		break;
	case easeinout:
		blendNode_out->exec = &a3hierarchyBlendExec2C1I;
		blendNode_out->operation = &a3hierarchyPoseOpEaseInOut;
		break;
	case triangular:
		blendNode_out->exec = &a3hierarchyBlendExec3C2I;
		blendNode_out->operation = &a3hierarchyPoseOpTriangularLERP;
		break;
	case cubic:
		blendNode_out->exec = &a3hierarchyBlendExec4C1I;
		blendNode_out->operation = &a3hierarchyPoseOpCubic;
		break;
	case binearest:
		blendNode_out->exec = &a3hierarchyBlendExec4C3I;
		blendNode_out->operation = &a3hierarchyPoseOpBiNearest;
		break;
	case bilerp:
		blendNode_out->exec = &a3hierarchyBlendExec4C3I;
		blendNode_out->operation = &a3hierarchyPoseOpBiLerp;
		break;
	case bicubic:
		blendNode_out->exec = &a3hierarchyBlendExec16C5I;
		blendNode_out->operation = &a3hierarchyPoseOpBiCubic;
		break;
		//case fk:
		//	blendNode_out->exec = &a3hierarchyBlendExec1C;
		//	blendNode_out->operation = NULL;
		//	break;
		//case ik:
		//	blendNode_out->exec = &a3hierarchyBlendExec1C;
		//	blendNode_out->operation = NULL; //this operation needs to call copy before doing anything
		//	break;
	default:
		blendNode_out->exec = &a3hierarchyBlendExec0C;
		blendNode_out->operation = &a3hierarchyPoseOpIdentity;
		break;
	}
	return 1;
}

a3_HierarchyBlendExec a3hierarchyBlendNodeGetExec(NodeType nodeType)
{
	switch (nodeType)
	{
	case addClip:
		return &a3hierarchyClipExec2C;
	case lerpClip:
		return &a3hierarchyClipExec2C1I;
	case scaleClip:
		return &a3hierarchyClipExec1C1I;
	case negateClip:
	case copyClip:
		return &a3hierarchyClipExec1C;
	case copy:
	case negate:
	case concat:
	case convert:
	case revert:
		return &a3hierarchyBlendExec1C;
	case scale:
	case biscale:
		return &a3hierarchyBlendExec1C1I;
	case deconcat:
		return &a3hierarchyBlendExec2C;
	case nearest:
	case lerp:
	case easeinout:
		return &a3hierarchyBlendExec2C1I;
	case triangular:
		return &a3hierarchyBlendExec3C2I;
	case cubic:
		return &a3hierarchyBlendExec4C1I;
	case binearest:
		return &a3hierarchyBlendExec4C3I;
	case bilerp:
		return &a3hierarchyBlendExec4C3I;
	case bicubic:
		return &a3hierarchyBlendExec16C5I;
	default:
		return &a3hierarchyBlendExec0C;
	}
}

a3i32 a3hierarchyBlendNodeCreate(a3_HierarchyBlendNode* blendNode_out, NodeType nodeType)
{
	if (blendNode_out)
	{
		blendNode_out->exec = a3hierarchyBlendNodeGetExec(nodeType);
		switch (nodeType)
		{
		case addClip:
			blendNode_out->operation = &a3clipOpAdd;
			blendNode_out->clipCount = 2; //merge with controlCount, maybe even do a union of clips and hStates????
			blendNode_out->uValCount = 0;
		case lerpClip:
			blendNode_out->operation = &a3clipOpLerp;
			blendNode_out->clipCount = 2;
			blendNode_out->uValCount = 1;
			break;
		case scaleClip:
			blendNode_out->operation = &a3clipOpScale;
			blendNode_out->clipCount = 1;
			blendNode_out->uValCount = 1;
			break;
		case negateClip:
			blendNode_out->operation = &a3clipOpNegate;
			blendNode_out->clipCount = 1;
			blendNode_out->uValCount = 0;
			break;
		case copyClip:
			blendNode_out->operation = &a3clipOpSampleClip;
			blendNode_out->clipCount = 1;
			blendNode_out->uValCount = 0;
			break;
		case identity:
		case init: //too hard
			blendNode_out->operation = &a3hierarchyPoseOpIdentity;
			blendNode_out->controlNodeCount = 0;
			blendNode_out->uValCount = 0;
			break;
		case copy:
			blendNode_out->operation = &a3hierarchyPoseOpCopy;
			blendNode_out->controlNodeCount = 1;
			blendNode_out->uValCount = 0;
			break;
		case negate:
			blendNode_out->operation = &a3hierarchyPoseOpNegate;
			blendNode_out->controlNodeCount = 1;
			blendNode_out->uValCount = 0;
			break;
		case concat:
			blendNode_out->operation = &a3hierarchyPoseOpConcat;
			blendNode_out->controlNodeCount = 1;
			blendNode_out->uValCount = 0;
			break;
		case convert:
			blendNode_out->operation = &a3hierarchyPoseOpConvert;
			blendNode_out->controlNodeCount = 1;
			blendNode_out->uValCount = 0;
			break;
		case revert:
			blendNode_out->operation = &a3hierarchyPoseOpRevert;
			blendNode_out->controlNodeCount = 1;
			blendNode_out->uValCount = 0;
			break;
		case scale:
			blendNode_out->operation = &a3hierarchyPoseOpScale;
			blendNode_out->controlNodeCount = 1;
			blendNode_out->uValCount = 1;
			break;
		case biscale:
			blendNode_out->operation = &a3hierarchyPoseOpBiDirectionalScale;
			blendNode_out->controlNodeCount = 1;
			blendNode_out->uValCount = 1;
			break;
		case deconcat:
			blendNode_out->operation = &a3hierarchyPoseOpDeconcat;
			blendNode_out->controlNodeCount = 2;
			blendNode_out->uValCount = 0;
			break;
		case nearest:
			blendNode_out->operation = &a3hierarchyPoseOpNearest;
			blendNode_out->controlNodeCount = 2;
			blendNode_out->uValCount = 1;
			break;
		case lerp:
			blendNode_out->operation = &a3hierarchyPoseOpLERP;
			blendNode_out->controlNodeCount = 2;
			blendNode_out->uValCount = 1;
			break;
		case easeinout:
			blendNode_out->operation = &a3hierarchyPoseOpEaseInOut;
			blendNode_out->controlNodeCount = 2;
			blendNode_out->uValCount = 1;
			break;
		case triangular:
			blendNode_out->operation = &a3hierarchyPoseOpTriangularLERP;
			blendNode_out->controlNodeCount = 3;
			blendNode_out->uValCount = 2;
			break;
		case cubic:
			blendNode_out->operation = &a3hierarchyPoseOpCubic;
			blendNode_out->controlNodeCount = 4;
			blendNode_out->uValCount = 1;
			break;
		case binearest:
			blendNode_out->operation = &a3hierarchyPoseOpBiNearest;
			blendNode_out->controlNodeCount = 4;
			blendNode_out->uValCount = 3;
			break;
		case bilerp:
			blendNode_out->operation = &a3hierarchyPoseOpBiLerp;
			blendNode_out->controlNodeCount = 4;
			blendNode_out->uValCount = 3;
			break;
		case bicubic:
			blendNode_out->operation = &a3hierarchyPoseOpBiCubic;
			blendNode_out->controlNodeCount = 16;
			blendNode_out->uValCount = 5;
			break;
		default:
			blendNode_out->operation = &a3hierarchyPoseOpIdentity;
			blendNode_out->controlNodeCount = 0;
			blendNode_out->uValCount = 0;
			break;
		}

		if (blendNode_out->nodeType <= copyClip)
		{
			for (a3ui32 i = 0; i < blendNode_out->clipCount; i++) //set all controllers to null
			{
				blendNode_out->clipControllers[i] = NULL;
			}
		}
		return -1;
	}
	return -1;
}

a3i32 a3hierarchyBlendNodeGenControllersWithClipNames(a3_HierarchyBlendNode* node, a3i32 ctrlCount, char** ctrlNames, char** clipNames, a3_ClipPool* pool, const a3i32 playback_step, const a3f64 playback_stepPerSec)
{
	if (node && node->clipCount == ctrlCount && pool && ctrlNames && clipNames && node->nodeType <= copyClip)
	{
		a3_ClipController* ctrl = malloc(sizeof(a3_ClipController) * ctrlCount);
		if (ctrl)
		{
			for (int i = 0; i < ctrlCount; i++)
			{
				if (clipNames[i] && ctrlNames[i])
				{
					a3i32 index = a3clipGetIndexInPool(pool, clipNames[i]);
					a3clipControllerInit(ctrl + i, ctrlNames[i], pool, index, playback_step, playback_stepPerSec);
					node->clipControllers[i] = ctrl + i;
				}
			}
			return 1;
		}
	}
	return -1;
}

a3i32 a3hierarchyBlendNodeGenControllersWithClipIndices(a3_HierarchyBlendNode* node, a3i32 ctrlCount, char** ctrlNames, a3i32* clipIndices, a3_ClipPool* pool, const a3i32 playback_step, const a3f64 playback_stepPerSec)
{
	if (node && node->clipCount == ctrlCount && pool && ctrlNames && clipIndices && node->nodeType <= copyClip)
	{
		a3_ClipController* ctrl = malloc(sizeof(a3_ClipController) * ctrlCount);
		if (ctrl)
		{
			for (int i = 0; i < ctrlCount; i++)
			{
				if (ctrlNames[i] && clipIndices[i] >= 0)
				{
					a3clipControllerInit(ctrl + i, ctrlNames[i], pool, clipIndices[i], playback_step, playback_stepPerSec);
					node->clipControllers[i] = ctrl + i;
				}
			}
			return 1;
		}
	}
	return -1;
}

a3i32 a3hierarchyBlendNodeBindClipController(a3_HierarchyBlendNode* node, a3_ClipController* ctrl, a3ui32 index)
{
	if (node && node->clipCount > index && node->nodeType <= copyClip)
	{
		node->clipControllers[index] = ctrl;
		return 1;
	}
	return -1;
}

a3i32 a3hierarchyBlendNodeAddControl(a3_HierarchyBlendNode* node, a3ui32 slot, a3f32* controlSource)
{
	if (node && controlSource && node->uValCount > slot)
	{
		node->uVals[slot] = controlSource;
		return 1;
	}
	return -1;
}

a3i32 a3hierarchyBlendTreeCreate(a3_HierarchyBlendTree* blendTree_out, int size, a3boolean allocateNodes)
{
	if (blendTree_out && !blendTree_out->blendNodes && !blendTree_out->tree)
	{
		blendTree_out->tree = malloc(sizeof(a3_TreeNode));
		a3TreeNodeInit(blendTree_out->tree, size);
		blendTree_out->blendNodes = malloc(sizeof(a3_HierarchyBlendNode*) * size);
		blendTree_out->maxNodes = size;
		blendTree_out->nodeCount = 0;
		if (!blendTree_out->blendNodes)
		{
			return -2;
		}
		if (allocateNodes)
		{
			for (int i = 0; i < size; i++)
			{
				blendTree_out->blendNodes[i] = malloc(sizeof(a3_HierarchyBlendNode));
			}
			blendTree_out->nodeCount = size;
		}
		return 1;
	}
	return -1;
}

a3i32 a3hierarchyBlendTreeStoreNode(a3_HierarchyBlendTree* tree, a3_HierarchyBlendNode* node)
{
	if (tree && node && tree->blendNodes && tree->nodeCount < tree->maxNodes)
	{
		tree->blendNodes[tree->nodeCount] = node;
		tree->nodeCount++;
		return 1;
	}
	return -1;
}

a3i32 a3hierarchyBlendTreeAddNodeToTree(a3_HierarchyBlendTree* blendTree, int nodeIndex, int sourceNodeIndex)
{
	if (blendTree)
	{
		if (sourceNodeIndex == -1)
		{
			blendTree->tree->value = nodeIndex;
		}
		else
		{
			a3_TreeNode* parentNode = a3TreeRetrieveNodeWithValue(blendTree->tree, sourceNodeIndex, blendTree->nodeCount);
			a3_TreeNode* childNode = a3TreeRetrieveNodeWithValue(blendTree->tree, nodeIndex, blendTree->nodeCount);
			if (!parentNode)
			{
				printf("Tree could not be built.");
				return -2;
			}
			if (!childNode)
			{
				a3TreeNodeAddChildValue(parentNode, nodeIndex);
			}
			else
			{
				if (a3TreeRetrieveNodeWithValue(childNode, sourceNodeIndex, blendTree->nodeCount))
				{
					printf("A node cannot have its parent as a child.");
					return -3;
				}
				a3TreeNodeAddChildNode(parentNode, childNode);
			}
		}
		return 1;
	}
	return -1;
}

a3i32 a3hierarchyBlendTreeBindStates(a3_HierarchyBlendTree* blendTree, a3_Hierarchy* hierarchy, a3_HierarchyState** optStates)
{
	if (blendTree && blendTree->tree && hierarchy)
	{
		a3i32 stateCount = blendTree->nodeCount;
		a3_HierarchyState* states = malloc(sizeof(a3_HierarchyState) * stateCount);
		if (optStates != NULL)
		{
			(*optStates) = states;
		}
		for (int i = 0; i < stateCount; i++)
		{
			blendTree->blendNodes[i]->state_out = states + i; //add states
		}


		a3_TreeNode** nodeStack = malloc(sizeof(a3_TreeNode*) * stateCount);
		a3i32* indexStack = malloc(sizeof(int) * stateCount);
		for (a3i32 i = 0; i < stateCount; i++)
		{
			nodeStack[i] = NULL;
			indexStack[i] = 0;
		}
		a3i32 stackSize = 0;

		nodeStack[0] = blendTree->tree;
		stackSize = 1;

		while (stackSize > 0)
		{
			a3_TreeNode* currentNode = nodeStack[stackSize - 1]; //grab node at top of stack
			if (currentNode->childCount > 0) //if node has children, go down as far as we can.
			{
				if (indexStack[stackSize - 1] < currentNode->childCount)
				{
					nodeStack[stackSize] = currentNode->children[indexStack[stackSize - 1]]; //add node to stack
					indexStack[stackSize - 1]++; //increment child index so when we return we go to a different node
					stackSize++; //move into new node
					if (stackSize < stateCount)
					{
						indexStack[stackSize - 1] = 0; //we've increased the stack position, so we're emptying out the next counter
					}
					continue;
				}
				else //if we're at a leaf node OR we're done iterating through a given node's children
				{
					if (stackSize > 1) //if we're not at the root
					{
						a3_TreeNode* parentNode = nodeStack[stackSize - 2]; //grab this node's parent
						a3_HierarchyBlendNode * parentBlendNode = blendTree->blendNodes[parentNode->value]; //find the corresponding blendNode
						for (int i = 0; i < parentNode->childCount; i++)
						{
							a3_HierarchyBlendNode* childBlendNode = blendTree->blendNodes[currentNode->value];
							parentBlendNode->controlStates[i] = childBlendNode->state_out; //attach output
						}
					}
					nodeStack[stackSize - 1] = NULL; //clear stack position
					stackSize--; //back out of stack
					continue;
				}
			}
			else
			{
				nodeStack[stackSize - 1] = NULL;
				stackSize--;
			}
		}
		return 1;
	}
	return -1;
}

/// <summary>
/// Loads a blend tree from a file
/// </summary>
/// <param name="blendTree_out"></param>
/// <param name="skeletal_hierarchy"></param>
/// <param name="resourceFilePath"></param>
/// <returns></returns>
/*
a3i32 a3hierarchyBlendTreeLoad(a3_HierarchyBlendTree* blendTree_out, a3_Hierarchy* skeletal_hierarchy, const a3byte* resourceFilePath)
{
	if (blendTree_out && skeletal_hierarchy && resourceFilePath && *resourceFilePath)
	{
		a3_Stream fs[1] = { 0 };
		a3ui32 fileLength = a3streamLoadContents(&fs[0], resourceFilePath);
		if (fileLength <= 0)
		{
			return -1;
		}
		a3byte* contentsCopy = malloc(fs->length * sizeof(a3byte));
		strncpy(contentsCopy, fs->contents, fs->length);
		//*strrchr(contentsCopy, '\r') = '\0';
		//node count
		char* token = strtok((char*)contentsCopy, "\n");
		while (token != NULL && strncmp(token, "NODECOUNT", 9) != 0)
		{
			token = strtok(NULL, "\n");
		}
		char* text = token;
		size_t len = strlen(text);
		if (text[len - 1] == '\r')
		{
			text[len - 1] = '\0';
		}
		int blendHierarchyCount = atoi(text + 9);
		blendTree_out->bt_hierarchy->nodes = 0; //won't run otherwise
		a3hierarchyCreate(blendTree_out->bt_hierarchy, blendHierarchyCount, NULL);
		blendTree_out->blendNodes = calloc(blendHierarchyCount, sizeof(a3_HierarchyBlendNode));

		//leaf count
		while (token != NULL && strncmp(token, "LEAFCOUNT", 9) != 0)
		{
			token = strtok(NULL, "\n");
		}
		text = token;
		len = strlen(text);
		if (text[len - 1] == '\r')
		{
			text[len - 1] = '\0';
		}
		int leafCount = atoi(text + 9);
		blendTree_out->leafCount = (a3ui32)leafCount;
		blendTree_out->leafIndices = calloc(leafCount, sizeof(a3i32));

		//leaves
		while (token != NULL && strncmp(token, "LEAVES", 6) != 0)
		{
			token = strtok(NULL, "\n");
		}
		text = token;
		len = strlen(text);
		if (text[len - 1] == '\r')
		{
			text[len - 1] = '\0';
		}
		text = text + 6;
		text = strchr(text, ' ') + 1;
		for (int i = 0; i < leafCount; i++) //loop through all leaf indices and store them in array
		{
			blendTree_out->leafIndices[i] = atoi(text);
			text = strchr(text, ' ') + 1;
		}

		//parse each node
		for (a3ui32 i = 0; i < blendTree_out->bt_hierarchy->numNodes; i++)
		{
			a3hierarchySetNode(blendTree_out->bt_hierarchy, i, -1, "Node"); //pre-assign them
		}
		//leaves
		while (token != NULL && strncmp(token, "NODES", 5) != 0)
		{
			token = strtok(NULL, "\n");
		}
		token = strtok(NULL, "\n"); //go to line after "NODES"

		//parse nodes themselves
		while (token != NULL)
		{
			text = token;
			a3ui32 index = atoi(text);
			text = strchr(text, ' ') + 1;
			a3boolean isClipNode = text[0] == 'C';
			text = strchr(text, ' ') + 1;
			NodeType clipType = identity;

			//names of source clips
			a3byte* clipNames[16];
			//index in clipNames array
			a3ui32 clipNameIndex = 0;

			a3ui32 sourceNodeIndices[16];
			a3ui32 sourceNodeCount = 0;

			a3f32* inputParams[8];
			a3ui32 paramCount = 0;

			//node types, clip first
			if (isClipNode)
			{
				if (strncmp(text, "ADD", 3) == 0)
					clipType = addClip;
				else if (strncmp(text, "LERP", 4) == 0)
					clipType = lerpClip;
				else if (strncmp(text, "NEGATE", 6) == 0)
					clipType = negateClip;
				else if (strncmp(text, "SCALE", 5) == 0)
					clipType = scaleClip;
				else if (strncmp(text, "COPY", 4) == 0)
					clipType = copyClip;
			}
			else
			{
				if (strncmp(text, "IDENTITY", 8) == 0)
					clipType = identity;
				else if (strncmp(text, "FK", 2) == 0)
					clipType = fk;
				else if (strncmp(text, "FK", 2) == 0)
					clipType = ik;
				else if (strncmp(text, "COPY", 4) == 0)
					clipType = copy;
				else if (strncmp(text, "NEGATE", 6) == 0)
					clipType = negate;
				else if (strncmp(text, "CONCAT", 6) == 0)
					clipType = concat;
				else if (strncmp(text, "CONVERT", 7) == 0)
					clipType = convert;
				else if (strncmp(text, "REVERT", 6) == 0)
					clipType = revert;
				else if (strncmp(text, "SCALE", 5) == 0)
					clipType = scale;
				else if (strncmp(text, "BISCALE", 7) == 0)
					clipType = biscale;
				else if (strncmp(text, "DECONCAT", 8) == 0)
					clipType = deconcat;
				else if (strncmp(text, "NEAREST", 7) == 0)
					clipType = nearest;
				else if (strncmp(text, "LERP", 4) == 0)
					clipType = lerp;
				else if (strncmp(text, "EASE", 4) == 0)
					clipType = easeinout;
				else if (strncmp(text, "TRIANGULAR", 10) == 0)
					clipType = triangular;
				else if (strncmp(text, "CUBIC", 5) == 0)
					clipType = cubic;
				else if (strncmp(text, "BINEAREST", 9) == 0)
					clipType = binearest;
				else if (strncmp(text, "BILERP", 6) == 0)
					clipType = bilerp;
				else if (strncmp(text, "BICUBIC", 7) == 0)
					clipType = bicubic;
			}
			while (strncmp(text, "END", 3) != 0)
			{
				//store controls
				if (strncmp(text, "CLIP", 4) == 0) //clip!
				{
					//do a lot of pointer math to copy over the name of a clip
					text = strchr(text, ' ') + 1;
					a3byte* terminator = strchr(text, ' ');
					if (*terminator == '\r')
					{
						*terminator = '\0';
					}
					clipNames[clipNameIndex] = malloc((strlen(text) + 1) * sizeof(a3byte));
					clipNames[clipNameIndex][terminator - text] = '\0';
					strncpy(clipNames[clipNameIndex], text, terminator - text);
					text = terminator + 1;
					clipNameIndex++;
				}
				else if (strncmp(text, "NODE", 4) == 0) //node
				{
					text = strchr(text, ' ') + 1;
					a3i32 childIndex = atoi(text);
					sourceNodeIndices[sourceNodeCount] = childIndex;
					a3hierarchySetNode(blendTree_out->bt_hierarchy, childIndex, index, "Node"); //sets parent
					sourceNodeCount++;
				}
				else if (strncmp(text, "PARAM", 5) == 0)
				{
					text = strchr(text, ' ') + 1;
					inputParams[paramCount] = malloc(sizeof(a3f32));
					*inputParams[paramCount] = (float)atof(text);
					paramCount++;
				}
				else
				{
					text = strchr(text, ' ') + 1;
				}
			}
			token = strtok(NULL, "\n");
			a3hierarchyBlendNodeCreateInternal(blendTree_out, &blendTree_out->blendNodes[index],
				clipType, clipNameIndex, clipNames, sourceNodeCount, sourceNodeIndices, paramCount, inputParams);

		}
		free(contentsCopy);
		///Bind outputs after everything's parsed
		return 1;
	}
	return -1;
}
*/
//TODO: once skeleton has been obtained, assign HPoseStates by iterating through the hierarchy leafnode first. Each leafnode gets an HState for its pose and inputs
//next, all the parents get their output nodes and have their input nodes bound
//repeat till done

//this pattern is also used for the update loop, though I don't know how to get the info for those clip nodes.

//other things:
//need to entirely rewrite FK/IK because that only operates on a single pose and doesn't work with this system since it returns mat4
//probably need a different exec for the clipOps because those require timing information

/*
/// <summary>
/// Update tree
/// </summary>
/// <param name="blendTree_out"></param>
/// <returns></returns>
a3i32 a3hierarchyblendTreeUpdate(a3_HierarchyBlendTree* blendTree_out)
{
	if (!blendTree_out)
	{
		return -1;
	}
	//a3_HierarchyState const* baseHS = demoMode->hierarchyState_skel_base;
	//Rules for the tree are that all prereqs for a node must be earlier than the node, thus this order works.
	for (a3ui32 nodeInd = 0; nodeInd < blendTree_out->bt_hierarchy->numNodes; nodeInd++)
	{
		a3_HierarchyBlendNode* node = &blendTree_out->blendNodes[nodeInd];
		node->exec(node);
		//the HTR nodes currently are pre-concated, so no concat call here.
		//a3hierarchyPoseOpCopy(node->state_out->localHPose, node->state_out->animPose, node->poseGroup->hierarchy->numNodes);
		//TODO switch to a concat with the animPose and baseHS, then run convert
		a3hierarchyPoseOpConvert(node->state_out->localSpace, node->poseGroup->hierarchy->numNodes);

		//outputs from local to object
		a3kinematicsSolveForward(node->state_out);
	}
	return 1;
}
*/