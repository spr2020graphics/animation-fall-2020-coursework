#include "..\a3_HierarchyBlendNode.h"

/// <summary>
/// used for identity
/// </summary>
/// <param name="node_inout"></param>
void a3spatialBlendExec0C(a3_SpatialBlendNode* node_inout)
{
	node_inout->operation(node_inout->pose);
}

/// <summary>
/// Copy, const, negate
/// </summary>
/// <param name="node_inout"></param>
void a3spatialBlendExec1C(a3_SpatialBlendNode* node_inout)
{
}
