// Copyright (C) 2014, Panagiotis Christopoulos Charitos.
// All rights reserved.
// Code licensed under the BSD License.
// http://www.anki3d.org/LICENSE

#include "anki/renderer/DebugDrawer.h"
#include "anki/renderer/Renderer.h"
#include "anki/resource/ProgramResource.h"
#include "anki/Collision.h"
#include "anki/Scene.h"
#include "anki/resource/TextureResource.h"
#include "anki/renderer/Renderer.h"
#include "anki/core/Logger.h"

namespace anki {

//==============================================================================
// DebugDrawer                                                                 =
//==============================================================================

//==============================================================================
DebugDrawer::DebugDrawer(Renderer* r)
{
	GlDevice& gl = r->_getGlDevice();

	m_vert.load("shaders/Dbg.vert.glsl", &r->_getResourceManager());
	m_frag.load("shaders/Dbg.frag.glsl", &r->_getResourceManager());

	GlCommandBufferHandle jobs(&gl);

	m_ppline = GlProgramPipelineHandle(jobs, 
		{m_vert->getGlProgram(), m_frag->getGlProgram()});

	m_vertBuff = GlBufferHandle(jobs, GL_ARRAY_BUFFER, 
		sizeof(m_clientLineVerts), GL_DYNAMIC_STORAGE_BIT);

	m_lineVertCount = 0;
	m_triVertCount = 0;
	m_mMat.setIdentity();
	m_vpMat.setIdentity();
	m_mvpMat.setIdentity();
	m_crntCol = Vec3(1.0, 0.0, 0.0);

	jobs.finish();
}

//==============================================================================
DebugDrawer::~DebugDrawer()
{}

//==============================================================================
void DebugDrawer::setModelMatrix(const Mat4& m)
{
	m_mMat = m;
	m_mvpMat = m_vpMat * m_mMat;
}

//==============================================================================
void DebugDrawer::setViewProjectionMatrix(const Mat4& m)
{
	m_vpMat = m;
	m_mvpMat = m_vpMat * m_mMat;
}

//==============================================================================
void DebugDrawer::begin(GLenum primitive)
{
	ANKI_ASSERT(primitive == GL_TRIANGLES || primitive == GL_LINES);
	m_primitive = primitive;
}

//==============================================================================
void DebugDrawer::end()
{
	if(m_primitive == GL_LINES)
	{
		if(m_lineVertCount % 2 != 0)
		{
			pushBackVertex(Vec3(0.0));
			ANKI_LOGW("Forgot to close the line loop");
		}
	}
	else
	{
		if(m_triVertCount % 3 != 0)
		{
			pushBackVertex(Vec3(0.0));
			pushBackVertex(Vec3(0.0));
			ANKI_LOGW("Forgot to close the line loop");
		}
	}
}

//==============================================================================
void DebugDrawer::flush()
{
	flushInternal(GL_LINES);
	flushInternal(GL_TRIANGLES);
}

//==============================================================================
void DebugDrawer::flushInternal(GLenum primitive)
{
	if((primitive == GL_LINES && m_lineVertCount == 0)
		|| (primitive == GL_TRIANGLES && m_triVertCount == 0))
	{
		// Early exit
		return;
	}

	U clientVerts;
	void* vertBuff;
	if(primitive == GL_LINES)
	{
		clientVerts = m_lineVertCount;
		vertBuff = &m_clientLineVerts[0];
		m_lineVertCount = 0;
	}
	else
	{
		clientVerts = m_triVertCount;
		vertBuff = &m_clientTriVerts[0];
		m_triVertCount = 0;
	}

	U size = sizeof(Vertex) * clientVerts;

	GlClientBufferHandle tmpBuff(m_jobs, size, nullptr);
	memcpy(tmpBuff.getBaseAddress(), vertBuff, size);

	m_vertBuff.write(m_jobs, tmpBuff, 0, 0, size);

	m_ppline.bind(m_jobs);

	m_vertBuff.bindVertexBuffer(m_jobs, 
		4, GL_FLOAT, false, sizeof(Vertex), 0, 0); // Pos

	m_vertBuff.bindVertexBuffer(m_jobs, 
		4, GL_FLOAT, true, sizeof(Vertex), sizeof(Vec4), 1); // Color

	m_jobs.drawArrays(primitive, clientVerts);
}

//==============================================================================
void DebugDrawer::pushBackVertex(const Vec3& pos)
{
	U32* vertCount;
	Vertex* vertBuff;
	if(m_primitive == GL_LINES)
	{
		vertCount = &m_lineVertCount;
		vertBuff = &m_clientLineVerts[0];
	}
	else
	{
		vertCount = &m_triVertCount;
		vertBuff = &m_clientTriVerts[0];
	}

	vertBuff[*vertCount].m_position = m_mvpMat * Vec4(pos, 1.0);
	vertBuff[*vertCount].m_color = Vec4(m_crntCol, 1.0);

	++(*vertCount);

	if(*vertCount == MAX_POINTS_PER_DRAW)
	{
		flush();
	}
}

//==============================================================================
void DebugDrawer::drawLine(const Vec3& from, const Vec3& to, const Vec4& color)
{
	setColor(color);
	begin(GL_LINES);
		pushBackVertex(from);
		pushBackVertex(to);
	end();
}

//==============================================================================
void DebugDrawer::drawGrid()
{
	Vec4 col0(0.5, 0.5, 0.5, 1.0);
	Vec4 col1(0.0, 0.0, 1.0, 1.0);
	Vec4 col2(1.0, 0.0, 0.0, 1.0);

	const F32 SPACE = 1.0; // space between lines
	const U NUM = 57;  // lines number. must be odd

	const F32 GRID_HALF_SIZE = ((NUM - 1) * SPACE / 2);

	setColor(col0);

	begin(GL_LINES);

	for(U x = - NUM / 2 * SPACE; x < NUM / 2 * SPACE; x += SPACE)
	{
		setColor(col0);

		// if the middle line then change color
		if(x == 0)
		{
			setColor(col1);
		}

		// line in z
		pushBackVertex(Vec3(x, 0.0, -GRID_HALF_SIZE));
		pushBackVertex(Vec3(x, 0.0, GRID_HALF_SIZE));

		// if middle line change col so you can highlight the x-axis
		if(x == 0)
		{
			setColor(col2);
		}

		// line in the x
		pushBackVertex(Vec3(-GRID_HALF_SIZE, 0.0, x));
		pushBackVertex(Vec3(GRID_HALF_SIZE, 0.0, x));
	}

	// render
	end();
}

//==============================================================================
void DebugDrawer::drawSphere(F32 radius, I complexity)
{
	Vector<Vec3>* sphereLines;

	// Pre-calculate the sphere points5
	//
	std::unordered_map<U32, Vector<Vec3>>::iterator it =
		m_complexityToPreCalculatedSphere.find(complexity);

	if(it != m_complexityToPreCalculatedSphere.end()) // Found
	{
		sphereLines = &(it->second);
	}
	else // Not found
	{
		m_complexityToPreCalculatedSphere[complexity] = Vector<Vec3>();
		sphereLines = &m_complexityToPreCalculatedSphere[complexity];

		F32 fi = getPi<F32>() / complexity;

		Vec3 prev(1.0, 0.0, 0.0);
		for(F32 th = fi; th < getPi<F32>() * 2.0 + fi; th += fi)
		{
			Vec3 p = Mat3(Euler(0.0, th, 0.0)) * Vec3(1.0, 0.0, 0.0);

			for(F32 th2 = 0.0; th2 < getPi<F32>(); th2 += fi)
			{
				Mat3 rot(Euler(th2, 0.0, 0.0));

				Vec3 rotPrev = rot * prev;
				Vec3 rotP = rot * p;

				sphereLines->push_back(rotPrev);
				sphereLines->push_back(rotP);

				Mat3 rot2(Euler(0.0, 0.0, getPi<F32>() / 2));

				sphereLines->push_back(rot2 * rotPrev);
				sphereLines->push_back(rot2 * rotP);
			}

			prev = p;
		}
	}

	// Render
	//
	Mat4 oldMMat = m_mMat;
	Mat4 oldVpMat = m_vpMat;

	setModelMatrix(m_mMat * Mat4(Vec4(0.0, 0.0, 0.0, 1.0), 
		Mat3::getIdentity(), radius));

	begin(GL_LINES);
	for(const Vec3& p : *sphereLines)
	{
		pushBackVertex(p);
	}
	end();

	// restore
	m_mMat = oldMMat;
	m_vpMat = oldVpMat;
}

//==============================================================================
void DebugDrawer::drawCube(F32 size)
{
	Vec3 maxPos = Vec3(0.5 * size);
	Vec3 minPos = Vec3(-0.5 * size);

	Array<Vec3, 8> points = {{
		Vec3(maxPos.x(), maxPos.y(), maxPos.z()),  // right top front
		Vec3(minPos.x(), maxPos.y(), maxPos.z()),  // left top front
		Vec3(minPos.x(), minPos.y(), maxPos.z()),  // left bottom front
		Vec3(maxPos.x(), minPos.y(), maxPos.z()),  // right bottom front
		Vec3(maxPos.x(), maxPos.y(), minPos.z()),  // right top back
		Vec3(minPos.x(), maxPos.y(), minPos.z()),  // left top back
		Vec3(minPos.x(), minPos.y(), minPos.z()),  // left bottom back
		Vec3(maxPos.x(), minPos.y(), minPos.z())   // right bottom back
	}};

	static const Array<U32, 24> indeces = {{
		0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6, 
		6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7}};

	begin(GL_LINES);
		for(U32 id : indeces)
		{
			pushBackVertex(points[id]);
		}
	end();
}

//==============================================================================
// CollisionDebugDrawer                                                        =
//==============================================================================

//==============================================================================
void CollisionDebugDrawer::visit(const Sphere& sphere)
{
	m_dbg->setModelMatrix(
		Mat4(sphere.getCenter().xyz1(), Mat3::getIdentity(), 1.0));
	m_dbg->drawSphere(sphere.getRadius());
}

//==============================================================================
void CollisionDebugDrawer::visit(const Obb& obb)
{
	Mat4 scale(Mat4::getIdentity());
	scale(0, 0) = obb.getExtend().x();
	scale(1, 1) = obb.getExtend().y();
	scale(2, 2) = obb.getExtend().z();

	Mat4 tsl(Transform(obb.getCenter(), obb.getRotation(), 1.0));

	m_dbg->setModelMatrix(tsl * scale);
	m_dbg->drawCube(2.0);
}

//==============================================================================
void CollisionDebugDrawer::visit(const Plane& plane)
{
	Vec3 n = plane.getNormal().xyz();
	const F32& o = plane.getOffset();
	Quat q;
	q.setFrom2Vec3(Vec3(0.0, 0.0, 1.0), n);
	Mat3 rot(q);
	rot.rotateXAxis(getPi<F32>() / 2.0);
	Mat4 trf(Vec4(n * o, 1.0), rot);

	m_dbg->setModelMatrix(trf);
	m_dbg->drawGrid();
}

//==============================================================================
void CollisionDebugDrawer::visit(const LineSegment& ls)
{
	m_dbg->setModelMatrix(Mat4::getIdentity());
	m_dbg->begin(GL_LINES);
	m_dbg->pushBackVertex(ls.getOrigin().xyz());
	m_dbg->pushBackVertex((ls.getOrigin() + ls.getDirection()).xyz());
	m_dbg->end();
}

//==============================================================================
void CollisionDebugDrawer::visit(const Aabb& aabb)
{
	Vec3 min = aabb.getMin().xyz();
	Vec3 max = aabb.getMax().xyz();

	Mat4 trf = Mat4::getIdentity();
	// Scale
	for(U32 i = 0; i < 3; ++i)
	{
		trf(i, i) = max[i] - min[i];
	}

	// Translation
	trf.setTranslationPart(Vec4((max + min) / 2.0, 1.0));

	m_dbg->setModelMatrix(trf);
	m_dbg->drawCube();
}

//==============================================================================
void CollisionDebugDrawer::visit(const Frustum& f)
{
	switch(f.getType())
	{
	case Frustum::Type::ORTHOGRAPHIC:
		visit(static_cast<const OrthographicFrustum&>(f).getObb());
		break;
	case Frustum::Type::PERSPECTIVE:
		{
			const PerspectiveFrustum& pf =
				static_cast<const PerspectiveFrustum&>(f);

			F32 camLen = pf.getFar();
			F32 tmp0 = camLen / tan((getPi<F32>() - pf.getFovX()) * 0.5) 
				+ 0.001;
			F32 tmp1 = camLen * tan(pf.getFovY() * 0.5) + 0.001;

			Vec3 points[] = {
				Vec3(0.0, 0.0, 0.0), // 0: eye point
				Vec3(-tmp0, tmp1, -camLen), // 1: top left
				Vec3(-tmp0, -tmp1, -camLen), // 2: bottom left
				Vec3(tmp0, -tmp1, -camLen), // 3: bottom right
				Vec3(tmp0, tmp1, -camLen) // 4: top right
			};

			const U32 indeces[] = {0, 1, 0, 2, 0, 3, 0, 4, 1, 2, 2,
				3, 3, 4, 4, 1};

			m_dbg->begin(GL_LINES);
			for(U32 i = 0; i < sizeof(indeces) / sizeof(U32); i++)
			{
				m_dbg->pushBackVertex(points[indeces[i]]);
			}
			m_dbg->end();
			break;
		}
	}
}

//==============================================================================
// SceneDebugDrawer                                                            =
//==============================================================================

//==============================================================================
void SceneDebugDrawer::draw(SceneNode& node)
{
	MoveComponent* mv = node.tryGetComponent<MoveComponent>();
	if(mv)
	{
		m_dbg->setModelMatrix(Mat4(mv->getWorldTransform()));
	}
	else
	{
		m_dbg->setModelMatrix(Mat4::getIdentity());
	}

	FrustumComponent* fr = node.tryGetComponent<FrustumComponent>();
	if(fr)
	{
		draw(*fr);
	}

	node.iterateComponentsOfType<SpatialComponent>([&](SpatialComponent& sp)
	{
		draw(sp);
	});
}

//==============================================================================
void SceneDebugDrawer::draw(FrustumComponent& fr) const
{
	const Frustum& fs = fr.getFrustum();

	m_dbg->setColor(Vec3(1.0, 1.0, 0.0));
	CollisionDebugDrawer coldraw(m_dbg);
	fs.accept(coldraw);
}

//==============================================================================
void SceneDebugDrawer::draw(SpatialComponent& x) const
{
	if(!x.bitsEnabled(SpatialComponent::SF_VISIBLE_CAMERA))
	{
		return;
	}

	m_dbg->setColor(Vec3(1.0, 0.0, 1.0));
	CollisionDebugDrawer coldraw(m_dbg);
	x.getAabb().accept(coldraw);
}

//==============================================================================
void SceneDebugDrawer::draw(const Sector& sector)
{
	// Draw the sector
	if(sector.getVisibleByMask() == VB_NONE)
	{
		m_dbg->setColor(Vec3(1.0, 0.5, 0.5));
	}
	else
	{
		if(sector.getVisibleByMask() & VB_CAMERA)
		{
			m_dbg->setColor(Vec3(0.5, 1.0, 0.5));
		}
		else
		{
			m_dbg->setColor(Vec3(0.5, 0.5, 1.0));
		}
	}
	CollisionDebugDrawer v(m_dbg);
	sector.getAabb().accept(v);

	// Draw the portals
	m_dbg->setColor(Vec3(0.0, 0.0, 1.0));
	for(const Portal* portal : sector.getSectorGroup().getPortals())
	{
		if(portal->sectors[0] == &sector || portal->sectors[1] == &sector)
		{
			portal->shape.accept(v);
		}
	}
}

//==============================================================================
void SceneDebugDrawer::drawPath(const Path& path) const
{
	/*const U count = path.getPoints().size();

	m_dbg->setColor(Vec3(1.0, 1.0, 0.0));

	m_dbg->begin();
	
	for(U i = 0; i < count - 1; i++)
	{
		m_dbg->pushBackVertex(path.getPoints()[i].getPosition());
		m_dbg->pushBackVertex(path.getPoints()[i + 1].getPosition());
	}

	m_dbg->end();*/
}

}  // end namespace anki
