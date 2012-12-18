#ifndef ANKI_SCENE_SKIN_NODE_H
#define ANKI_SCENE_SKIN_NODE_H

#include "anki/scene/SceneNode.h"
#include "anki/scene/Renderable.h"
#include "anki/scene/Movable.h"
#include "anki/scene/Spatial.h"
#include "anki/resource/Model.h"
#include "anki/math/Math.h"
#include "anki/util/Vector.h"

namespace anki {

class Skin;

/// Skin specific mesh. It contains a number of VBOs for transform feedback
class SkinMesh: public MeshBase
{
public:
	/// Create the @a tfVbos with empty data
	SkinMesh(const MeshBase* mesh);

	/// @name Accessors
	/// @{
	const Vbo& getXfbVbo() const
	{
		return vbo;
	}
	/// @}

	/// @name MeshBase implementers
	/// @{
	U32 getVerticesCount() const
	{
		return mesh->getVerticesCount();
	}

	U32 getIndicesCount(U32 lod) const
	{
		return mesh->getIndicesCount(lod);
	}

	U32 getTextureChannelsCount() const
	{
		return mesh->getTextureChannelsCount();
	}

	Bool hasWeights() const
	{
		return false;
	}

	U32 getLodsCount() const
	{
		return mesh->getLodsCount();
	}

	const Obb& getBoundingShape() const
	{
		return mesh->getBoundingShape();
	}

	void getVboInfo(
		const VertexAttribute attrib, const U32 lod, const Vbo*& vbo, 
		U32& size, GLenum& type, U32& stride, U32& offset) const;
	/// @}

private:
	Vbo vbo; ///< Contains the transformed P,N,T 
	const MeshBase* mesh; ///< The resource
};


/// Skin specific ModelPatch. It uses a SkinMesh to create the VAOs. It also
/// creates a VAO for the transform feedback pass
class SkinModelPatch: public ModelPatchBase
{
public:
	/// See TfHwSkinningGeneric.glsl for the locations
	enum XfbAttributeLocation
	{
		XFBAL_POSITION,
		XFBAL_NORMAL,
		XFBAL_TANGENT,
		XFBAL_BONES_COUNT,
		XFBAL_BONE_IDS,
		XFBAL_BONE_WEIGHTS
	};

	/// @name Constructors/Destructor
	/// @{
	SkinModelPatch(const ModelPatch* mpatch_);
	/// @}

	/// @name Accessors
	/// @{
	SkinMesh& getSkinMesh()
	{
		return *skinMesh;
	}

	const SkinMesh& getSkinMesh() const
	{
		return *skinMesh;
	}

	const Vao& getTransformFeedbackVao() const
	{
		return xfbVao;
	}
	/// @}

	/// @name Implementations of ModelPatchBase virtuals
	/// @{
	const MeshBase& getMeshBase() const
	{
		return *skinMesh;
	}

	const Material& getMaterial() const
	{
		return mpatch->getMaterial();
	}
	/// @}

private:
	std::unique_ptr<SkinMesh> skinMesh;
	const ModelPatch* mpatch;
	Vao xfbVao; ///< Used as a source VAO in XFB
};


/// A fragment of the SkinNode
class SkinPatchNode: public SceneNode, public Movable, public Renderable,
	public Spatial
{
public:
	/// @name Constructors/Destructor
	/// @{
	SkinPatchNode(const ModelPatch* modelPatch_,
		const char* name, Scene* scene, // Scene
		uint movableFlags, Movable* movParent, // Movable
		CollisionShape* spatialCs); // Spatial
	/// @}

	/// @name Accessors
	/// @{
	SkinModelPatch& getSkinModelPatch()
	{
		return *skinModelPatch;
	}
	const SkinModelPatch& getSkinModelPatch() const
	{
		return *skinModelPatch;
	}
	/// @}

	/// @name SceneNode virtuals
	/// @{

	/// Override SceneNode::getMovable()
	Movable* getMovable()
	{
		return this;
	}

	/// Override SceneNode::getSpatial()
	Spatial* getSpatial()
	{
		return this;
	}

	/// Override SceneNode::getRenderable
	Renderable* getRenderable()
	{
		return this;
	}
	/// @}

	/// @name Renderable virtuals
	/// @{

	/// Implements Renderable::getModelPatchBase
	const ModelPatchBase& getRenderableModelPatchBase() const
	{
		return *skinModelPatch;
	}

	/// Implements Renderable::getMaterial
	const Material& getRenderableMaterial() const
	{
		return skinModelPatch->getMaterial();
	}

	/// Overrides Renderable::getRenderableWorldTransforms
	const Transform* getRenderableWorldTransforms() const
	{
		return &getWorldTransform();
	}
	/// @}

private:
	std::unique_ptr<SkinModelPatch> skinModelPatch;
};

/// A skin scene node
class SkinNode: public SceneNode, public Movable
{
public:
	/// @name Constructors/Destructor
	/// @{
	SkinNode(const char* skinFname,
		const char* name, Scene* scene, // SceneNode
		uint movableFlags, Movable* movParent); // Movable

