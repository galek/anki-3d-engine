#include <stdio.h>
#include <iostream>
#include <fstream>
#include <typeinfo>
#include "common.h"

#include "input.h"
#include "Camera.h"
#include "Math.h"
#include "renderer.h"
#include "ui.h"
#include "app.h"
#include "particles.h"
#include "Texture.h"
#include "Mesh.h"
#include "Light.h"
#include "collision.h"
#include "Material.h"
#include "Resource.h"
#include "Scene.h"
#include "Scanner.h"
#include "skybox.h"
#include "map.h"
#include "MeshNode.h"
#include "SkelModelNode.h"
#include "MeshNode.h"
#include "SkelAnim.h"
#include "MeshSkelNodeCtrl.h"
#include "SkelAnimCtrl.h"
#include "SkelNode.h"
#include "LightProps.h"
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "BulletDebuger.h"


// map (hard coded)
Camera* main_cam;
MeshNode* floor__,* sarge,* horse;
skelModelNode* imp;
PointLight* point_lights[10];
SpotLight* spot_lights[2];

class floor_t: public Camera
{
	public:
		void render()
		{
			r::dbg::RenderCube( true, 1.0 );
		}

		void RenderDepth()
		{
			r::dbg::RenderCube( true, 1.0 );
		}
}* floor_;


// Physics
btDefaultCollisionConfiguration* collisionConfiguration;
btCollisionDispatcher* dispatcher;
btDbvtBroadphase* broadphase;
btSequentialImpulseConstraintSolver* sol;
btDiscreteDynamicsWorld* dynamicsWorld;
BulletDebuger debugDrawer;

#define ARRAY_SIZE_X 5
#define ARRAY_SIZE_Y 5
#define ARRAY_SIZE_Z 5

#define MAX_PROXIES (ARRAY_SIZE_X*ARRAY_SIZE_Y*ARRAY_SIZE_Z + 1024)

#define SCALING 1.
#define START_POS_X -5
#define START_POS_Y -5
#define START_POS_Z -3


void initPhysics()
{
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new	btCollisionDispatcher(collisionConfiguration);
	broadphase = new btDbvtBroadphase();
	sol = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld( dispatcher, broadphase, sol, collisionConfiguration );

	dynamicsWorld->setGravity(btVector3(0,-10,0));

	btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.),btScalar(50.),btScalar(50.)));

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0,-50,0));

	//We can also use DemoApplication::localCreateRigidBody, but for clarity it is provided here:
	{
		btScalar mass(0.);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0,0,0);
		if (isDynamic)
			groundShape->calculateLocalInertia(mass,localInertia);

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,groundShape,localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		//add the body to the dynamics world
		dynamicsWorld->addRigidBody(body);
	}


	{
		//create a few dynamic rigidbodies
		// Re-using the same collision is better for memory usage and performance

		btCollisionShape* colShape = new btBoxShape(btVector3(SCALING*1,SCALING*1,SCALING*1));
		//btCollisionShape* colShape = new btSphereShape(btScalar(1.));

		/// Create Dynamic Objects
		btTransform startTransform;
		startTransform.setIdentity();

		btScalar	mass(1.f);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0,0,0);
		if (isDynamic)
			colShape->calculateLocalInertia(mass,localInertia);

		float start_x = START_POS_X - ARRAY_SIZE_X/2;
		float start_y = START_POS_Y;
		float start_z = START_POS_Z - ARRAY_SIZE_Z/2;

		for (int k=0;k<ARRAY_SIZE_Y;k++)
		{
			for (int i=0;i<ARRAY_SIZE_X;i++)
			{
				for(int j = 0;j<ARRAY_SIZE_Z;j++)
				{
					startTransform.setOrigin(SCALING*btVector3(
										btScalar(2.0*i + start_x),
										btScalar(20+2.0*k + start_y),
										btScalar(2.0*j + start_z)));


					//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
					btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
					btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,colShape,localInertia);
					btRigidBody* body = new btRigidBody(rbInfo);

					body->setActivationState(ISLAND_SLEEPING);

					dynamicsWorld->addRigidBody(body);
					body->setActivationState(ISLAND_SLEEPING);
				}
			}
		}
	}

	dynamicsWorld->setDebugDrawer(&debugDrawer);
}


