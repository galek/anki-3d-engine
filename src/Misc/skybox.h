#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#include "Common.h"
#include "Texture.h"
#include "Math.h"

class ShaderProg;

class Skybox
{
	protected:
		enum textures_e
		{
			FRONT,
			BACK,
			LEFT,
			RIGHT,
			UP,
			DOWN
		};

		Texture* textures[6];
		Texture* noise;
		ShaderProg* shader;

		float rotation_ang;

	public:
		Skybox() { rotation_ang=0.0; }

		bool load(const char* filenames[6]);
		void Render(const Mat3& rotation);
};


#endif