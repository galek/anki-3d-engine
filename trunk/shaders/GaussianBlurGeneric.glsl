/**
 * @file
 * Generic shader program for Gausian blur inspired by Daniel Rakos' article
 * http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/
 *
 * Switches: VPASS or HPASS, COL_RGBA or COL_RGB or COL_R
 *
 * This is an optimized version. See the clean one at rev213
 */

#pragma anki vertShaderBegins

#pragma anki attribute position 0
attribute vec2 position;

uniform float imgDimension = 0.0; ///< the img width for hspass or the img height for vpass

varying vec2 vTexCoords;
varying float vOffsets[2]; ///< For side pixels

const float _offset_[2] = float[](1.3846153846, 3.2307692308); ///< The offset of side pixels


void main()
{
	vTexCoords = position;

	vOffsets[0] = _offset_[0] / imgDimension;
	vOffsets[1] = _offset_[1] / imgDimension;

	gl_Position = vec4(position * 2.0 - 1.0, 0.0, 1.0);
}


#pragma anki fragShaderBegins


/*
 * Preprocessor switcher sanity checks
 */
#if !defined(VPASS) && !defined(HPASS)
	#error "See file"
#endif

#if !(defined(COL_RGBA) || defined(COL_RGB) || defined(COL_R))
	#error "See file"
#endif


uniform sampler2D img; ///< Input FAI
uniform float blurringDist = 0.0;

varying vec2 vTexCoords;
varying float vOffsets[2];

/*
 * Determin color type
 */
#if defined(COL_RGBA)
	#define COL_TYPE vec4
#elif defined(COL_RGB)
	#define COL_TYPE vec3
#elif defined(COL_R)
	#define COL_TYPE float
#endif

/*
 * Determine tex fetch
 */
#if defined(COL_RGBA)
	#define TEX_FETCH rgba
#elif defined(COL_RGB)
	#define TEX_FETCH rgb
#elif defined(COL_R)
	#define TEX_FETCH r
#endif


const float _firstWeight_ = 0.2255859375;
const float _weights_[2] = float[](0.314208984375, 0.06982421875);


void main()
{
	// the center (0,0) pixel
	COL_TYPE _col_ = texture2D(img, vTexCoords).TEX_FETCH * _firstWeight_;

	// side pixels
	for(int i=0; i<2; i++)
	{
		#if defined(HPASS)
			vec2 _texCoords_ = vec2(vTexCoords.x + blurringDist + vOffsets[i], vTexCoords.y);
			_col_ += texture2D(img, _texCoords_).TEX_FETCH * _weights_[i];

			_texCoords_.x = vTexCoords.x - blurringDist - vOffsets[i];
			_col_ += texture2D(img, _texCoords_).TEX_FETCH * _weights_[i];
		#elif defined(VPASS)
			vec2 _texCoords_ = vec2(vTexCoords.x, vTexCoords.y + blurringDist + vOffsets[i]);
			_col_ += texture2D(img, _texCoords_).TEX_FETCH * _weights_[i];

			_texCoords_.y = vTexCoords.y - blurringDist - vOffsets[i];
			_col_ += texture2D(img, _texCoords_).TEX_FETCH * _weights_[i];
		#endif
	}

	gl_FragData[0].TEX_FETCH = _col_;
}