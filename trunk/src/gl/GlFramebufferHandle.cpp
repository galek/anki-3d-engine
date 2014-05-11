#include "anki/gl/GlFramebufferHandle.h"
#include "anki/gl/GlHandleDeferredDeleter.h"

namespace anki {

//==============================================================================
GlFramebufferHandle::GlFramebufferHandle()
{}

//==============================================================================
GlFramebufferHandle::GlFramebufferHandle(
	GlJobChainHandle& jobs,
	const std::initializer_list<Attachment>& attachments)
{
	using Attachments = 
		Array<Attachment, GlFramebuffer::MAX_COLOR_ATTACHMENTS + 1>;

	class Job: public GlJob
	{
	public:
		Attachments m_attachments;
		U8 m_size;
		GlFramebufferHandle m_fb;

		Job(GlFramebufferHandle& handle, const Attachments& attachments, U size)
			: m_attachments(attachments), m_size(size), m_fb(handle)
		{}

		void operator()(GlJobChain*)
		{
			Attachment* begin;
			Attachment* end;

			if(m_size > 0)
			{
				begin = &m_attachments[0];
				end = begin + m_size;
			}
			else
			{
				begin = end = nullptr;
			}

			GlFramebuffer fb(begin, end);
			m_fb._get() = std::move(fb);
			GlHandleState oldState = m_fb._setState(GlHandleState::CREATED);
			ANKI_ASSERT(oldState == GlHandleState::TO_BE_CREATED);
			(void)oldState;
		}
	};

	typedef GlGlobalHeapAllocator<GlFramebuffer> Alloc;
	typedef GlDeleteObjectJob<GlFramebuffer, Alloc> DeleteJob;
	typedef GlHandleDeferredDeleter<GlFramebuffer, Alloc, DeleteJob> Deleter;

	*static_cast<Base::Base*>(this) = Base::Base(
		&jobs._get().getJobManager().getManager(),
		jobs._get().getGlobalAllocator(), 
		Deleter());
	_setState(GlHandleState::TO_BE_CREATED);

	Attachments att;
	U i = 0;
	for(const Attachment& a : attachments)
	{
		att[i++] = a;
	}

	jobs._pushBackNewJob<Job>(*this, att, attachments.size());
}

//==============================================================================
GlFramebufferHandle::~GlFramebufferHandle()
{}

//==============================================================================
void GlFramebufferHandle::bind(GlJobChainHandle& jobs, Bool invalidate)
{
	class Job: public GlJob
	{
	public:
		GlFramebufferHandle m_fb;
		Bool8 m_invalidate;

		Job(GlFramebufferHandle& fb, Bool invalidate)
			: m_fb(fb), m_invalidate(invalidate)
		{}

		void operator()(GlJobChain*)
		{
			m_fb._get().bind(m_invalidate);
		}
	};

	jobs._pushBackNewJob<Job>(*this, invalidate);
}

} // end namespace anki

