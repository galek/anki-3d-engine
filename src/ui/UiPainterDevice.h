#ifndef UI_PAINTER_DEVICE_H
#define UI_PAINTER_DEVICE_H

#include "gl/Fbo.h"
#include "math/Math.h"


class Texture;


/// This actually and FBO but with size info
class UiPainterDevice: public Fbo
{
	public:
		/// Constructor
		UiPainterDevice(Texture& colorFai);

		/// @name Accessors
		/// @{
		Vec2 getSize() const;
		/// @}

	private:
		Texture& colorFai;

		void create();
};


#endif
