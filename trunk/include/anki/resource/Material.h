#ifndef ANKI_RESOURCE_MATERIAL_H
#define ANKI_RESOURCE_MATERIAL_H

#include "anki/resource/Resource.h"
#include "anki/resource/ShaderProgramResource.h"
#include "anki/resource/PassLodKey.h"
#include "anki/Math.h"
#include "anki/util/Visitor.h"
#include "anki/util/Dictionary.h"
#include "anki/util/NonCopyable.h"
#include <memory>

namespace anki {

// Forward
class XmlElement;
class MaterialShaderProgramCreator;

// Forward
template<typename T>
class MaterialVariableTemplate;

class MaterialVariable;

/// Material variable base. Its a visitable
typedef VisitableCommonBase<
	MaterialVariable,
	MaterialVariableTemplate<F32>,
	MaterialVariableTemplate<Vec2>,
	MaterialVariableTemplate<Vec3>,
	MaterialVariableTemplate<Vec4>,
	MaterialVariableTemplate<Mat3>,
	MaterialVariableTemplate<Mat4>,
	MaterialVariableTemplate<TextureResourcePointer>>
	MateriaVariableVisitable;

/// Holds the shader variables. Its a container for shader program variables
/// that share the same name
class MaterialVariable: public MateriaVariableVisitable, public NonCopyable
{
public:
	typedef MateriaVariableVisitable Base;

	/// @name Constructors & destructor
	/// @{
	MaterialVariable(
		const char* shaderProgVarName,
		PassLodArray<ShaderProgramResourcePointer>& progs)
	{
		init(shaderProgVarName, progs);
	}

	virtual ~MaterialVariable();
	/// @}

	/// @name Accessors
	/// @{
	template<typename T>
	const T* getValues() const
	{
		ANKI_ASSERT(Base::getVariadicTypeId<MaterialVariableTemplate<T>>()
			== Base::getVisitableTypeId());
		return static_cast<const MaterialVariableTemplate<T>*>(this)->get();
	}

	template<typename T>
	void setValues(const T* x, U32 size)
	{
		ANKI_ASSERT(Base::getVariadicTypeId<MaterialVariableTemplate<T>>()
			== Base::getVisitableTypeId());
		static_cast<MaterialVariableTemplate<T>*>(this)->set(x, size);
	}

	U32 getArraySize() const;

	/// Given a key return the uniform. If the uniform is not present in the
	/// LOD pass key then returns nullptr
	const ShaderProgramUniformVariable* findShaderProgramUniformVariable(
		const PassLodKey& key) const
	{
		return progVars[key.pass][key.lod];
	}

	/// Get the GL data type of all the shader program variables
	GLenum getGlDataType() const;

	/// Get the name of all the shader program variables
	const std::string& getName() const;

	const ShaderProgramUniformVariable&
		getAShaderProgramUniformVariable() const
	{
		return *oneSProgVar;
	}

	/// If false then it should be buildin
	virtual Bool hasValues() const = 0;
	/// @}

private:
	PassLodArray<const ShaderProgramUniformVariable*> progVars;

	/// Keep one ShaderProgramVariable here for easy access of the common
	/// variable stuff like name or GL data type etc
	const ShaderProgramUniformVariable* oneSProgVar;

	/// Common constructor code
	void init(const char* shaderProgVarName,
		PassLodArray<ShaderProgramResourcePointer>& progs);
};

/// Material variable with data. A complete type
template<typename Data>
class MaterialVariableTemplate: public MaterialVariable
{
public:
	typedef Data Type;

	/// @name Constructors/Destructor
	/// @{
	MaterialVariableTemplate(
		const char* shaderProgVarName,
		PassLodArray<ShaderProgramResourcePointer>& progs)
		: MaterialVariable(shaderProgVarName, progs)
	{
		setupVisitable(this);
	}

	~MaterialVariableTemplate()
	{}
	/// @}

	/// @name Accessors
	/// @{
	const Data* get() const
	{
		return (data.size() > 0) ? &data[0] : nullptr;
	}

	void set(const Data* x, U32 size)
	{
		if(size > 0)
		{
			data.insert(data.begin(), x, x + size);
		}
	}

	Bool hasValues() const
	{
		return data.size() > 0;
	}
	/// @}

private:
	Vector<Data> data;
};

/// Contains a few properties that other classes may use. For an explanation of
/// the variables refer to Material class documentation
class MaterialProperties
{
public:
	/// @name Accessors
	/// @{
	U getLevelsOfDetail() const
	{
		return lodsCount;
	}

	U getPassesCount() const
	{
		return passesCount;
	}

	Bool getShadow() const
	{
		return shadow;
	}

	GLenum getBlendingSfactor() const
	{
		return blendingSfactor;
	}

	GLenum getBlendingDfactor() const
	{
		return blendingDfactor;
	}

	Bool getDepthTestingEnabled() const
	{
		return depthTesting;
	}

	Bool getWireframe() const
	{
		return wireframe;
	}

	Bool getTessellation() const
	{
		return tessellation;
	}
	/// @}