//=====================================================================================================================================
// init                                                                                                                               =
//=====================================================================================================================================
void Init()
{
	PRINT( "Engine initializing..." );

	initPhysics();

	srand( unsigned(time(NULL)) );
	mathSanityChecks();

	app::initWindow();
	uint ticks = app::getTicks();

	r::Init();
	ui::Init();

	// camera
	main_cam = new Camera( r::aspect_ratio*toRad(60.0), toRad(60.0), 0.5, 100.0 );
	main_cam->moveLocalY( 3.0 );
	main_cam->moveLocalZ( 5.7 );
	main_cam->moveLocalX( -0.3 );

	// lights
	point_lights[0] = new PointLight();
	point_lights[0]->init( "maps/temple/light0.light" );
	point_lights[0]->setLocalTransformation( Vec3( -1.0, 2.4, 1.0 ), Mat3::getIdentity(), 1.0 );
	point_lights[1] = new PointLight();
	point_lights[1]->init( "maps/temple/light1.light" );
	point_lights[1]->setLocalTransformation( Vec3( 2.5, 1.4, 1.0 ), Mat3::getIdentity(), 1.0 );

	spot_lights[0] = new SpotLight();
	spot_lights[0]->init( "maps/temple/light2.light" );
	spot_lights[0]->setLocalTransformation( Vec3( 1.3, 4.3, 3.0 ), Mat3( Euler(toRad(-20), toRad(20), 0.0) ), 1.0 );
	spot_lights[1] = new SpotLight();
	spot_lights[1]->init( "maps/temple/light3.light" );
	spot_lights[1]->setLocalTransformation( Vec3( -2.3, 6.3, 2.9 ), Mat3( Euler(toRad(-70), toRad(-20), 0.0) ), 1.0 );

	// horse
	horse = new MeshNode();
	horse->init( "meshes/horse/horse.mesh" );
	horse->setLocalTransformation( Vec3( -2, 0, 1 ), Mat3( Euler(-M::PI/2, 0.0, 0.0) ), 0.5 );
	
	// sarge
	sarge = new MeshNode();
	sarge->init( "meshes/sphere/sphere16.mesh" );
	//sarge->setLocalTransformation( Vec3( 0, -2.8, 1.0 ), Mat3( Euler(-M::PI/2, 0.0, 0.0) ), 1.1 );
	sarge->setLocalTransformation( Vec3( 0, 2.0, 2.0 ), Mat3::getIdentity(), 0.4 );
	
	// floor
	floor__ = new MeshNode();
	floor__->init( "maps/temple/Cube.019.mesh" );
	floor__->setLocalTransformation( Vec3(0.0, -0.19, 0.0), Mat3( Euler(-M::PI/2, 0.0, 0.0) ), 0.8 );

	// imp	
	imp = new skelModelNode();
	imp->init( "models/imp/imp.smdl" );
	imp->setLocalTransformation( Vec3( 0.0, 2.11, 0.0 ), Mat3( Euler(-M::PI/2, 0.0, 0.0) ), 0.7 );
	imp->meshNodes[0]->meshSkelCtrl->skelNode->skelAnimCtrl->skelAnim = rsrc::skel_anims.load( "models/imp/walk.imp.anim" );
	imp->meshNodes[0]->meshSkelCtrl->skelNode->skelAnimCtrl->step = 0.8;


	//
	floor_ = new floor_t;
	//floor_->material = rsrc::materials.load( "materials/default.mtl" );

	const char* skybox_fnames [] = { "textures/env/hellsky4_forward.tga", "textures/env/hellsky4_back.tga", "textures/env/hellsky4_left.tga",
																	 "textures/env/hellsky4_right.tga", "textures/env/hellsky4_up.tga", "textures/env/hellsky4_down.tga" };
	scene::skybox.load( skybox_fnames );

	PRINT( "Engine initialization ends (" << app::getTicks()-ticks << ")" );
	cerr.flush();
}


