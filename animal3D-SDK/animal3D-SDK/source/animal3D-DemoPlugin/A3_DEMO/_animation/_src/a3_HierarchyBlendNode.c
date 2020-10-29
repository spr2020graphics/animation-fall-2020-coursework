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

void a3hierarchyBlendExec0C(a3_HierarchyBlendNode* node_inout)
{
	node_inout->operation(node_inout->state_out->sampleHPose,
		node_inout->state_out->hierarchy->numNodes);
}

void a3hierarchyBlendExec1C(a3_HierarchyBlendNode* node_inout)
{
	node_inout->operation(node_inout->state_out->sampleHPose,
		node_inout->controlStates[0],
		node_inout->state_out->hierarchy->numNodes);
}

void a3hierarchyBlendExec1C1I(a3_HierarchyBlendNode* node_inout)
{
	node_inout->operation(node_inout->state_out->sampleHPose,
		node_inout->controlStates[0],
		&node_inout->uVals[0],
		node_inout->state_out->hierarchy->numNodes);
}

void a3hierarchyBlendExec2C(a3_HierarchyBlendNode* node_inout)
{
	node_inout->operation(node_inout->state_out->sampleHPose,
		node_inout->controlStates[0],
		node_inout->controlStates[1],
		node_inout->state_out->hierarchy->numNodes);
}

void a3hierarchyBlendExec2C1I(a3_HierarchyBlendNode* node_inout)
{
	node_inout->operation(node_inout->state_out->sampleHPose,
		node_inout->controlStates[0],
		node_inout->controlStates[1],
		&node_inout->uVals[0],
		node_inout->state_out->hierarchy->numNodes);
}

void a3hierarchyBlendExec3C2I(a3_HierarchyBlendNode* node_inout)
{
	node_inout->operation(node_inout->state_out->sampleHPose,
		node_inout->controlStates[0],
		node_inout->controlStates[1],
		node_inout->controlStates[2],
		&node_inout->uVals[0], &node_inout->uVals[1],
		node_inout->state_out->hierarchy->numNodes);
}

void a3hierarchyBlendExec4C1I(a3_HierarchyBlendNode* node_inout)
{
	node_inout->operation(node_inout->state_out->sampleHPose,
		node_inout->controlStates[0],
		node_inout->controlStates[1],
		node_inout->controlStates[2],
		node_inout->controlStates[3],
		&node_inout->uVals[0],
		node_inout->state_out->hierarchy->numNodes);
}

void a3hierarchyBlendExec4C3I(a3_HierarchyBlendNode* node_inout)
{
	node_inout->operation(node_inout->state_out->sampleHPose,
		node_inout->controlStates[0],
		node_inout->controlStates[1],
		node_inout->controlStates[2],
		node_inout->controlStates[3],
		&node_inout->uVals[0], &node_inout->uVals[1], &node_inout->uVals[2],
		node_inout->state_out->hierarchy->numNodes);
}

void a3hierarchyBlendExec16C5I(a3_HierarchyBlendNode* node_inout)
{
	node_inout->operation(node_inout->state_out->sampleHPose,
		node_inout->controlStates[0 * 4 + 0], node_inout->controlStates[0 * 4 + 1], node_inout->controlStates[0 * 4 + 2], node_inout->controlStates[0 * 4 + 3],
		node_inout->controlStates[1 * 4 + 0], node_inout->controlStates[1 * 4 + 1], node_inout->controlStates[1 * 4 + 2], node_inout->controlStates[1 * 4 + 3],
		node_inout->controlStates[2 * 4 + 0], node_inout->controlStates[2 * 4 + 1], node_inout->controlStates[2 * 4 + 2], node_inout->controlStates[2 * 4 + 3],
		node_inout->controlStates[3 * 4 + 0], node_inout->controlStates[3 * 4 + 1], node_inout->controlStates[3 * 4 + 2], node_inout->controlStates[3 * 4 + 3],
		&node_inout->uVals[0], &node_inout->uVals[1], &node_inout->uVals[2], &node_inout->uVals[3], &node_inout->uVals[4],
		node_inout->state_out->hierarchy->numNodes);
}

a3i32 a3hierarchyBlendNodeCreate(a3_HierarchyBlendTree* refTree, a3_HierarchyBlendNode* blendNode_out, NodeType type,
	a3ui32 clipCount, a3byte** clipNames,
	a3ui32 controlNodeCount, a3ui32* controlNodes,
	a3ui32 inputCount, a3f32** uValPtrs)
{
	blendNode_out->nodeType = type;
	//how do I bind the hPose????
	for (a3ui32 i = 0; i < clipCount; i++)
	{
		blendNode_out->clipNames[i] = clipNames[i];
	}
	blendNode_out->clipCount;
	for (a3ui32 i = 0; i < inputCount; i++)
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
	case copyClip:
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
	case fk:
		blendNode_out->exec = &a3hierarchyBlendExec0C;
		blendNode_out->operation = NULL;
		break;
	case ik:
		blendNode_out->exec = &a3hierarchyBlendExec0C;
		blendNode_out->operation = NULL; //this operation needs to call copy before doing anything
		break;
	}
	return 1;
}