	/// Check if blending is enabled
	Bool isBlendingEnabled() const
	{
		return blendingSfactor != GL_ONE || blendingDfactor != GL_ZERO;
	}

protected:
	GLenum blendingSfactor = GL_ONE; ///< Default GL_ONE
	GLenum blendingDfactor = GL_ZERO; ///< Default GL_ZERO

	Bool8 depthTesting = true;
	Bool8 wireframe = false;
	Bool8 shadow = true;
	Bool8 tessellation = false;

	U8 passesCount = 1;
	U8 lodsCount = 1;
};

/// Material resource
///
/// Every material keeps info of how to render a RenedrableNode. Using a node
/// based logic it creates a couple of shader programs dynamically. One for
/// color passes and one for depth. It also keeps two sets of material
/// variables. The first is the build in and the second the user defined.
/// During the renderer's shader setup the buildins will be set automatically,
/// for the user variables the user needs to have its value in the material
/// file. Some material variables may be present in both shader programs and
/// some in only one of them
///
/// Material XML file format:
/// @code
/// <material>
/// 	[<passes>COLOR DEPTH</passes>]
///
/// 	[<levelsOfDetail>N</levelsOfDetail>]
///
/// 	[<shadow>0 | 1</shadow>]
///
/// 	[<blendFunctions>
/// 		<sFactor>GL_SOMETHING</sFactor>
/// 		<dFactor>GL_SOMETHING</dFactor>
/// 	</blendFunctions>]
///
/// 	[<depthTesting>0 | 1</depthTesting>]
///
/// 	[<wireframe>0 | 1</wireframe>]
///
/// 	<shaderProgram>
///
///			[<inputs> (3)
///				<input>
///					<name>xx</name>
///					<type>any glsl type</type>
///					<value> (4)
///						[a_series_of_numbers |
///						path/to/image.tga]
///					</value>
///					[<const>0 | 1</const>] (5)
///				</input>
///			</inputs>]
///
/// 		<shader> (2)
/// 			<type>vertex | tc | te | geometry | fragment</type>
///
/// 			<includes>
/// 				<include>path/to/file.glsl</include>
/// 				<include>path/to/file2.glsl</include>
/// 			</includes>
///
/// 			<operations>
/// 				<operation>
/// 					<id>x</id>
/// 					<returnType>any glsl type or void</returnType>
/// 					<function>functionName</function>
/// 					[<arguments>
/// 						<argument>xx</argument>
/// 						<argument>yy</argument>
/// 					</arguments>]
/// 				</operation>
/// 			</operations>
/// 		</vertexShader>
///
/// 		<shader>...</shader>
/// 	</shaderProgram>
/// </material>
/// @endcode
/// (2): The order of the shaders is crucial
/// (3): AKA uniforms
/// (4): The \<value\> can be left empty for build-in variables
/// (5): The \<const\> will mark a variable as constant and it cannot be changed
///      at all. Defauls is 0
class Material: public MaterialProperties, public NonCopyable
{
public:
	typedef Vector<MaterialVariable*> VarsContainer;

	Material();
	~Material();

	/// @name Accessors
	/// @{

	/// Access the base class just for copying in other classes
	const MaterialProperties& getBaseClass() const
	{
		return *this;
	}

	// Variable accessors
	const VarsContainer& getVariables() const
	{
		return vars;
	}

	const ShaderProgramUniformBlock* getCommonUniformBlock() const
	{
		return commonUniformBlock;
	}
	/// @}

	const ShaderProgram& findShaderProgram(const PassLodKey& key) const
	{
		ANKI_ASSERT(progs[key.pass][key.lod].isLoaded());
		return *progs[key.pass][key.lod];
	}

	const ShaderProgram* tryFindShaderProgram(const PassLodKey& key) const
	{
		if(progs[key.pass][key.lod].isLoaded())
		{
			return progs[key.pass][key.lod].get();
		}
		else
		{
			return nullptr;
		}
	}

	/// Get by name
	const MaterialVariable* findVariableByName(const char* name) const
	{
		NameToVariableHashMap::const_iterator it = nameToVar.find(name);
		return (it == nameToVar.end()) ? nullptr : it->second;
	}

	/// Load a material file
	void load(const char* filename);

	/// For sorting
	Bool operator<(const Material& b) const
	{
		return hash < b.hash;
	}

private:
	typedef Dictionary<MaterialVariable*> NameToVariableHashMap;

	std::string fname; ///< filename

	/// All the material variables
	VarsContainer vars;

	NameToVariableHashMap nameToVar;

	/// The most important aspect of materials. These are all the shader
	/// programs per level and per pass. Their number are NP * NL where
	/// NP is the number of passes and NL the number of levels of detail
	PassLodArray<ShaderProgramResourcePointer> progs;

	/// Used for sorting
	std::size_t hash;

	/// One uniform block
	const ShaderProgramUniformBlock* commonUniformBlock;

	/// Parse what is within the @code <material></material> @endcode
	void parseMaterialTag(const XmlElement& el);

	/// Create a unique shader source in chache. If already exists do nothing
	std::string createShaderProgSourceToCache(const std::string& source);

	/// Read all shader programs and pupulate the @a vars and @a nameToVar
	/// containers
	void populateVariables(const MaterialShaderProgramCreator& mspc);
};

} // end namespace anki

#endif
