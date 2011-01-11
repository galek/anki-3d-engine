#ifndef MESH_H
#define MESH_H

#include <boost/array.hpp>
#include "Math.h"
#include "RsrcPtr.h"
#include "Vbo.h"
#include "Properties.h"


class MeshData;


/// Mesh Resource. It contains the geometry packed in VBOs
class Mesh
{
	public:
		/// Used in @ref vbos array
		enum Vbos
		{
			VBO_VERT_POSITIONS, ///< VBO never empty
			VBO_VERT_NORMALS, ///< VBO never empty
			VBO_VERT_TANGENTS, ///< VBO never empty
			VBO_TEX_COORDS, ///< VBO may be empty
			VBO_VERT_INDECES, ///< VBO never empty
			VBO_VERT_WEIGHTS, ///< VBO may be empty
			VBOS_NUM
		};

	PROPERTY_R(uint, vertIdsNum, getVertIdsNum)

	public:
		/// Default constructor
		Mesh() {}

		/// Does nothing
		~Mesh() {}

		/// Accessor
		const Vbo& getVbo(Vbos id) const {return vbos[id];}

		/// Implements @ref Resource::load
		void load(const char* filename);

		/// @name Ask for data
		/// @{
		bool hasTexCoords() const {return vbos[VBO_TEX_COORDS].isCreated();}
		bool hasVertWeights() const {return vbos[VBO_VERT_WEIGHTS].isCreated();}
		bool hasNormalsAndTangents() const;
		/// @}

	private:
		boost::array<Vbo, VBOS_NUM> vbos; ///< The vertex buffer objects

		/// Create the VBOs
		void createVbos(const MeshData& meshData);
};


inline bool Mesh::hasNormalsAndTangents() const
{
	return vbos[VBO_VERT_NORMALS].isCreated() && vbos[VBO_VERT_TANGENTS].isCreated();
}


#endif
