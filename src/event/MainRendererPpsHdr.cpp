#include "MainRendererPpsHdr.h"
#include "r/MainRenderer.h"
#include "core/Globals.h"


namespace event {


//==============================================================================
// Constructor                                                                 =
//==============================================================================
MainRendererPpsHdr::MainRendererPpsHdr(float startTime, float duration,
	float exposure_, uint blurringIterationsNum_, float blurringDist_)
:	Event(MAIN_RENDERER_PPS_HDR, startTime, duration)
{
	finalData.exposure = exposure_;
	finalData.blurringIterationsNum = blurringIterationsNum_;
	finalData.blurringDist = blurringDist_;

	const r::Hdr& hdr =
		r::MainRendererSingleton::get().getPps().getHdr();
	originalData.exposure = hdr.getExposure();
	originalData.blurringIterationsNum = hdr.getBlurringIterationsNum();
	originalData.blurringDist = hdr.getBlurringDist();
}


//==============================================================================
// Copy constructor                                                            =
//==============================================================================
MainRendererPpsHdr::MainRendererPpsHdr(const MainRendererPpsHdr& b)
:	Event(MAIN_RENDERER_PPS_HDR, 0.0, 0.0)
{
	*this = b;
}


//==============================================================================
// operator=                                                                   =
//==============================================================================
MainRendererPpsHdr& MainRendererPpsHdr::operator=(const MainRendererPpsHdr& b)
{
	Event::operator=(b);
	finalData = b.finalData;
	originalData = b.originalData;
	return *this;
}


//==============================================================================
// updateSp                                                                    =
//==============================================================================
void MainRendererPpsHdr::updateSp(float /*prevUpdateTime*/, float crntTime)
{
	float d = crntTime - getStartTime(); // delta
	float dp = d / getDuration(); // delta as percentage

	r::Hdr& hdr = r::MainRendererSingleton::get().getPps().getHdr();

	hdr.setExposure(interpolate(originalData.exposure, finalData.exposure, dp));

	hdr.setBlurringIterationsNum(interpolate(originalData.blurringIterationsNum,
		finalData.blurringIterationsNum, dp));

	hdr.setBlurringDist(interpolate(originalData.blurringDist,
		finalData.blurringDist, dp));
}


} // end namespace
