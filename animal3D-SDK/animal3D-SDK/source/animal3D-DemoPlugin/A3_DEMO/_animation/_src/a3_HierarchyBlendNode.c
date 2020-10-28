/*
	Animation Framework Addons by Scott Dagen and Cameron Schneider
	a3_HierarchyBlendNode.h: implementations for blend trees and blend nodes.
*/

#include "..\a3_HierarchyBlendNode.h"
#include <stdlib.h>
#include <string.h>
#include "..\a3_HierarchyStateBlend.h"

enum NodeType {
	addClip, lerpClip, scaleClip, negateClip,
	identity, init,
	copy, constant, negate, concat, convert, revert,
	scale, biscale,
	deconcat,
	nearest, lerp, easeinout,
	triangular,
	cubic,
	binearest, bilerp,
	bicubic
};
typedef enum NodeType NodeType;

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
		&node_inout->uVals[0]);
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
		&node_inout->uVals[0]);
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
		&node_inout->uVals[0], &node_inout->uVals[1]);
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

void a3spatialBlendExec16C5I(a3_SpatialBlendNode* node_inout)
{
	node_inout->operation(node_inout->pose,
		node_inout->controls[0 * 4 + 0], node_inout->controls[0 * 4 + 1], node_inout->controls[0 * 4 + 2], node_inout->controls[0 * 4 + 3],
		node_inout->controls[1 * 4 + 0], node_inout->controls[1 * 4 + 1], node_inout->controls[1 * 4 + 2], node_inout->controls[1 * 4 + 3],
		node_inout->controls[2 * 4 + 0], node_inout->controls[2 * 4 + 1], node_inout->controls[2 * 4 + 2], node_inout->controls[2 * 4 + 3],
		node_inout->controls[3 * 4 + 0], node_inout->controls[3 * 4 + 1], node_inout->controls[3 * 4 + 2], node_inout->controls[3 * 4 + 3],
		&node_inout->uVals[0], &node_inout->uVals[1], &node_inout->uVals[2], &node_inout->uVals[3], &node_inout->uVals[4]);
}

void a3hierarchyBlendExec0C(a3_HierarchyBlendNode* node_inout, const a3ui32 nodeCount)
{
	node_inout->operation(node_inout->pose, nodeCount);
}

void a3hierarchyBlendExec1C(a3_HierarchyBlendNode* node_inout, const a3ui32 nodeCount)
{
	node_inout->operation(node_inout->pose,
		node_inout->controls[0],
		nodeCount);
}

void a3hierarchyBlendExec1C1I(a3_HierarchyBlendNode* node_inout, const a3ui32 nodeCount)
{
	node_inout->operation(node_inout->pose,
		node_inout->controls[0],
		&node_inout->uVals[0],
		nodeCount);
}

void a3hierarchyBlendExec2C(a3_HierarchyBlendNode* node_inout, const a3ui32 nodeCount)
{
	node_inout->operation(node_inout->pose,
		node_inout->controls[0],
		node_inout->controls[1],
		nodeCount);
}

void a3hierarchyBlendExec2C1I(a3_HierarchyBlendNode* node_inout, const a3ui32 nodeCount)
{
	node_inout->operation(node_inout->pose,
		node_inout->controls[0],
		node_inout->controls[1],
		&node_inout->uVals[0],
		nodeCount);
}

void a3hierarchyBlendExec3C2I(a3_HierarchyBlendNode* node_inout, const a3ui32 nodeCount)
{
	node_inout->operation(node_inout->pose,
		node_inout->controls[0],
		node_inout->controls[1],
		node_inout->controls[2],
		&node_inout->uVals[0], &node_inout->uVals[1],
		nodeCount);
}

void a3hierarchyBlendExec4C1I(a3_HierarchyBlendNode* node_inout, const a3ui32 nodeCount)
{
	node_inout->operation(node_inout->pose,
		node_inout->controls[0],
		node_inout->controls[1],
		node_inout->controls[2],
		node_inout->controls[3],
		&node_inout->uVals[0],
		nodeCount);
}

void a3hierarchyBlendExec4C3I(a3_HierarchyBlendNode* node_inout, const a3ui32 nodeCount)
{
	node_inout->operation(node_inout->pose,
		node_inout->controls[0],
		node_inout->controls[1],
		node_inout->controls[2],
		node_inout->controls[3],
		&node_inout->uVals[0], &node_inout->uVals[1], &node_inout->uVals[2],
		nodeCount);
}

void a3hierarchyBlendExec16C5I(a3_HierarchyBlendNode* node_inout, const a3ui32 nodeCount)
{
	node_inout->operation(node_inout->pose,
		node_inout->controls[0 * 4 + 0], node_inout->controls[0 * 4 + 1], node_inout->controls[0 * 4 + 2], node_inout->controls[0 * 4 + 3],
		node_inout->controls[1 * 4 + 0], node_inout->controls[1 * 4 + 1], node_inout->controls[1 * 4 + 2], node_inout->controls[1 * 4 + 3],
		node_inout->controls[2 * 4 + 0], node_inout->controls[2 * 4 + 1], node_inout->controls[2 * 4 + 2], node_inout->controls[2 * 4 + 3],
		node_inout->controls[3 * 4 + 0], node_inout->controls[3 * 4 + 1], node_inout->controls[3 * 4 + 2], node_inout->controls[3 * 4 + 3],
		&node_inout->uVals[0], &node_inout->uVals[1], &node_inout->uVals[2], &node_inout->uVals[3], &node_inout->uVals[4],
		nodeCount);
}

