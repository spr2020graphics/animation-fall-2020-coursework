#ifndef A3_HIERARCHYBLENDNODE_H

#include "a3_SpatialPose.h"

typedef struct a3_SpatialBlendNode a3_SpatialBlendNode;
typedef void (*a3_SpatialBlendOp)(a3_SpatialPose* pose, ...);
typedef void (*a3_SpatialBlendExec)(a3_SpatialBlendNode* node_in);



struct a3_SpatialBlendNode
{
	a3f32* uVals[8];
};

#endif // !A3_HIERARCHYBLENDNODE_H