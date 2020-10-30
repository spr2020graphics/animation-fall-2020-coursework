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
void a3spatialBlendExec0C(a3_SpatialBlendNode* node_inout)
{
	node_inout->operation(node_inout->pose);
}

/// <summary>
/// Copy, Const, Negate, Concat, Convert, Revert
/// </summary>
/// <param name="node_inout"></param>
void a3spatialBlendExec1C(a3_SpatialBlendNode* node_inout)
{
	node_inout->operation(node_inout->pose,
		node_inout->controls[0]);
}

/// <summary>
/// Scale, Bidirectional scale
/// </summary>
/// <param name="node_inout"></param>
void a3spatialBlendExec1C1I(a3_SpatialBlendNode* node_inout)
{
	node_inout->operation(node_inout->pose,
		node_inout->controls[0],
		*node_inout->uVals[0]);
}

/// <summary>
/// Deconcat
/// </summary>
/// <param name="node_inout"></param>
void a3spatialBlendExec2C(a3_SpatialBlendNode* node_inout)
{
	node_inout->operation(node_inout->pose,
		node_inout->controls[0],
		node_inout->controls[1]);
}

/// <summary>
/// Nearest, LERP, EaseInOut
/// </summary>
/// <param name="node_inout"></param>
void a3spatialBlendExec2C1I(a3_SpatialBlendNode* node_inout)
{
	node_inout->operation(node_inout->pose,
		node_inout->controls[0],
		node_inout->controls[1],
		*node_inout->uVals[0]);
}

/// <summary>
/// Triangular
/// </summary>
/// <param name="node_inout"></param>
void a3spatialBlendExec3C2I(a3_SpatialBlendNode* node_inout)
{
	node_inout->operation(node_inout->pose,
		node_inout->controls[0],
		node_inout->controls[1],
		node_inout->controls[2],
		*node_inout->uVals[0], *node_inout->uVals[1]);
}

/// <summary>
/// Cubic
/// </summary>
/// <param name="node_inout"></param>
void a3spatialBlendExec4C1I(a3_SpatialBlendNode* node_inout)
{
	node_inout->operation(node_inout->pose,
		node_inout->controls[0],
		node_inout->controls[1],
		node_inout->controls[2],
		node_inout->controls[3], &node_inout->uVals[0]);
}

/// <summary>
/// Binearest, Bilinear
/// </summary>
/// <param name="node_inout"></param>
void a3spatialBlendExec4C3I(a3_SpatialBlendNode* node_inout)
{
	node_inout->operation(node_inout->pose,
		node_inout->controls[0],
		node_inout->controls[1],
		node_inout->controls[2],
		node_inout->controls[3],
		&node_inout->uVals[0], &node_inout->uVals[1], &node_inout->uVals[2]);
}

/// <summary>
/// Bicubic
/// </summary>
/// <param name="node_inout"></param>
void a3spatialBlendExec16C5I(a3_SpatialBlendNode* node_inout)
{
	node_inout->operation(node_inout->pose,
		node_inout->controls[0 * 4 + 0], node_inout->controls[0 * 4 + 1], node_inout->controls[0 * 4 + 2], node_inout->controls[0 * 4 + 3],
		node_inout->controls[1 * 4 + 0], node_inout->controls[1 * 4 + 1], node_inout->controls[1 * 4 + 2], node_inout->controls[1 * 4 + 3],
		node_inout->controls[2 * 4 + 0], node_inout->controls[2 * 4 + 1], node_inout->controls[2 * 4 + 2], node_inout->controls[2 * 4 + 3],
		node_inout->controls[3 * 4 + 0], node_inout->controls[3 * 4 + 1], node_inout->controls[3 * 4 + 2], node_inout->controls[3 * 4 + 3],
		&node_inout->uVals[0], &node_inout->uVals[1], &node_inout->uVals[2], &node_inout->uVals[3], &node_inout->uVals[4]);
}

/// <summary>
/// Identity
/// </summary>
/// <param name="node_inout"></param>
void a3hierarchyBlendExec0C(a3_HierarchyBlendNode* node_inout)
{
	node_inout->operation(node_inout->state_out->sampleHPose,
		node_inout->state_out->hierarchy->numNodes);
}

