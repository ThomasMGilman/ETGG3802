#include <application.h>
#include <game_object_manager.h>

using namespace OgreEngine;

Application* Application::msSingleton = nullptr;

Application::Application() : OgreBites::ApplicationContext("OgreTutorialApp")
{

}

void Application::setup(void)
{
	///////////////////////////////////////////////////////////////////////////////////////////// Initial Setup
	//base setup
	OgreBites::ApplicationContext::setup();

	//register for input events
	addInputListener(this);

	//get pointer to created root and create sceneManager
	mRoot = getRoot();
	mScnMgr = mRoot->createSceneManager();

	mScnMgr->addRenderQueueListener(getOverlaySystem());

	//register scene with the RTSS(Shader)
	Ogre::RTShader::ShaderGenerator* shadergen = 
		Ogre::RTShader::ShaderGenerator::getSingletonPtr();
	shadergen->addSceneManager(mScnMgr);

	///////////////////////////////////////////////////////////////////////////////////////////// Add Lights
	//Set Ambient light color
	mScnMgr->setAmbientLight(Ogre::ColourValue(0, 0, 0));
	mScnMgr->setShadowTechnique(Ogre::ShadowTechnique::SHADOWTYPE_STENCIL_ADDITIVE);
	getRenderWindow()->addViewport(nullptr)->setBackgroundColour(Ogre::ColourValue(0,0,0));
	mLogger = new LogManager(getRenderWindow()->getViewport(0)->getActualHeight());

	//Initialize GameObjectManager
	mGOM = new GameObjectManager();
	//mGOM->set_default_scene();

	mGOM->load_scene("main_scene.scene", "../Media/invader_media/");
}

bool Application::frameStarted(const Ogre::FrameEvent& e)
{
	OgreBites::ApplicationContext::frameStarted(e);

	// Update current Animation being played by penguin
	GAME_OBJ_MANAGER->update(e.timeSinceLastFrame);
	//mAnimationState->addTime(e.timeSinceLastFrame);
	/*
	GameObject* player = GAME_OBJ_MANAGER->get_game_object("OgreEnt", "temporary");
	if (player != nullptr)
	{
		////////////////////////////////////////////////////////////// Rotation Keys for Ogre
		if (mKeysDown.find(OgreBites::SDLK_LEFT) != mKeysDown.end())
			player->rotate_world(-90 * e.timeSinceLastFrame, 0, 1, 0);

		if (mKeysDown.find(OgreBites::SDLK_RIGHT) != mKeysDown.end())
			player->rotate_world(90 * e.timeSinceLastFrame, 0, 1, 0);
	}*/
	
	////////////////////////////////////////////////////////////// visibility Keys for all of temporary group
	if (mKeysDown.find(OgreBites::SDLK_F1) != mKeysDown.end())
		GAME_OBJ_MANAGER->set_visibility("temporary", false);

	if (mKeysDown.find(OgreBites::SDLK_F2) != mKeysDown.end())
		GAME_OBJ_MANAGER->set_visibility("temporary", true);

	mLogger->update(e.timeSinceLastFrame);
	
	return true;
}

/**
Handle keypress events from the user
*/
bool Application::keyPressed(const OgreBites::KeyboardEvent& evt)
{
	if (evt.keysym.sym == OgreBites::SDLK_ESCAPE)
	{
		getRoot()->queueEndRendering();
	}
	else
		//Add key pressed to set of keysCurrently down
		mKeysDown.emplace(evt.keysym.sym);

	// Build message to be printed and clear stringstream after for next message
	mStringStream << "Key Pressed = " << evt.keysym.sym;
	mLogger->log_message(mStringStream.str(), Ogre::ColourValue(RAND_COLOUR_VAL, RAND_COLOUR_VAL, RAND_COLOUR_VAL), 2);
	mStringStream.str(std::string());

	return true;
}

bool Application::keyReleased(const OgreBites::KeyboardEvent& evt)
{
	//Remove key that was released from set
	mKeysDown.erase(mKeysDown.find(evt.keysym.sym));

	return true;
}

Application::~Application()
{
	delete(mLogger);
}