#include "anki/resource/Material.h"
#include "anki/misc/PropertyTree.h"
#include "anki/resource/MaterialShaderProgramCreator.h"
#include "anki/core/App.h"
#include "anki/util/Filesystem.h"
#include "anki/resource/ShaderProgramResource.h"
#include "anki/resource/TextureResource.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <functional>
#include <algorithm>
#include <map>

namespace anki {

//==============================================================================
// MaterialVariable                                                            =
//==============================================================================

//==============================================================================
MaterialVariable::~MaterialVariable()
{}

//==============================================================================
GLenum MaterialVariable::getGlDataType() const
{
	return oneSProgVar->getGlDataType();
}

//==============================================================================
const std::string& MaterialVariable::getName() const
{
	return oneSProgVar->getName();
}

//==============================================================================
void MaterialVariable::init(const char* shaderProgVarName,
	PassLevelToShaderProgramHashMap& sProgs)
{
	oneSProgVar = NULL;

	// For all programs
	PassLevelToShaderProgramHashMap::iterator it = sProgs.begin();
	for(; it != sProgs.end(); ++it)
	{
		const ShaderProgram& sProg = *(it->second);
		const PassLevelKey& key = it->first;

		// Variable exists put it the map
		const ShaderProgramUniformVariable* uni = 
			sProg.tryFindUniformVariable(shaderProgVarName);
		if(uni)
		{
			sProgVars[key] = uni;

			// Set oneSProgVar
			if(!oneSProgVar)
			{
				oneSProgVar = uni;
			}

			// Sanity check: All the sprog vars need to have same GL data type
			if(oneSProgVar->getGlDataType() != uni->getGlDataType() 
				|| oneSProgVar->getType() != uni->getType())
			{
				throw ANKI_EXCEPTION("Incompatible shader "
					"program variables: " 
					+ shaderProgVarName);
			}
		}
	}

	// Extra sanity checks
	if(!oneSProgVar)
	{
		throw ANKI_EXCEPTION("Variable not found in "
			"any of the shader programs: " 
			+ shaderProgVarName);
	}
}

//==============================================================================
// Material                                                                    =
//==============================================================================

//==============================================================================
Material::Material()
{}

//==============================================================================
Material::~Material()
{}

//==============================================================================
void Material::load(const char* filename)
{
	fname = filename;
	try
	{
		using namespace boost::property_tree;
		ptree pt;
		read_xml(filename, pt);
		parseMaterialTag(pt.get_child("material"));
	}
	catch(std::exception& e)
	{
		throw ANKI_EXCEPTION("Failed to load file: " + filename) << e;
	}
}

//==============================================================================
void Material::parseMaterialTag(const boost::property_tree::ptree& pt)
{
	using namespace boost::property_tree;

	// renderingStage
	//
	renderingStage = pt.get<int>("renderingStage");

	// passes
	//
	boost::optional<std::string> pass =
		pt.get_optional<std::string>("passes");

	if(pass)
	{
		passes = StringList::splitString(pass.get().c_str(), ' ');
	}
	else
	{
		passes.push_back("DUMMY");
	}

	// levelsOfDetail
	//
	boost::optional<int> lod = pt.get_optional<int>("levelsOfDetail");

	if(lod)
	{
		levelsOfDetail = (lod.get() < 1) ? 1 : lod.get();
	}
	else
	{
		levelsOfDetail = 1;
	}

	// shadow
	//
	boost::optional<int> sw = pt.get_optional<int>("shadow");

	if(sw)
	{
		shadow = sw.get();
	}

	// blendFunctions
	//
	boost::optional<const ptree&> blendFuncsTree =
		pt.get_child_optional("blendFunctions");
	if(blendFuncsTree)
	{
		// sFactor
		{
			const std::string& tmp =
				blendFuncsTree.get().get<std::string>("sFactor");
			blendingSfactor = blendToEnum(tmp.c_str());
		}

		// dFactor
		{
			const std::string& tmp =
				blendFuncsTree.get().get<std::string>("dFactor");
			blendingDfactor = blendToEnum(tmp.c_str());
		}
	}

	// depthTesting
	//
	boost::optional<int> dp = pt.get_optional<int>("depthTesting");

	if(dp)
	{
		depthTesting = dp.get();
	}

	// wireframe
	//
	boost::optional<int> wf = pt.get_optional<int>("wireframe");

	if(wf)
	{
		wireframe = wf.get();
	}

	// shaderProgram
	//
	MaterialShaderProgramCreator mspc(pt.get_child("shaderProgram"));

	for(uint level = 0; level < levelsOfDetail; ++level)
	{
		for(uint pid = 0; pid < passes.size(); ++pid)
		{
			std::stringstream src;

			src << "#define LOD " << level << std::endl;
			src << "#define PASS_" << passes[pid] << std::endl;
			src << mspc.getShaderProgramSource() << std::endl;

			std::string filename =
				createShaderProgSourceToCache(src.str().c_str());

			ShaderProgramResourcePointer* pptr =
				new ShaderProgramResourcePointer;

			pptr->load(filename.c_str());

			ShaderProgram* sprog = pptr->get();

			sProgs.push_back(pptr);

			eSProgs[PassLevelKey(pid, level)] = sprog;
		}
	}

	populateVariables(pt.get_child("shaderProgram"));
}

//==============================================================================
std::string Material::createShaderProgSourceToCache(const std::string& source)
{
	// Create the hash
	std::hash<std::string> stringHash;
	std::size_t h = stringHash(source);
	std::string prefix = std::to_string(h);

	// Create path
	std::string newfPathName =
		AppSingleton::get().getCachePath() + "/mtl_" + prefix + ".glsl";
	toNativePath(newfPathName.c_str());

	// If file not exists write it
	if(!fileExists(newfPathName.c_str()))
	{
		// If not create it
		std::ofstream f(newfPathName.c_str());
		if(!f.is_open())
		{
			throw ANKI_EXCEPTION("Cannot open file for writing: " 
				+ newfPathName);
		}

		f.write(source.c_str(), source.length());
		f.close();
	}

	return newfPathName;
}

//==============================================================================
void Material::populateVariables(const boost::property_tree::ptree& pt)
{
	using namespace boost::property_tree;

	// Get all names of all the uniforms. Dont duplicate
	//
	std::map<std::string, GLenum> allVarNames;

	for(const ShaderProgramResourcePointer& sProg : sProgs)
	{
		for(const ShaderProgramUniformVariable* v :
			sProg->getUniformVariables())
		{
			allVarNames[v->getName()] = v->getGlDataType();
		}
	}

	// Iterate all the <input> and get the value
	//
	std::map<std::string, std::string> nameToValue;

	for(const ptree::value_type& v : pt)
	{
		if(v.first != "shader")
		{
			throw ANKI_EXCEPTION("Expected \"shader\" tag and not: " +
				v.first);
		}

		boost::optional<const ptree&> insPt =
			v.second.get_child_optional("inputs");

		if(!insPt)
		{
			continue;
		}

		for(const ptree::value_type& vv : insPt.get())
		{
			if(vv.first != "input")
			{
				throw ANKI_EXCEPTION("Expected \"input\" tag and not: " 
					+ vv.first);
			}

			std::string name = vv.second.get<std::string>("name");
			std::string value = vv.second.get<std::string>("value");

			nameToValue[name] = value;

			// A simple warning
			std::map<std::string, GLenum>::const_iterator iit =
				allVarNames.find(name);

			if(iit == allVarNames.end())
			{
				ANKI_LOGW("Material input variable " << name
					<< " not used by shader program. Material:"
					<< fname);
			}
		}
	}

	// Now combine
	//
	std::map<std::string, GLenum>::const_iterator it = allVarNames.begin();
	for(; it != allVarNames.end(); it++)
	{
		std::string name = it->first;
		GLenum dataType = it->second;

		std::map<std::string, std::string>::const_iterator it1 =
			nameToValue.find(name);

		MaterialVariable* v = nullptr;

		// Not found
		if(it1 == nameToValue.end())
		{
			const char* n = name.c_str(); // Var name
			// Get the value
			switch(dataType)
			{
				// sampler2D
				case GL_SAMPLER_2D:
					v = new MaterialVariableTemplate<TextureResourcePointer>(
						n, eSProgs, TextureResourcePointer(), false);
					break;
				// float
				case GL_FLOAT:
					v = new MaterialVariableTemplate<float>(n, eSProgs, 
						float(), false);
					break;
				// vec2
				case GL_FLOAT_VEC2:
					v = new MaterialVariableTemplate<Vec2>(n, eSProgs,
						Vec2(), false);
					break;
				// vec3
				case GL_FLOAT_VEC3:
					v = new MaterialVariableTemplate<Vec3>(n, eSProgs,
						Vec3(), false);
					break;
				// vec4
				case GL_FLOAT_VEC4:
					v = new MaterialVariableTemplate<Vec4>(n, eSProgs,
						Vec4(), false);
					break;
				// mat3
				case GL_FLOAT_MAT3:
					v = new MaterialVariableTemplate<Mat3>(n, eSProgs,
						Mat3(), false);
					break;
				// mat4
				case GL_FLOAT_MAT4:
					v = new MaterialVariableTemplate<Mat4>(n, eSProgs,
						Mat4(), false);
					break;
				// default is error
				default:
					ANKI_ASSERT(0);
			}
		}
		else
		{
			std::string value = it1->second;
			//ANKI_LOGI("With value " << name << " " << value);
			const char* n = name.c_str();

			// Get the value
			switch(dataType)
			{
				// sampler2D
				case GL_SAMPLER_2D:
					v = new MaterialVariableTemplate<TextureResourcePointer>(
						n, eSProgs, 
						TextureResourcePointer(value.c_str()), true);
					break;
				// float
				case GL_FLOAT:
					v = new MaterialVariableTemplate<float>(n, eSProgs,
						std::stof(value), true);
					break;
				// vec2
				case GL_FLOAT_VEC2:
					v = new MaterialVariableTemplate<Vec2>(n, eSProgs,
						setMathType<Vec2, 2>(value.c_str()), true);
					break;
				// vec3
				case GL_FLOAT_VEC3:
					v = new MaterialVariableTemplate<Vec3>(n, eSProgs,
						setMathType<Vec3, 3>(value.c_str()), true);
					break;
				// vec4
				case GL_FLOAT_VEC4:
					v = new MaterialVariableTemplate<Vec4>(n, eSProgs,
						setMathType<Vec4, 4>(value.c_str()), true);
					break;
				// default is error
				default:
					ANKI_ASSERT(0);
			}
		}

		vars.push_back(v);
		nameToVar[v->getName().c_str()] = v;
	}
}

//==============================================================================
template<typename Type, size_t n>
Type Material::setMathType(const char* str)
{
	Type out;
	StringList sl = StringList::splitString(str, ' ');
	ANKI_ASSERT(sl.size() == n);

	for(uint i = 0; i < n; ++i)
	{
		out[i] = std::stof(sl[i]);
	}

	return out;
}

//==============================================================================
GLenum Material::blendToEnum(const char* str)
{
// Dont make idiotic mistakes
#define TXT_AND_ENUM(x) \
	if(strcmp(str, #x) == 0) { \
		return x; \
	}

	TXT_AND_ENUM(GL_ZERO)
	TXT_AND_ENUM(GL_ONE)
	TXT_AND_ENUM(GL_DST_COLOR)
	TXT_AND_ENUM(GL_ONE_MINUS_DST_COLOR)
	TXT_AND_ENUM(GL_SRC_ALPHA)
	TXT_AND_ENUM(GL_ONE_MINUS_SRC_ALPHA)
	TXT_AND_ENUM(GL_DST_ALPHA)
	TXT_AND_ENUM(GL_ONE_MINUS_DST_ALPHA)
	TXT_AND_ENUM(GL_SRC_ALPHA_SATURATE)
	TXT_AND_ENUM(GL_SRC_COLOR)
	TXT_AND_ENUM(GL_ONE_MINUS_SRC_COLOR);
	ANKI_ASSERT(0);
	throw ANKI_EXCEPTION("Incorrect blend enum: " + str);

#undef TXT_AND_ENUM
}

} // end namespace