//=====================================================================================================================================
// main                                                                                                                               =
//=====================================================================================================================================
int main( int /*argc*/, char* /*argv*/[] )
{
	float f = M::sin( 10.0 );
	PRINT( f );

	app::printAppInfo();

	Init();

	PRINT( "Entering main loop" );
	int ticks = app::getTicks();
	do
	{
		int ticks_ = app::getTicks();
		i::HandleEvents();
		r::PrepareNextFrame();

		float dist = 0.2;
		float ang = toRad(3.0);
		float scale = 0.01;

		// move the camera
		static Node* mover = main_cam;

		if( i::keys[ SDLK_1 ] ) mover = main_cam;
		if( i::keys[ SDLK_2 ] ) mover = point_lights[0];
		if( i::keys[ SDLK_3 ] ) mover = spot_lights[0];
		if( i::keys[ SDLK_4 ] ) mover = point_lights[1];
		if( i::keys[ SDLK_5 ] ) mover = spot_lights[1];
		if( i::keys[ SDLK_m ] == 1 ) i::warp_mouse = !i::warp_mouse;

		if( i::keys[SDLK_a] ) mover->moveLocalX( -dist );
		if( i::keys[SDLK_d] ) mover->moveLocalX( dist );
		if( i::keys[SDLK_LSHIFT] ) mover->moveLocalY( dist );
		if( i::keys[SDLK_SPACE] ) mover->moveLocalY( -dist );
		if( i::keys[SDLK_w] ) mover->moveLocalZ( -dist );
		if( i::keys[SDLK_s] ) mover->moveLocalZ( dist );
		if( !i::warp_mouse )
		{
			if( i::keys[SDLK_UP] ) mover->rotateLocalX( ang );
			if( i::keys[SDLK_DOWN] ) mover->rotateLocalX( -ang );
			if( i::keys[SDLK_LEFT] ) mover->rotateLocalY( ang );
			if( i::keys[SDLK_RIGHT] ) mover->rotateLocalY( -ang );
		}
		else
		{
			float accel = 44.0;
			mover->rotateLocalX( ang * i::mouse_velocity.y * accel );
			mover->rotateLocalY( -ang * i::mouse_velocity.x * accel );
		}
		if( i::keys[SDLK_q] ) mover->rotateLocalZ( ang );
		if( i::keys[SDLK_e] ) mover->rotateLocalZ( -ang );
		if( i::keys[SDLK_PAGEUP] ) mover->scaleLspace += scale ;
		if( i::keys[SDLK_PAGEDOWN] ) mover->scaleLspace -= scale ;

		if( i::keys[SDLK_k] ) main_cam->lookAtPoint( point_lights[0]->translationWspace );

		mover->rotationLspace.reorthogonalize();


		scene::updateAllControllers();
		scene::updateAllWorldStuff();

		dynamicsWorld->stepSimulation( 1 );

		r::Render( *main_cam );

		//map.octree.root->bounding_box.render();

		// print some debug stuff
		ui::SetColor( Vec4(1.0, 1.0, 1.0, 1.0) );
		ui::SetPos( -0.98, 0.95 );
		ui::SetFontWidth( 0.03 );
		ui::printf( "frame:%d time:%dms\n", r::frames_num, app::getTicks()-ticks_ );
		//ui::print( "Movement keys: arrows,w,a,s,d,q,e,shift,space\nSelect objects: keys 1 to 5\n" );
		ui::printf( "Mover: Pos(%.2f %.2f %.2f) Angs(%.2f %.2f %.2f)", mover->translationWspace.x, mover->translationWspace.y, mover->translationWspace.z,
								 toDegrees(Euler(mover->rotationWspace).x), toDegrees(Euler(mover->rotationWspace).y), toDegrees(Euler(mover->rotationWspace).z) );

		if( i::keys[SDLK_ESCAPE] ) break;
		if( i::keys[SDLK_F11] ) app::togleFullScreen();
		if( i::keys[SDLK_F12] == 1 ) r::TakeScreenshot("gfx/screenshot.jpg");

		/*char str[128];
		if( r::frames_num < 1000 )
			sprintf( str, "capt/%06d.jpg", r::frames_num );
		else
			sprintf( str, "capt2/%06d.jpg", r::frames_num );
		r::TakeScreenshot(str);*/

		// std stuff follow
		SDL_GL_SwapBuffers();
		r::printLastError();
		if( 1 )
		{
			//if( r::frames_num == 10 ) r::TakeScreenshot("gfx/screenshot.tga");
			app::waitForNextFrame();
		}
		else
			if( r::frames_num == 5000 ) break;
	}while( true );
	PRINT( "Exiting main loop (" << app::getTicks()-ticks << ")" );


	PRINT( "Exiting..." );
	app::quitApp( EXIT_SUCCESS );
	return 0;
}
