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
	///////////////////////////////////////////////////////////////////////////////////////////// Initial Setup
	//base setup
	OgreBites::ApplicationContext::setup();

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

	//Initialize application managers
	mLogger = new LogManager(getRenderWindow()->getViewport(0)->getActualHeight());
	mGOM = new GameObjectManager();
	mSM = new ScriptManager();
	mIM = new InputManager("../Media/my_media/input_bindings.xml");

	// Start Game instance
	mSM->run_script("../Media/invader_media/init.py");
}

bool Application::frameStarted(const Ogre::FrameEvent& e)
{
	OgreBites::ApplicationContext::frameStarted(e);
	mIM->update(e.timeSinceLastFrame);

	// Update current Animation being played by penguin
	mGOM->update(e.timeSinceLastFrame);

	////////////////////////////////////////////////////////////// visibility Keys for all of temporary group
	if (mKeysDown.find(OgreBites::SDLK_F1) != mKeysDown.end())
		mGOM->set_visibility("temporary", false);

	if (mKeysDown.find(OgreBites::SDLK_F2) != mKeysDown.end())
		mGOM->set_visibility("temporary", true);

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
	delete(mGOM);
	delete(mSM);
	delete(mLogger);
	delete(mIM);
}