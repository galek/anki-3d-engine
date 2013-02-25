#include "anki/resource/MeshLoader.h"
#include "anki/util/BinaryStream.h"
#include <fstream>
#include <cstring>

namespace anki {

//==============================================================================
void MeshLoader::load(const char* filename)
{
	// Try
	try
	{
		// Open the file
		std::fstream file(filename, std::fstream::in | std::fstream::binary);

		if(!file.is_open())
		{
			throw ANKI_EXCEPTION("Cannot open file:" + filename);
		}

		BinaryStream bs(file.rdbuf());

		// Magic word
		char magic[8];
		bs.read(magic, 8);
		if(bs.fail() || memcmp(magic, "ANKIMESH", 8))
		{
			throw ANKI_EXCEPTION("Incorrect magic word");
		}

		// Mesh name
		std::string meshName = bs.readString();

		// Verts num
		uint vertsNum = bs.readUint();
		vertCoords.resize(vertsNum);

		// Vert coords
		for(Vec3& vertCoord : vertCoords)
		{
			for(uint j = 0; j < 3; j++)
			{
				vertCoord[j] = bs.readFloat();
			}
		}

		// Faces num
		uint facesNum = bs.readUint();
		tris.resize(facesNum);

		// Faces IDs
		for(Triangle& tri : tris)
		{
			for(uint j = 0; j < 3; j++)
			{
				tri.vertIds[j] = bs.readUint();

				// a sanity check
				if(tri.vertIds[j] >= vertCoords.size())
				{
					throw ANKI_EXCEPTION("Vert index out of bounds");
				}
			}
		}

		// Tex coords num
		uint texCoordsNum = bs.readUint();
		texCoords.resize(texCoordsNum);

		// Tex coords
		for(Vec2& texCoord : texCoords)
		{
			for(uint i = 0; i < 2; i++)
			{
				texCoord[i] = bs.readFloat();
			}
		}

		// Vert weights num
		uint vertWeightsNum = bs.readUint();
		vertWeights.resize(vertWeightsNum);

		// Vert weights
		for(VertexWeight& vw : vertWeights)
		{
			// get the bone connections num
			uint boneConnections = bs.readUint();

			// we treat as error if one vert doesnt have a bone
			if(boneConnections < 1)
			{
				throw ANKI_EXCEPTION("Vert sould have at least one bone");
			}

			// and here is another possible error
			if(boneConnections > VertexWeight::MAX_BONES_PER_VERT)
			{
				uint tmp = VertexWeight::MAX_BONES_PER_VERT;
				throw ANKI_EXCEPTION("Cannot have more than "
					+ std::to_string(tmp) + " bones per vertex");
			}
			vw.bonesNum = boneConnections;

			// for all the weights of the current vertes
			for(uint i = 0; i < vw.bonesNum; i++)
			{
				// read bone id
				uint boneId = bs.readUint();
				vw.boneIds[i] = boneId;

				// read the weight of that bone
				float weight = bs.readFloat();
				vw.weights[i] = weight;
			}
		} // end for all vert weights

		doPostLoad();
	}
	catch(Exception& e)
	{
		throw ANKI_EXCEPTION("Loading of file failed: " + filename) << e;
	}
}


//==============================================================================
void MeshLoader::doPostLoad()
{
	// Sanity checks
	if(vertCoords.size() < 1 || tris.size() < 1)
	{
		throw ANKI_EXCEPTION("Vert coords and tris must be filled");
	}
	if(texCoords.size() != 0 && texCoords.size() != vertCoords.size())
	{
		throw ANKI_EXCEPTION("Tex coords num must be "
			"zero or equal to the vertex "
			"coords num");
	}
	if(vertWeights.size() != 0 && vertWeights.size() != vertCoords.size())
	{
		throw ANKI_EXCEPTION("Vert weights num must be zero or equal to the "
			"vertex coords num");
	}

	createAllNormals();
	fixNormals();
	if(texCoords.size() > 0)
	{
		createVertTangents();
	}
	createVertIndeces();
}

//==============================================================================
void MeshLoader::createVertIndeces()
{
	vertIndices.resize(tris.size() * 3);
	for(uint i = 0; i < tris.size(); i++)
	{
		vertIndices[i * 3 + 0] = tris[i].vertIds[0];
		vertIndices[i * 3 + 1] = tris[i].vertIds[1];
		vertIndices[i * 3 + 2] = tris[i].vertIds[2];
	}
}

//==============================================================================
void MeshLoader::createFaceNormals()
{
	for(Triangle& tri : tris)
	{
		const Vec3& v0 = vertCoords[tri.vertIds[0]];
		const Vec3& v1 = vertCoords[tri.vertIds[1]];
		const Vec3& v2 = vertCoords[tri.vertIds[2]];

		tri.normal = (v1 - v0).cross(v2 - v0);

		tri.normal.normalize();
	}
}

//==============================================================================
void MeshLoader::createVertNormals()
{
	vertNormals.resize(vertCoords.size());

	for(Vec3& vertNormal : vertNormals)
	{
		vertNormal = Vec3(0.0, 0.0, 0.0);
	}

	for(Triangle& tri : tris)
	{
		vertNormals[tri.vertIds[0]] += tri.normal;
		vertNormals[tri.vertIds[1]] += tri.normal;
		vertNormals[tri.vertIds[2]] += tri.normal;
	}

	for(Vec3& vertNormal : vertNormals)
	{
		vertNormal.normalize();
	}
}

//==============================================================================
void MeshLoader::createVertTangents()
{
	vertTangents.resize(vertCoords.size(), Vec4(0.0)); // alloc
	Vector<Vec3> bitagents(vertCoords.size(), Vec3(0.0));

	for(uint i = 0; i < tris.size(); i++)
	{
		const Triangle& tri = tris[i];
		const int i0 = tri.vertIds[0];
		const int i1 = tri.vertIds[1];
		const int i2 = tri.vertIds[2];
		const Vec3& v0 = vertCoords[i0];
		const Vec3& v1 = vertCoords[i1];
		const Vec3& v2 = vertCoords[i2];
		Vec3 edge01 = v1 - v0;
		Vec3 edge02 = v2 - v0;
		Vec2 uvedge01 = texCoords[i1] - texCoords[i0];
		Vec2 uvedge02 = texCoords[i2] - texCoords[i0];


		float det = (uvedge01.y() * uvedge02.x()) -
			(uvedge01.x() * uvedge02.y());
		if(isZero(det))
		{
			//ANKI_LOGW(getRsrcName() << ": det == " << fixed << det);
			det = 0.0001;
		}
		else
		{
			det = 1.0 / det;
		}

		Vec3 t = (edge02 * uvedge01.y() - edge01 * uvedge02.y()) * det;
		Vec3 b = (edge02 * uvedge01.x() - edge01 * uvedge02.x()) * det;
		t.normalize();
		b.normalize();

		vertTangents[i0] += Vec4(t, 1.0);
		vertTangents[i1] += Vec4(t, 1.0);
		vertTangents[i2] += Vec4(t, 1.0);

		bitagents[i0] += b;
		bitagents[i1] += b;
		bitagents[i2] += b;
	}

	for(uint i = 0; i < vertTangents.size(); i++)
	{
		Vec3 t = Vec3(vertTangents[i]);
		const Vec3& n = vertNormals[i];
		Vec3& b = bitagents[i];

		//t = t - n * n.dot(t);
		t.normalize();

		b.normalize();

		float w = ((n.cross(t)).dot(b) < 0.0) ? 1.0 : -1.0;

		vertTangents[i] = Vec4(t, w);
	}
}

//==============================================================================
void MeshLoader::fixNormals()
{
	const F32 positionsDistanceThresh = getEpsilon<F32>() * getEpsilon<F32>();
	const F32 normalsDotThresh = cos(NORMALS_ANGLE_MERGE);

	for(U i = 1; i < vertCoords.size(); i++)
	{
		const Vec3& crntPos = vertCoords[i];
		Vec3& crntNormal = vertNormals[i];

		// Check the previous
		for(U j = 0; j < i; j++)
		{
			const Vec3& otherPos = vertCoords[j];
			Vec3& otherNormal = vertNormals[j];

			F32 distanceSq = crntPos.getDistanceSquared(otherPos);

			if(distanceSq <= positionsDistanceThresh)
			{
				F32 dot = crntNormal.dot(otherNormal);

				if(dot <= normalsDotThresh)
				{
					Vec3 newNormal = (crntNormal + otherNormal) * 0.5;
					newNormal.normalize();

					newNormal = otherNormal = newNormal;
				}
			}
		}
	}
}

} // end namespace anki