	~SkinNode();
	/// @}

	/// @name SceneNode virtuals
	/// @{

	/// Override SceneNode::getMovable()
	Movable* getMovable()
	{
		return this;
	}

	/// Update the animation stuff
	void frameUpdate(float prevUpdateTime, float crntTime, int frame);
	/// @}

	/// @name Movable virtuals
	/// @{

	/// Update boundingShapeWSpace from bone tails (not heads as well
	/// cause its faster that way). The tails come from the previous frame
	void movableUpdate();
	/// @}

	/// @name Accessors
	/// @{
	const Vector<Vec3>& getHeads() const
	{
		return heads;
	}

	const Vector<Vec3>& getTails() const
	{
		return tails;
	}

	const Vector<Mat3>& getBoneRotations() const
	{
		return boneRotations;
	}

	const Vector<Vec3>& getBoneTranslations() const
	{
		return boneTranslations;
	}

	const PtrVector<SkinPatchNode>& getPatchNodes() const
	{
		return patches;
	}

	const Skin& getSkin() const
	{
		return *skin;
	}

	float getStep() const
	{
		return step;
	}
	float& getStep()
	{
		return step;
	}
	void setStep(const float x)
	{
		step = x;
	}

	float getFrame() const
	{
		return frame;
	}
	float& getFrame()
	{
		return frame;
	}
	void setFrame(const float x)
	{
		frame = x;
	}

	void setAnimation(const SkelAnim& anim_)
	{
		anim = &anim_;
	}
	const SkelAnim* getAnimation() const
	{
		return anim;
	}
	/// @}

private:
	SkinResourcePointer skin; ///< The resource
	PtrVector<SkinPatchNode> patches;
	Obb visibilityShapeWSpace;

	/// @name Animation stuff
	/// @{
	float step;
	float frame;
	const SkelAnim* anim; ///< The active skeleton animation
	/// @}

	/// @name Bone data
	/// @{
	Vector<Vec3> heads;
	Vector<Vec3> tails;
	Vector<Mat3> boneRotations;
	Vector<Vec3> boneTranslations;
	/// @}

	/// Interpolate
	/// @param[in] animation Animation
	/// @param[in] frame Frame
	/// @param[out] translations Translations vector
	/// @param[out] rotations Rotations vector
	static void interpolate(const SkelAnim& animation, float frame,
		Vector<Vec3>& translations, std::vector<Mat3>& rotations);

	/// Calculate the global pose
	static void updateBoneTransforms(const Skeleton& skel,
		Vector<Vec3>& translations, std::vector<Mat3>& rotations);

	/// Deform the heads and tails
	static void deformHeadsTails(const Skeleton& skeleton,
		const Vector<Vec3>& boneTranslations,
		const Vector<Mat3>& boneRotations,
		Vector<Vec3>& heads, std::vector<Vec3>& tails);
};

} // end namespace

#endif