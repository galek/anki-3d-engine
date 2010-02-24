#ifndef _SKEL_MODEL_NODE_H_
#define _SKEL_MODEL_NODE_H_

#include "common.h"
#include "MeshNode.h" 


class MeshNode;
class SkelNode;


/** 
 * Skeleton model scene node
 * It is just a group node with a derived init
 */
class skelModelNode: public Node
{
	public:
		Vec<MeshNode*> meshNodes;
		SkelNode*   skelNode;
		
		skelModelNode(): Node(NT_SKEL_MODEL), skelNode(NULL) { isGroupNode = true; }
		void init( const char* filename );
		void deinit() {} ///< Do nothing because it loads no resources
		void render() {} ///< Do nothing
};

#endif