/// <summary>
/// Copy, Const, Negate, Concat, Convert, Revert
/// </summary>
/// <param name="node_inout"></param>
void a3hierarchyBlendExec1C(a3_HierarchyBlendNode* node_inout)
{
	node_inout->operation(node_inout->state_out->sampleHPose,
		node_inout->controlStates[0],
		node_inout->state_out->hierarchy->numNodes);
}

/// <summary>
/// Scale, Bidirectional scale
/// </summary>
/// <param name="node_inout"></param>
void a3hierarchyBlendExec1C1I(a3_HierarchyBlendNode* node_inout)
{
	node_inout->operation(node_inout->state_out->sampleHPose,
		node_inout->controlStates[0],
		*node_inout->uVals[0],
		node_inout->state_out->hierarchy->numNodes);
}

/// <summary>
/// Deconcat
/// </summary>
/// <param name="node_inout"></param>
void a3hierarchyBlendExec2C(a3_HierarchyBlendNode* node_inout)
{
	node_inout->operation(node_inout->state_out->sampleHPose,
		node_inout->controlStates[0],
		node_inout->controlStates[1],
		node_inout->state_out->hierarchy->numNodes);
}

/// <summary>
/// Nearest, LERP, EaseInOut
/// </summary>
/// <param name="node_inout"></param>
void a3hierarchyBlendExec2C1I(a3_HierarchyBlendNode* node_inout)
{
	node_inout->operation(node_inout->state_out->sampleHPose,
		node_inout->controlStates[0],
		node_inout->controlStates[1],
		*node_inout->uVals[0],
		node_inout->state_out->hierarchy->numNodes);
}

/// <summary>
/// Triangular
/// </summary>
/// <param name="node_inout"></param>
void a3hierarchyBlendExec3C2I(a3_HierarchyBlendNode* node_inout)
{
	node_inout->operation(node_inout->state_out->sampleHPose,
		node_inout->controlStates[0],
		node_inout->controlStates[1],
		node_inout->controlStates[2],
		*node_inout->uVals[0], *node_inout->uVals[1],
		node_inout->state_out->hierarchy->numNodes);
}

/// <summary>
/// Cubic
/// </summary>
/// <param name="node_inout"></param>
void a3hierarchyBlendExec4C1I(a3_HierarchyBlendNode* node_inout)
{
	node_inout->operation(node_inout->state_out->sampleHPose,
		node_inout->controlStates[0],
		node_inout->controlStates[1],
		node_inout->controlStates[2],
		node_inout->controlStates[3],
		*node_inout->uVals[0],
		node_inout->state_out->hierarchy->numNodes);
}

/// <summary>
/// Binearest, Bilinear
/// </summary>
/// <param name="node_inout"></param>
void a3hierarchyBlendExec4C3I(a3_HierarchyBlendNode* node_inout)
{
	node_inout->operation(node_inout->state_out->sampleHPose,
		node_inout->controlStates[0],
		node_inout->controlStates[1],
		node_inout->controlStates[2],
		node_inout->controlStates[3],
		*node_inout->uVals[0], *node_inout->uVals[1], *node_inout->uVals[2],
		node_inout->state_out->hierarchy->numNodes);
}

/// <summary>
/// Bicubic
/// </summary>
/// <param name="node_inout"></param>
void a3hierarchyBlendExec16C5I(a3_HierarchyBlendNode* node_inout)
{
	node_inout->operation(node_inout->state_out->sampleHPose,
		node_inout->controlStates[0 * 4 + 0], node_inout->controlStates[0 * 4 + 1], node_inout->controlStates[0 * 4 + 2], node_inout->controlStates[0 * 4 + 3],
		node_inout->controlStates[1 * 4 + 0], node_inout->controlStates[1 * 4 + 1], node_inout->controlStates[1 * 4 + 2], node_inout->controlStates[1 * 4 + 3],
		node_inout->controlStates[2 * 4 + 0], node_inout->controlStates[2 * 4 + 1], node_inout->controlStates[2 * 4 + 2], node_inout->controlStates[2 * 4 + 3],
		node_inout->controlStates[3 * 4 + 0], node_inout->controlStates[3 * 4 + 1], node_inout->controlStates[3 * 4 + 2], node_inout->controlStates[3 * 4 + 3],
		*node_inout->uVals[0], *node_inout->uVals[1], *node_inout->uVals[2], *node_inout->uVals[3], *node_inout->uVals[4],
		node_inout->state_out->hierarchy->numNodes);
}

