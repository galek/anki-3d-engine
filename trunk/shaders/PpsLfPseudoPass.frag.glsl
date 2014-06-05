// Copyright (C) 2014, Panagiotis Christopoulos Charitos.
// All rights reserved.
// Code licensed under the BSD License.
// http://www.anki3d.org/LICENSE

// Screen space lens flare. Used the technique from here
// http://john-chapman-graphics.blogspot.no/2013/02/pseudo-lens-flare.html

#pragma anki type frag
#pragma anki include "shaders/Common.glsl"

#define MAX_GHOSTS 4
#define GHOST_DISPERSAL (0.7)
#define HALO_WIDTH 0.4
#define CHROMATIC_DISTORTION 4.0
#define ENABLE_CHROMATIC_DISTORTION 1
#define ENABLE_HALO 1

layout(location = 0) in vec2 inTexCoord;

layout(binding = 0) uniform sampler2D uRt;
layout(binding = 1) uniform sampler2D uLensDirtTex;

layout(location = 0) out vec3 outColor;

vec3 textureDistorted(
	in sampler2D tex,
	in vec2 texcoord,
	in vec2 direction, // direction of distortion
	in vec3 distortion) // per-channel distortion factor  
{
#if ENABLE_CHROMATIC_DISTORTION
	return vec3(
		textureRt(tex, texcoord + direction * distortion.r).r,
		textureRt(tex, texcoord + direction * distortion.g).g,
		textureRt(tex, texcoord + direction * distortion.b).b);
#else
	return textureRt(tex, texcoord).rgb;
#endif
}

void main()
{
	vec2 texcoord = vec2(1.0) - inTexCoord;

	vec2 ghostVec = (vec2(0.5) - texcoord) * GHOST_DISPERSAL;

	const vec2 texelSize = 1.0 / vec2(TEX_DIMENSIONS);

	const vec3 distortion = vec3(
		-texelSize.x * CHROMATIC_DISTORTION, 
		0.0, 
		texelSize.x * CHROMATIC_DISTORTION);

	const float lenOfHalf = length(vec2(0.5));

	vec2 direction = normalize(ghostVec);

	vec3 result = vec3(0.0);

	// sample ghosts:  
	for(int i = 0; i < MAX_GHOSTS; ++i) 
	{ 
		vec2 offset = fract(texcoord + ghostVec * float(i));

		float weight = length(vec2(0.5) - offset) / lenOfHalf;
		weight = pow(1.0 - weight, 10.0);

		result += 
			textureDistorted(uRt, offset, direction, distortion) * weight;
	}

	// sample halo
#if ENABLE_HALO
	vec2 haloVec = normalize(ghostVec) * HALO_WIDTH;
	float weight = 
		length(vec2(0.5) - fract(texcoord + haloVec)) / lenOfHalf;
	weight = pow(1.0 - weight, 20.0);
	result += textureDistorted(uRt, texcoord + haloVec, direction, distortion) 
		* weight;
#endif

	// lens dirt
	result *= texture(uLensDirtTex, inTexCoord).rgb;

	// Write
	outColor = result;
}
