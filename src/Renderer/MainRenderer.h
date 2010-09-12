#ifndef MAIN_RENDERER_H
#define MAIN_RENDERER_H

#include "Common.h"
#include "Renderer.h"


/**
 * Main onscreen renderer
 */
class MainRenderer: public Renderer
{
	public:
		Dbg dbg; ///< Debugging rendering stage. Only the main renderer has it

		MainRenderer(Object* parent); ///< The quality of the JPEG screenshots. From 0 to 100

		/**
		 * @name Setters & getters
		 */
		/**@{*/
		int& getScreenshotJpegQuality() {return screenshotJpegQuality;}
		void setScreenshotJpegQuality(int i) {screenshotJpegQuality = i;}
		float getRenderingQuality() const {return renderingQuality;}
		/**@}*/

		/**
		 * The same as Renderer::init but with additional initialization. @see Renderer::init
		 */
		void init(const RendererInitializer& initializer);

		/**
		 * The same as Renderer::render but in addition it renders the final FAI to the framebuffer
		 * @param cam @see Renderer::render
		 */
		void render(Camera& cam);

		/**
		 * Save the color buffer to a tga (lossless & uncompressed & slow) or jpeg (lossy & compressed * fast)
		 * @param filename The file to save
		 */
		void takeScreenshot(const char* filename);

	private:
		RsrcPtr<ShaderProg> sProg; ///< Final pass' shader program
		int screenshotJpegQuality; ///< The quality of the JPEG screenshots. From 0 to 100

		/**
		 * The global rendering quality of the raster image. Its a percentage of the application's window size. From
		 * 0.0(low) to 1.0(high)
		 */
		float renderingQuality;

		bool takeScreenshotTga(const char* filename);
		bool takeScreenshotJpeg(const char* filename);
		static void initGl();
};


inline MainRenderer::MainRenderer(Object* parent):
	Renderer(parent),
	dbg(*this),
	screenshotJpegQuality(90)
{}

#endif