/// <summary>
/// Negate, Copy
/// </summary>
/// <param name="node_inout"></param>
void a3hierarchyClipExec1C(a3_HierarchyBlendNode* node_inout)
{
	a3clipOpSampleClip(node_inout->controlStates[0]->sampleHPose, node_inout->poseGroup, node_inout->clipControllers[0]);
	node_inout->operation(node_inout->state_out->sampleHPose,
		node_inout->poseGroup,
		node_inout->clipControllers[0]);
}

/// <summary>
/// Scale
/// </summary>
/// <param name="node_inout"></param>
void a3hierarchyClipExec1C1I(a3_HierarchyBlendNode* node_inout)
{
	a3clipOpSampleClip(node_inout->controlStates[0]->sampleHPose, node_inout->poseGroup, node_inout->clipControllers[0]);
	node_inout->operation(node_inout->state_out->sampleHPose,
		node_inout->poseGroup,
		node_inout->clipControllers[0],
		*node_inout->uVals[0]);
}

/// <summary>
/// Add
/// </summary>
/// <param name="node_inout"></param>
void a3hierarchyClipExec2C(a3_HierarchyBlendNode* node_inout)
{
	a3clipOpSampleClip(node_inout->controlStates[0]->sampleHPose, node_inout->poseGroup, node_inout->clipControllers[0]);
	a3clipOpSampleClip(node_inout->controlStates[1]->sampleHPose, node_inout->poseGroup, node_inout->clipControllers[1]);
	node_inout->operation(node_inout->state_out->sampleHPose,
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
	a3clipOpSampleClip(node_inout->controlStates[0]->sampleHPose, node_inout->poseGroup, node_inout->clipControllers[0]);
	a3clipOpSampleClip(node_inout->controlStates[1]->sampleHPose, node_inout->poseGroup, node_inout->clipControllers[1]);
	node_inout->operation(node_inout->state_out->sampleHPose,
		node_inout->poseGroup,
		node_inout->clipControllers[0],
		node_inout->clipControllers[1],
		*node_inout->uVals[0]);
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
a3i32 a3hierarchyBlendNodeCreate(a3_HierarchyBlendTree* refTree, a3_HierarchyBlendNode* blendNode_out, NodeType type,
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

/// <summary>
/// Loads a blend tree from a file
/// </summary>
/// <param name="blendTree_out"></param>
/// <param name="skeletal_hierarchy"></param>
/// <param name="resourceFilePath"></param>
/// <returns></returns>
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
			a3hierarchyBlendNodeCreate(blendTree_out, &blendTree_out->blendNodes[index],
				clipType, clipNameIndex, clipNames, sourceNodeCount, sourceNodeIndices, paramCount, inputParams);

		}
		free(contentsCopy);
		///Bind outputs after everything's parsed
		return 1;
	}
	return -1;
}

//TODO: once skeleton has been obtained, assign HPoseStates by iterating through the hierarchy leafnode first. Each leafnode gets an HState for its pose and inputs
//next, all the parents get their output nodes and have their input nodes bound
//repeat till done

//this pattern is also used for the update loop, though I don't know how to get the info for those clip nodes.

//other things:
//need to entirely rewrite FK/IK because that only operates on a single pose and doesn't work with this system since it returns mat4
//probably need a different exec for the clipOps because those require timing information

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

	//Rules for the tree are that all prereqs for a node must be earlier than the node, thus this order works.
	for (a3ui32 nodeInd = 0; nodeInd < blendTree_out->bt_hierarchy->numNodes; nodeInd++)
	{
		a3_HierarchyBlendNode* node = &blendTree_out->blendNodes[nodeInd];
		node->exec(node);
		//the HTR nodes currently are pre-concated, so no concat call here.
		a3hierarchyPoseOpCopy(node->state_out->localHPose, node->state_out->sampleHPose, node->poseGroup->hierarchy->numNodes);
		a3hierarchyPoseOpConvert(node->state_out->localHPose, node->poseGroup->hierarchy->numNodes);

		//outputs from local to object
		a3kinematicsSolveForward(node->state_out);
	}
	return 1;
}