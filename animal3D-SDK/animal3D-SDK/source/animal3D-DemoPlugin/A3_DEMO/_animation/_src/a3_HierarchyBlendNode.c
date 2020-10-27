/*
	Animation Framework Addons by Scott Dagen and Cameron Schneider
	a3_HierarchyBlendNode.h: implementations for blend trees and blend nodes.
*/

#include "..\a3_HierarchyBlendNode.h"
#include <stdlib.h>
#include <string.h>

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
		blendTree_out->hierarchy = malloc(sizeof(a3_Hierarchy));
		a3hierarchyCreate(blendTree_out->hierarchy, blendHierarchyCount, NULL);

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
		return 1;
	}
	return -1;
}