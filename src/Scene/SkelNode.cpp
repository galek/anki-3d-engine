#include "SkelNode.h"
#include "SkelAnim.h"
#include "Skeleton.h"
#include "SkelAnimCtrl.h"
#include "App.h"
#include "MainRenderer.h"


//======================================================================================================================
// SkelNode                                                                                                         =
//======================================================================================================================
SkelNode::SkelNode(): 
	SceneNode(SNT_SKELETON)
{}


//======================================================================================================================
// init                                                                                                                =
//======================================================================================================================
void SkelNode::init(const char* filename)
{
	skeleton.loadRsrc(filename);
	heads.resize(skeleton->bones.size());
	tails.resize(skeleton->bones.size());
	boneRotations.resize(skeleton->bones.size());
	boneTranslations.resize(skeleton->bones.size());
}


//======================================================================================================================
// render                                                                                                              =
//======================================================================================================================
void SkelNode::render()
{
	app->getMainRenderer().getDbg().setModelMat(Mat4(getWorldTransform()));
	app->getMainRenderer().getDbg().setColor(Vec4(1.0, 0.0, 0.0, 1.0));
	app->getMainRenderer().getDbg().setModelMat(Mat4(getWorldTransform()));

	Vec<Vec3> positions;

	for(uint i=0; i<skeleton->bones.size(); i++)
	{
		app->getMainRenderer().getDbg().drawLine(heads[i], tails[i], Vec4(1.0));
	}
}