a3i32 a3hierarchyBlendTreeLoad(a3_HierarchyBlendTree* blendTree_out, a3_Hierarchy* skeletal_hierarchy, const a3byte* resourceFilePath)
{
	if (blendTree_out && skeletal_hierarchy && !skeletal_hierarchy->numNodes && resourceFilePath && *resourceFilePath)
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
/// Pass in the tree, the demo state, and a pose group, and the tree will be configured appropriately.
/// </summary>
/// <param name="tree_in"></param>
/// <param name="demoState"></param>
/// <param name="selectedHierarchy"></param>
/// <returns></returns>
a3i32 a3hierarchyBlendTreeBind(a3_HierarchyBlendTree* tree_in, a3_DemoState* demoState, a3_HierarchyPoseGroup* poseGroup)
{
	if (tree_in && demoState)
	{
		//get total number of clip controllers to allocate
		a3i32 clipCtrlCount = 0;
		for (a3ui32 i = 0; i < tree_in->leafCount; i++)
		{
			clipCtrlCount += tree_in->blendNodes[tree_in->leafIndices[i]].clipCount;
		}

		demoState->demoMode1_animation->bTreeControllerCount = clipCtrlCount;
		if (demoState->demoMode1_animation->bTreeClipControllers)
		{
			free(demoState->demoMode1_animation->bTreeClipControllers); //clear out any old data
		}
		demoState->demoMode1_animation->bTreeClipControllers = malloc(clipCtrlCount * sizeof(a3_ClipController));

		a3ui32 controllerIndex = 0;

		//bind base inputs and init controllers
		for (a3ui32 leafIndex = 0; leafIndex < tree_in->leafCount; leafIndex++)
		{
			a3_HierarchyBlendNode* node = &tree_in->blendNodes[tree_in->leafIndices[leafIndex]];
			for (a3ui32 nodeClipIndex = 0; nodeClipIndex < node->clipCount; nodeClipIndex++)
			{
				//assign a clip controller and increment the controller index. Also assign the clip itself
				node->clipControllers[nodeClipIndex] = &demoState->demoMode1_animation->bTreeClipControllers[controllerIndex];
				a3i32 clipPoolIndex = a3clipGetIndexInPool(demoState->clipPool, node->clipNames[nodeClipIndex]);
				a3clipControllerInit(node->clipControllers[nodeClipIndex], "CTRL", demoState->clipPool, clipPoolIndex);
				controllerIndex++;
			}
		}

		//next is binding hstates. Total HStates = all clip inputs + all node outputs, or more conveniently, controllerCount + nodeCount
		demoState->demoMode1_animation->bTreeHStateCount = clipCtrlCount + tree_in->bt_hierarchy->numNodes;

		//free hstates if necessary
		if (demoState->demoMode1_animation->bTreeHStates)
		{
			free(demoState->demoMode1_animation->bTreeHStates);
		}

		//create new hstates
		demoState->demoMode1_animation->bTreeHStates = malloc(demoState->demoMode1_animation->bTreeHStateCount * sizeof(a3_HierarchyState));
		//init hstates
		for (int hStateIndex = 0; hStateIndex < demoState->demoMode1_animation->bTreeHStateCount; hStateIndex++)
		{
			a3hierarchyStateCreate(&demoState->demoMode1_animation->bTreeHStates[hStateIndex], poseGroup->hierarchy);
		}

		//bind hState outputs
		a3ui32 hStateBindIndex = 0;
		for (a3ui32 nodeIndex = 0; nodeIndex < tree_in->bt_hierarchy->numNodes; nodeIndex++)
		{
			//clip nodes have to also load data into their input states
			a3_HierarchyBlendNode* node = &tree_in->blendNodes[nodeIndex];
			if (node->nodeType < identity) //clip node
			{
				for (a3ui32 clipIndex = 0; clipIndex < node->clipCount; clipIndex++) //all clip counts combined = clipCtrlCount, then we add in one per node
				{
					node->controlStates[clipIndex] = &demoState->demoMode1_animation->bTreeHStates[hStateBindIndex];
					hStateBindIndex++;
				}
			}
			node->state_out = &demoState->demoMode1_animation->bTreeHStates[hStateBindIndex];
			hStateBindIndex++;
		}

		//bind child poses!!
		for (a3ui32 nodeIndex = 0; nodeIndex < tree_in->bt_hierarchy->numNodes; nodeIndex++)
		{
			a3_HierarchyBlendNode* node = &tree_in->blendNodes[nodeIndex];
			if (node->nodeType >= identity) //internal node
			{
				//node index within node's control _node_index array
				for (a3ui32 controlIndex = 0; controlIndex < node->controlNodeCount; controlIndex++) //all clip counts combined = clipCtrlCount, then we add in one per node
				{
					a3ui32 controlNodeIndex = node->controlNodeIndices[controlIndex];
					node->controlStates[controlIndex] = tree_in->blendNodes[controlNodeIndex].state_out;
				}
			}
		}
		return 1;
	}
	return -1;
}