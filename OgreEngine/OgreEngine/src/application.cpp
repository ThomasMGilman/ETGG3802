#include <stdafx.h>
#include <application.h>
#include <game_object_manager.h>
#include <script_manager.h>
#include <input_manager.h>

using namespace OgreEngine;

Application* Application::msSingleton = nullptr;

Application::Application() : OgreBites::ApplicationContext("OgreTutorialApp")
{

}

void Application::setup(void)
{
	//base parent Ogre initialization setup
	OgreBites::ApplicationContext::setup();

	//get pointer to created root and create sceneManager
	mRoot = getRoot();
	mScnMgr = mRoot->createSceneManager();
	mScnMgr->addRenderQueueListener(getOverlaySystem());

	//register scene with the RTSS(Shader)
	Ogre::RTShader::ShaderGenerator* shadergen = 
		Ogre::RTShader::ShaderGenerator::getSingletonPtr();
	shadergen->addSceneManager(mScnMgr);

	//Set Ambient light color, shadowing type, and background color
	mScnMgr->setAmbientLight(Ogre::ColourValue(0, 0, 0));
	mScnMgr->setShadowTechnique(Ogre::ShadowTechnique::SHADOWTYPE_STENCIL_ADDITIVE);
	getRenderWindow()->addViewport(nullptr)->setBackgroundColour(Ogre::ColourValue(0,0,0));

	//Initialize application managers
	mLM = new LogManager(getRenderWindow()->getViewport(0)->getActualHeight(), getRenderWindow()->getViewport(0)->getActualWidth());
	mGOM = new GameObjectManager();
	mSM = new ScriptManager();
	mIM = new InputManager("../Media/core_media/input_bindings.xml");
	mCM = new CollisionManager();

	// Start Game instance
	mSM->run_script("../Media/invader_media/init.py");
}

bool Application::frameStarted(const Ogre::FrameEvent& e)
{
	//Update InputManager, CollisionManager, GameObjectManage, and LogManager
	mGOM->updating = true;
	mIM->update(e.timeSinceLastFrame);		// Input Manager
	mGOM->update(e.timeSinceLastFrame);		// Game Object Manager
	mCM->update(e.timeSinceLastFrame);		// Collision Manager
	mLM->update(e.timeSinceLastFrame);		// Log Manager
	mGOM->updating = false;
	mGOM->add_queued_objects();
	mGOM->destroy_queued_objects();

	return frameEnded(e);
}

void OgreEngine::Application::quite()
{
	this->mRoot->queueEndRendering();
}

Application::~Application()
{
	
}