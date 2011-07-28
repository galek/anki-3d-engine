#ifndef SHADER_PROG_H
#define SHADER_PROG_H

#include <boost/ptr_container/ptr_vector.hpp>
#include <GL/glew.h>
#include <limits>
#include "Util/ConstCharPtrHashMap.h"
#include "Util/Assert.h"
#include "SProgUniVar.h"
#include "SProgAttribVar.h"
#include "Util/Vec.h"
#include "GfxApi/GlStateMachine.h"
#include "Core/Globals.h"


class ShaderProgramPrePreprocessor;


/// Shader program @ref Resource
///
/// Shader program. Combines a fragment and a vertex shader. Every shader
/// program consist of one OpenGL ID, a vector of uniform variables and a
/// vector of attribute variables. Every variable is a struct that contains
/// the variable's name, location, OpenGL data type and if it is a uniform or
/// an attribute var.
class ShaderProg
{
	public:
		ShaderProg();
		~ShaderProg();

		/// @name Accessors
		/// @{
		GLuint getGlId() const;
		GETTER_R(boost::ptr_vector<SProgVar>, vars, getVariables)
		/// @}

		/// Resource load
		void load(const char* filename);

		/// Bind the shader program
		void bind() const;

		/// @name Variable getters
		/// Used to find and return the variable. They throw exception if
		/// variable not found so ask if the variable with that name exists
		/// prior using any of these
		/// @{
		const SProgVar& getVariable(const char* varName) const;
		const SProgUniVar& getUniformVariable(const char* varName) const;
		const SProgAttribVar& getAttributeVariable(const char* varName) const;
		/// @}

		/// @name Check for variable existance
		/// @{
		bool variableExists(const char* varName) const;
		bool uniformVariableExists(const char* varName) const;
		bool attributeVariableExists(const char* varName) const;
		/// @}

		/// Used by @ref Material and @ref Renderer to create custom shaders in
		/// the cache
		/// @param sProgFPathName The file pathname of the shader prog
		/// @param preAppendedSrcCode The source code we want to write on top
		/// of the shader prog
		/// @param newFNamePrefix The prefix of the new shader prog
		/// @return The file pathname of the new shader prog. Its
		/// $HOME/.anki/cache/newFNamePrefix_fName
		static std::string createSrcCodeToCache(const char* sProgFPathName,
			const char* preAppendedSrcCode);

		/// For debuging
		std::string getShaderInfoString() const;

	private:
		/// XXX
		typedef ConstCharPtrHashMap<SProgVar*>::Type VarsHashMap;

		/// XXX
		typedef ConstCharPtrHashMap<SProgUniVar*>::Type UniVarsHashMap;

		/// XXX
		typedef ConstCharPtrHashMap<SProgAttribVar*>::Type AttribVarsHashMap;

		std::string rsrcFilename;
		GLuint glId; ///< The OpenGL ID of the shader program
		GLuint vertShaderGlId; ///< Vertex shader OpenGL id
		GLuint geomShaderGlId; ///< Geometry shader OpenGL id
		GLuint fragShaderGlId; ///< Fragment shader OpenGL id

		/// Shader source that is used in ALL shader programs
		static std::string stdSourceCode;

		/// @name Containers
		/// @{
		boost::ptr_vector<SProgVar> vars; ///< All the vars
		VarsHashMap nameToVar; ///< Variable searching
		UniVarsHashMap nameToUniVar; ///< Uniform searching
		AttribVarsHashMap nameToAttribVar; ///< Attribute searching
		/// @}

		/// Query the driver to get the vars. After the linking of the shader
		/// prog is done gather all the vars in custom containers
		void getUniAndAttribVars();

		/// Create and compile shader
		/// @return The shader's OpenGL id
		/// @exception Exception
		uint createAndCompileShader(const char* sourceCode,
			const char* preproc, int type) const;

		/// Link the shader program
		/// @exception Exception
		void link() const;
}; 


//==============================================================================
// Inlines                                                                     =
//==============================================================================

inline ShaderProg::ShaderProg():
	glId(std::numeric_limits<uint>::max())
{}


inline GLuint ShaderProg::getGlId() const
{
	ASSERT(glId != std::numeric_limits<uint>::max());
	return glId;
}


inline void ShaderProg::bind() const
{
	ASSERT(glId != std::numeric_limits<uint>::max());
	GlStateMachineSingleton::getInstance().useShaderProg(glId);
}


#endif
