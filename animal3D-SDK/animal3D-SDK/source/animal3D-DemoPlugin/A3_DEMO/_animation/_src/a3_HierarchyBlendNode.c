/*
	Animation Framework Addons by Scott Dagen and Cameron Schneider
	a3_HierarchyBlendNode.h: implementations for blend trees and blend nodes.
*/

#include "..\a3_HierarchyBlendNode.h"
#include <stdlib.h>
#include <string.h>

enum NodeType { addClip, lerpClip, scaleClip, negateClip };
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
/// Copy, Const, Negate, Concat
/// </summary>
/// <param name="node_inout"></param>
void a3spatialBlendExec1C(a3_SpatialBlendNode* node_inout)
{
	node_inout->operation(node_inout->pose,
		node_inout->controls[0]);
}

/// <summary>
/// Scale
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
/// Nearest, LERP
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


void a3HierarchyBlendExec0C(a3_HierarchyBlendNode* node_inout, const a3ui32 nodeCount)
{
	node_inout->operation(node_inout->pose, nodeCount);
}

void a3HierarchyBlendExec1C(a3_HierarchyBlendNode* node_inout, const a3ui32 nodeCount)
{
	node_inout->operation(node_inout->pose,
		node_inout->controls[0],
		nodeCount);
}

void a3HierarchyBlendExec1C1I(a3_HierarchyBlendNode* node_inout, const a3ui32 nodeCount)
{
	node_inout->operation(node_inout->pose,
		node_inout->controls[0],
		&node_inout->uVals[0],
		nodeCount);
}

void a3HierarchyBlendExec2C(a3_HierarchyBlendNode* node_inout, const a3ui32 nodeCount)
{
	node_inout->operation(node_inout->pose,
		node_inout->controls[0],
		node_inout->controls[1],
		nodeCount);
}

void a3HierarchyBlendExec2C1I(a3_HierarchyBlendNode* node_inout, const a3ui32 nodeCount)
{
	node_inout->operation(node_inout->pose,
		node_inout->controls[0],
		node_inout->controls[1],
		&node_inout->uVals[0],
		nodeCount);
}

void a3HierarchyBlendExec3C2I(a3_HierarchyBlendNode* node_inout, const a3ui32 nodeCount)
{
	node_inout->operation(node_inout->pose,
		node_inout->controls[0],
		node_inout->controls[1],
		node_inout->controls[2],
		&node_inout->uVals[0], &node_inout->uVals[1],
		nodeCount);
}

void a3HierarchyBlendExec4C1I(a3_HierarchyBlendNode* node_inout, const a3ui32 nodeCount)
{
	node_inout->operation(node_inout->pose,
		node_inout->controls[0],
		node_inout->controls[1],
		node_inout->controls[2],
		node_inout->controls[3],
		&node_inout->uVals[0],
		nodeCount);
}

void a3HierarchyBlendExec4C3I(a3_HierarchyBlendNode* node_inout, const a3ui32 nodeCount)
{
	node_inout->operation(node_inout->pose,
		node_inout->controls[0],
		node_inout->controls[1],
		node_inout->controls[2],
		node_inout->controls[3],
		&node_inout->uVals[0], &node_inout->uVals[1], &node_inout->uVals[2],
		nodeCount);
}

void a3HierarchyBlendExec16C5I(a3_HierarchyBlendNode* node_inout, const a3ui32 nodeCount)
{
	node_inout->operation(node_inout->pose,
		node_inout->controls[0 * 4 + 0], node_inout->controls[0 * 4 + 1], node_inout->controls[0 * 4 + 2], node_inout->controls[0 * 4 + 3],
		node_inout->controls[1 * 4 + 0], node_inout->controls[1 * 4 + 1], node_inout->controls[1 * 4 + 2], node_inout->controls[1 * 4 + 3],
		node_inout->controls[2 * 4 + 0], node_inout->controls[2 * 4 + 1], node_inout->controls[2 * 4 + 2], node_inout->controls[2 * 4 + 3],
		node_inout->controls[3 * 4 + 0], node_inout->controls[3 * 4 + 1], node_inout->controls[3 * 4 + 2], node_inout->controls[3 * 4 + 3],
		&node_inout->uVals[0], &node_inout->uVals[1], &node_inout->uVals[2], &node_inout->uVals[3], &node_inout->uVals[4],
		nodeCount);
}

a3i32 a3hierarchyBlendNodeCreate(a3_HierarchyBlendTree* refTree, a3_HierarchyBlendNode* blendNode_out, a3_HierarchyBlendOp op, a3_HierarchyBlendExec exec,
	a3ui32 clipCount, a3byte** clipNames,
	a3ui32 controlNodeCount, a3ui32* controlNodes,
	a3ui32 inputCount, a3f32** uValPtrs)
{
	blendNode_out->exec = exec;
	blendNode_out->operation = op;
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
			a3hierarchySetNode(blendTree_out->hierarchy, i, -1, str); //pre-assign them
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
			NodeType clipType;
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
				case 'S':
					clipType = scaleClip;
				}
			}
			else
			{
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
					sourceNodeIndices[sourceNodeCount] = atoi(text);
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
			
		}

		return 1;
	}
	return -1;
}