#pragma anki vertShaderBegins

in vec2 position;

uniform mat3 tranformation; ///< Position transformation
uniform mat3 textureTranformation; ///< Texture transformation

out vec2 vTexCoords;


void main(void)
{
	vec3 pos3d = vec3(position, 0.0);
	vTexCoords = (textureTranformation * pos3d).xy;
	gl_Position = vec4(tranformation * pos3d, 1.0);
}

#pragma anki fragShaderBegins

in vec2 vTexCoords;

uniform sampler2D texture;
uniform vec4 color;

layout(location = 0) out vec4 fColor;


void main()
{
	vec4 texCol = texture2D(texture, vTexCoords).rgba * color;

	fColor = texCol;
}
