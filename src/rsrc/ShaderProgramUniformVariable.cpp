#include "ShaderProgramUniformVariable.h"
#include "rsrc/ShaderProgram.h"
#include "rsrc/Texture.h"
#include "gl/GlStateMachine.h"


//==============================================================================
// doSanityChecks                                                              =
//==============================================================================
void ShaderProgramUniformVariable::doSanityChecks() const
{
	ASSERT(getLoc() != -1);
	ASSERT(GlStateMachineSingleton::get().getCurrentProgramGlId() ==
		getFatherSProg().getGlId());
	ASSERT(glGetUniformLocation(getFatherSProg().getGlId(),
		getName().c_str()) == getLoc());
}


//==============================================================================
// set uniforms                                                                =
//==============================================================================

void ShaderProgramUniformVariable::set(const float f[], uint size) const
{
	doSanityChecks();
	ASSERT(getGlDataType() == GL_FLOAT);

	if(size == 1)
	{
		glUniform1f(getLoc(), f[0]);
	}
	else
	{
		glUniform1fv(getLoc(), size, f);
	}
}


void ShaderProgramUniformVariable::set(const Vec2 v2[], uint size) const
{
	doSanityChecks();
	ASSERT(getGlDataType() == GL_FLOAT_VEC2);
	if(size == 1)
	{
		glUniform2f(getLoc(), v2[0].x(), v2[0].y());
	}
	else
	{
		glUniform2fv(getLoc(), size, &(const_cast<Vec2&>(v2[0]))[0]);
	}
}


void ShaderProgramUniformVariable::set(const Vec3 v3[], uint size) const
{
	doSanityChecks();
	ASSERT(getGlDataType() == GL_FLOAT_VEC3);

	if(size == 1)
	{
		glUniform3f(getLoc(), v3[0].x(), v3[0].y(), v3[0].z());
	}
	else
	{
		glUniform3fv(getLoc(), size, &(const_cast<Vec3&>(v3[0]))[0]);
	}
}


void ShaderProgramUniformVariable::set(const Vec4 v4[], uint size) const
{
	doSanityChecks();
	ASSERT(getGlDataType() == GL_FLOAT_VEC4);
	glUniform4fv(getLoc(), size, &(const_cast<Vec4&>(v4[0]))[0]);
}


void ShaderProgramUniformVariable::set(const Mat3 m3[], uint size) const
{
	doSanityChecks();
	ASSERT(getGlDataType() == GL_FLOAT_MAT3);
	glUniformMatrix3fv(getLoc(), size, true, &(m3[0])[0]);
}


void ShaderProgramUniformVariable::set(const Mat4 m4[], uint size) const
{
	doSanityChecks();
	ASSERT(getGlDataType() == GL_FLOAT_MAT4);
	glUniformMatrix4fv(getLoc(), size, true, &(m4[0])[0]);
}


void ShaderProgramUniformVariable::set(const Texture& tex, uint texUnit) const
{
	doSanityChecks();
	ASSERT(getGlDataType() == GL_SAMPLER_2D ||
		getGlDataType() == GL_SAMPLER_2D_SHADOW);
	tex.bind(texUnit);
	glUniform1i(getLoc(), texUnit);
}
