#pragma anki vertShaderBegins

layout(location = 0) in vec2 position;

uniform mat3 tranformation; ///< Position transformation
uniform mat3 textureTranformation; ///< Texture transformation

out vec2 vTexCoords;


void main(void)
{
	vTexCoords = (textureTranformation * vec3(position, 1.0)).xy;
	gl_Position = vec4(tranformation * position, 1.0);
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
