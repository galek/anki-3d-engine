#pragma anki include "shaders/MsBsCommon.glsl"

/// @name Attributes
/// @{
layout(location = POSITION_LOCATION) in highp vec3 position;
layout(location = TEXTURE_COORDINATE_LOCATION) in mediump vec2 texCoord;

#if PASS_COLOR || TESSELLATION
layout(location = NORMAL_LOCATION) in mediump vec3 normal;
#endif

#if PASS_COLOR
layout(location = TANGENT_LOCATION) in mediump vec4 tangent;
#endif
/// @}

/// @name Varyings
/// @{
out mediump vec2 vTexCoords;

#if INSTANCE_ID_FRAGMENT_SHADER
flat out uint vInstanceId;
#endif

#if PASS_COLOR || TESSELLATION
out mediump vec3 vNormal;
#endif

#if PASS_COLOR
out mediump vec4 vTangent;
out mediump vec3 vVertPosViewSpace; ///< For env mapping. AKA view vector
#endif

#if TESSELLATION
out highp vec3 vPosition;
#endif
/// @}

//==============================================================================
#define writePositionTexCoord_DEFINED
void writePositionTexCoord(in mat4 mvp)
{
#if PASS_DEPTH && LOD > 0
	// No tex coords for you
#else
	vTexCoords = texCoord;
#endif

#if TESSELLATION
	vPosition = position;
#else
	gl_Position = mvp * vec4(position, 1.0);
#endif
}

//==============================================================================
#define writePositionNormalTangentTexCoord_DEFINED
void writePositionNormalTangentTexCoord(in mat4 mvp, in mat3 normalMat)
{

#if TESSELLATION

	// Passthrough
	vNormal = normal;
#	if PASS_COLOR
	vTangent = tangent;
#	endif

#else

#	if PASS_COLOR
	vNormal = normalMat * normal;
	vTangent.xyz = normalMat * tangent.xyz;
	vTangent.w = tangent.w;
#	endif

#endif

	writePositionTexCoord(mvp);
}

//==============================================================================
#if PASS_COLOR
#define setVertPosViewSpace_DEFINED
void setVertPosViewSpace(in mat4 modelViewMat)
{
	vVertPosViewSpace = vec3(modelViewMat * vec4(position, 1.0));
}
#endif