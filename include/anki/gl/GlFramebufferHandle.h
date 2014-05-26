#ifndef ANKI_GL_GL_FRAMEBUFFER_HANDLE_H
#define ANKI_GL_GL_FRAMEBUFFER_HANDLE_H

#include "anki/gl/GlContainerHandle.h"
#include "anki/gl/GlFramebuffer.h"

namespace anki {

// Forward
class GlJobChainHandle;

/// @addtogroup opengl_misc
/// @{

/// Framebuffer handle
class GlFramebufferHandle: public GlContainerHandle<GlFramebuffer>
{
public:
	using Base = GlContainerHandle<GlFramebuffer>;
	using Attachment = GlFramebuffer::Attachment;

	/// @name Contructors/Destructor
	/// @{
	GlFramebufferHandle();

	/// Create a framebuffer
	explicit GlFramebufferHandle(
		GlJobChainHandle& jobs,
		const std::initializer_list<Attachment>& attachments);

	~GlFramebufferHandle();
	/// @}

	/// Bind it to the state
	/// @param jobs The job chain
	/// @param invalidate If true invalidate the FB after binding it
	void bind(GlJobChainHandle& jobs, Bool invalidate);

	/// Blit another framebuffer to this
	/// @param[in, out] jobs The job chain
	/// @param[in] b The sorce framebuffer
	/// @param[in] sourceRect The source rectangle
	/// @param[in] destRect The destination rectangle
	/// @param linear Perform linean filtering
	void blit(GlJobChainHandle& jobs,
		const GlFramebufferHandle& b, 
		const Array<F32, 4>& sourceRect,
		const Array<F32, 4>& destRect, Bool linear);
};

} // end namespace anki

#endif

