#include "Bs.h"
#include "Renderer.h"
#include "App.h"
#include "Scene.h"
#include "ShaderProg.h"


//======================================================================================================================
// createFbo                                                                                                           =
//======================================================================================================================
void Bs::createFbo()
{
	try
	{
		fbo.create();
		fbo.bind();

		fbo.setNumOfColorAttachements(1);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
		                       r.getPps().getPrePassFai().getGlId(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D,
		                       r.getMs().getDepthFai().getGlId(), 0);

		fbo.checkIfGood();

		fbo.unbind();
	}
	catch(std::exception& e)
	{
		throw EXCEPTION("Failed to create blending stage FBO");
	}
}


//======================================================================================================================
// createRefractFbo                                                                                                    =
//======================================================================================================================
void Bs::createRefractFbo()
{
	try
	{
		refractFbo.create();
		refractFbo.bind();

		refractFbo.setNumOfColorAttachements(1);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, refractFai.getGlId(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D,
		                       r.getMs().getDepthFai().getGlId(), 0);

		refractFbo.checkIfGood();

		refractFbo.unbind();
	}
	catch(std::exception& e)
	{
		throw EXCEPTION("Failed to create blending stage refract FBO");
	}
}


//======================================================================================================================
// init                                                                                                                =
//======================================================================================================================
void Bs::init(const RendererInitializer& /*initializer*/)
{
	createFbo();
	refractFai.createEmpty2D(r.getWidth(), r.getHeight(), GL_RGBA8, GL_RGBA, GL_FLOAT);
	createRefractFbo();
	refractSProg.loadRsrc("shaders/BsRefract.glsl");
}


//======================================================================================================================
// run                                                                                                                 =
//======================================================================================================================
void Bs::run()
{
	Renderer::setViewport(0, 0, r.getWidth(), r.getHeight());

	glDepthMask(false);

	// render the meshes
	/// @todo Uncomment this
	/*for(Vec<MeshNode*>::iterator it=app->getScene().meshNodes.begin(); it!=app->getScene().meshNodes.end(); it++)
	{
		MeshNode* meshNode = (*it);

		if(meshNode->mesh->material.get() == NULL)
		{
			throw EXCEPTION("Mesh \"" + meshNode->mesh->getRsrcName() + "\" doesnt have material" );
		}

		if(!meshNode->mesh->material->renderInBlendingStage())
		{
			continue;
		}

		// refracts
		if(meshNode->mesh->material->getStdUniVar(Material::SUV_PPS_PRE_PASS_FAI))
		{
			// render to the temp FAI
			refractFbo.bind();

			glEnable(GL_STENCIL_TEST);
			glClear(GL_STENCIL_BUFFER_BIT);
			glStencilFunc(GL_ALWAYS, 0x1, 0x1);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

			r.setupMaterial(*meshNode->mesh->material, *meshNode, r.getCamera());
			glDisable(GL_BLEND); // a hack
			meshNode->render();

			// render the temp FAI to prePassFai
			fbo.bind();

			glStencilFunc(GL_EQUAL, 0x1, 0x1);
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

			if(meshNode->mesh->material->renderInBlendingStage())
			{
				glEnable(GL_BLEND);
				glBlendFunc(meshNode->mesh->material->getBlendingSfactor(), meshNode->mesh->material->getBlendingDfactor());
			}
			else
			{
				glDisable(GL_BLEND);
			}

			refractSProg->bind();
			refractSProg->findUniVar("fai")->setTexture(refractFai, 0);

			r.drawQuad();

			// cleanup
			glStencilFunc(GL_ALWAYS, 0x1, 0x1);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
			glClear(GL_STENCIL_BUFFER_BIT);
			glDisable(GL_STENCIL_TEST);
		}
		else
		{
			fbo.bind();
			r.setupMaterial(*meshNode->mesh->material, *meshNode, r.getCamera());
			meshNode->render();
		}
	}*/

	glDepthMask(true);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // the rendering above fucks the polygon mode
	Fbo::unbind();
}
