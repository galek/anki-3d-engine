#pragma anki vertShaderBegins

#pragma anki include "shaders/SimpleVert.glsl"

#pragma anki fragShaderBegins

uniform sampler2D fai;

in vec2 vTexCoords;

layout(location = 0) out vec4 fFragColor;

void main()
{
	fFragColor = texture2D(fai, vTexCoords);
}