a3i32 a3hierarchyBlendNodeCreate(a3_HierarchyBlendTree* refTree, a3_HierarchyBlendNode* blendNode_out, NodeType type,
	a3ui32 clipCount, a3byte** clipNames,
	a3ui32 controlNodeCount, a3ui32* controlNodes,
	a3ui32 inputCount, a3f32** uValPtrs)
{
	//how do I bind the hPose????
	for (a3ui32 i = 0; i < clipCount; i++)
	{
		blendNode_out->clipNames[i] = clipNames[i];
	}
	for (a3ui32 i = 0; i < controlNodeCount; i++)
	{
		//assign input control pose as the output of a different node's blend op
		blendNode_out->controls[i] = refTree->blendNodes[controlNodes[i]].pose;
	}
	for (a3ui32 i = 0; i < inputCount; i++)
	{
		blendNode_out->uVals[i] = uValPtrs[i];
	}

	switch (type)
	{
	case addClip:
		blendNode_out->exec = &a3hierarchyBlendExec2C;
		blendNode_out->operation = NULL;
		break;
	case lerpClip:
		blendNode_out->exec = &a3hierarchyBlendExec2C1I;
		blendNode_out->operation = NULL;
		break;
	case scaleClip:
		blendNode_out->exec = &a3hierarchyBlendExec1C1I;
		blendNode_out->operation = NULL;
		break;
	case negateClip:
		blendNode_out->exec = &a3hierarchyBlendExec1C;
		blendNode_out->operation = NULL;
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
	case constant:
		blendNode_out->exec = &a3hierarchyBlendExec1C;
		blendNode_out->operation = &a3hierarchyPoseOpConst;
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
	}
	return 1;
}

a3i32 a3hierarchyBlendTreeLoad(a3_HierarchyBlendTree* blendTree_out, a3_Hierarchy* hierarchy_out, const a3byte* resourceFilePath)
{
	if (blendTree_out && hierarchy_out && !hierarchy_out->numNodes && resourceFilePath && *resourceFilePath)
	{
		a3_Stream fs[1] = { 0 };
		a3ui32 fileLength = a3streamLoadContents(&fs[0], resourceFilePath);
		if (fileLength <= 0)
		{
			return -1;
		}
		a3byte* contentsCopy = malloc(fs->length * sizeof(a3byte));
		strncpy(contentsCopy, fs->contents, fs->length);
		*strrchr(contentsCopy, '\r') = '\0';
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
		blendTree_out->hierarchy->nodes = 0; //won't run otherwise
		a3hierarchyCreate(blendTree_out->hierarchy, blendHierarchyCount, NULL);
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
		text = strchr(text, ' ') + 1; //advance past the number of channels
		//need to advance past the channel count
		for (int i = 0; i < leafCount; i++) //loop through all channels and store the data both as a combined mask and as an array
		{
			blendTree_out->leafIndices[i] = atoi(text);
			text = strchr(text, ' ') + 1; //advance past the number of channels. parsePos will be 1 at the end of the loop, but that's fine
		}

		//parse each node
		a3byte* str = malloc(sizeof(a3byte));
		str[0] = 'N';
		for (a3ui32 i = 0; i < blendTree_out->hierarchy->numNodes; i++)
		{
			a3hierarchySetNode(blendTree_out->hierarchy, i, -1, "Node"); //pre-assign them
		}
		//leaves
		while (token != NULL && strncmp(token, "NODES", 5) != 0)
		{
			token = strtok(NULL, "\n");
		}
		token = strtok(NULL, "\n"); //go to line after "NODES"

		while (token != NULL)
		{
			text = token;
			a3ui32 index = atoi(text);
			text = strchr(text, ' ') + 1;
			a3boolean isClipNode = text[0] == 'C';
			text = strchr(text, ' ') + 1;
			NodeType clipType = identity;
			//determine blend op
			a3byte* clipNames[16];
			a3ui32 clipNameIndex = 0;
			a3ui32 sourceNodeIndices[16];
			a3ui32 sourceNodeCount = 0;
			a3f32* inputParams[8];
			a3ui32 paramCount = 0;

			if (isClipNode)
			{
				switch (text[0])
				{
				case 'A':
					clipType = addClip;
					break;
				case 'L':
					clipType = lerpClip;
					break;
				case 'N':
					clipType = negateClip;
					break;
				case 'S':
					clipType = scaleClip;
					break;
				}
			}
			else
			{
				switch (text[0])
				{
				case 'L':
					clipType = lerp;
					break;
				}
				/////HOWEVER MANY CASE SWITCH STATEMENT
			}
			while (strncmp(text, "END", 3) != 0)
			{
				//store controls
				if (strncmp(text, "CLIP", 4) == 0) //clip!
				{
					//do a lot of pointer math to copy over the name of a clip
					text = strchr(text, ' ') + 1;
					a3byte* terminator = strchr(text, ' ');
					*terminator = '\0';
					clipNames[clipNameIndex] = malloc((strlen(text)+1) * sizeof(a3byte));
					clipNames[clipNameIndex][strlen(text)] = '\0';
					strncpy(clipNames[clipNameIndex], text, strlen(text));
					text = terminator + 1;
					clipNameIndex++;
				}
				else if (strncmp(text, "NODE", 4) == 0) //node
				{
					text = strchr(text, ' ') + 1;
					a3i32 childIndex = atoi(text);
					sourceNodeIndices[sourceNodeCount] = childIndex;
					a3hierarchySetNode(blendTree_out->hierarchy, childIndex, index, "Node"); //sets parent
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
			a3hierarchyBlendNodeCreate(blendTree_out, &blendTree_out->blendNodes[index], clipType, clipNameIndex, clipNames, sourceNodeCount, sourceNodeIndices, paramCount, inputParams);
		}

		///Bind outputs after everything's parsed
		return 1;
	}
	return -1;
}