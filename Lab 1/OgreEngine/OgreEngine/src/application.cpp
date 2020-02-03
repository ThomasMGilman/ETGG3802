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

	//Initialize GameObjectManager
	mGOM = new GameObjectManager();

	//add light to the scene
	Ogre::Light* light = mScnMgr->createLight("MainLight");
	GameObject* lightNode = GAME_OBJ_MANAGER->create_game_object("temporary", light->getName(), nullptr, 0, Ogre::Vector3(20, 80, 50));
	lightNode->attach_object(light);

	//add Spotlight to the scene
	Ogre::Light* spotLight = mScnMgr->createLight("SpotLight");
	spotLight->setDiffuseColour(.1, .1, .1);
	spotLight->setSpecularColour(.1, .1, .1);
	spotLight->setType(Ogre::Light::LT_SPOTLIGHT);
	spotLight->setSpotlightRange(Ogre::Degree(35), Ogre::Degree(50));

	//attach spotlight to new node and set direction and position
	GameObject* spotLightNode = GAME_OBJ_MANAGER->create_game_object("temporary", spotLight->getName(), nullptr, 0, Ogre::Vector3(200, 200, 0));
	spotLightNode->attach_object(spotLight);
	spotLightNode->set_direction(-1, -1, 0);

	//add Directional Light
	Ogre::Light* dirLight = mScnMgr->createLight("DirectionalLight");
	dirLight->setType(Ogre::Light::LT_DIRECTIONAL);
	dirLight->setDiffuseColour(Ogre::ColourValue(.2, .2, .2));
	dirLight->setSpecularColour(Ogre::ColourValue(.2, .2, .2));

	//set Directional Light in scene
	GameObject* dirLightNode = GAME_OBJ_MANAGER->create_game_object("temporary", dirLight->getName());
	dirLightNode->attach_object(dirLight);
	dirLightNode->set_direction(Ogre::Vector3(0, -1, 1));

	//add Point Light
	Ogre::Light* pointLight = mScnMgr->createLight("PointLight");
	pointLight->setType(Ogre::Light::LT_POINT);
	pointLight->setDiffuseColour(.3, .3, .3);
	pointLight->setSpecularColour(.3, .3, .3);

	//set Point light in scene
	GameObject* pointLightNode = GAME_OBJ_MANAGER->create_game_object("temporary", pointLight->getName());
	pointLightNode->attach_object(pointLight);
	pointLightNode->set_position(Ogre::Vector3(0, 150, 250));
	
	///////////////////////////////////////////////////////////////////////////////////////////// Add Camera and ViewPort
	//create the camera
	Ogre::Camera* cam = mScnMgr->createCamera("myCam");
	cam->setNearClipDistance(5);	//can be changed, specific for this sample
	cam->setAutoAspectRatio(true);

	//Create CameraNode and attach camera to be put into the scene
	GameObject* camNode = GAME_OBJ_MANAGER->create_game_object("temporary", cam->getName(), nullptr, 0, Ogre::Vector3(0, 15, 30));
	camNode->look_at(Ogre::Vector3::ZERO);
	camNode->attach_object(cam);

	//set camera to render to main window by creating a viewport and attaching the camera
	Ogre::Viewport* vp = getRenderWindow()->addViewport(cam);
	vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));

	//set camera aspect Ratio
	cam->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
	
	///////////////////////////////////////////////////////////////////////////////////////////// Add Entitys and plane
	//create Ogre Entity to render and set to cast shadows
	Ogre::Entity* ogreEnt = mScnMgr->createEntity("OgreEnt", "Sinbad.mesh");
	ogreEnt->setCastShadows(true);
	GameObject* ogreNode = GAME_OBJ_MANAGER->create_game_object("temporary", ogreEnt->getName(), nullptr, 0, Ogre::Vector3(0, 5, 0));
	ogreNode->look_at();
	ogreNode->rotate_world(90, 1, 0, 0);
	ogreNode->attach_object(ogreEnt);

	//create Peguin Entity to render that is a child of Ogre
	Ogre::Entity* penguinEnt = mScnMgr->createEntity("PenguinEnt", "penguin.mesh");
	penguinEnt->setCastShadows(true);
	GameObject* penguinNode = GAME_OBJ_MANAGER->create_game_object("temporary", penguinEnt->getName(), ogreNode, 0, Ogre::Vector3(1.75, 3.5, 0));
	penguinNode->look_at();
	penguinNode->set_scale(Ogre::Vector3(.05));
	penguinNode->rotate_world(90, 1, 0, 0);
	penguinNode->rotate_world(-25, 0, 0, 1);
	penguinNode->attach_object(penguinEnt);

	//Enable Penguins amuse animation and play it
	mAnimationState = penguinEnt->getAnimationState("amuse");
	mAnimationState->setLoop(true);
	mAnimationState->setEnabled(true);

	//Create Ground Plane
	Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
	Ogre::MeshManager::getSingleton().createPlane(
		"ground",				//Name
		Ogre::RGN_DEFAULT,		//GroupName
		plane,					//Plane object
		100,					//Width
		100,					//Height
		20,						//xSegments
		20,						//ySegments
		true,					//normals?
		1,						//numTexCoordSets
		5,						//uTile floats
		5,						//vTile floats
		Ogre::Vector3::UNIT_Z);	//upVector

	Ogre::Entity* groundEntity = mScnMgr->createEntity("ground");
	groundEntity->setCastShadows(false);
	groundEntity->setMaterialName("Examples/Rockwall");

	GameObject* groundPlane = GAME_OBJ_MANAGER->create_game_object("temporary", groundEntity->getName());
	groundPlane->attach_object(groundEntity);

	///////////////////////////////////////////////////////////////////////////////////////////// Enable and Add SkyBox
	mScnMgr->setSkyBox(true, "Examples/SpaceSkyBox", 300, false);
	
	mLogger = new LogManager(vp->getActualHeight());
}

bool Application::frameStarted(const Ogre::FrameEvent& e)
{
	OgreBites::ApplicationContext::frameStarted(e);

	// Update current Animation being played by penguin
	mAnimationState->addTime(e.timeSinceLastFrame);
	
	////////////////////////////////////////////////////////////// Rotation Keys for Ogre
	if (mKeysDown.find(OgreBites::SDLK_LEFT) != mKeysDown.end())
		GAME_OBJ_MANAGER->get_game_object("OgreEnt", "temporary")->rotate_world(-90 * e.timeSinceLastFrame, 0, 1, 0);
	
	if (mKeysDown.find(OgreBites::SDLK_RIGHT) != mKeysDown.end())
		GAME_OBJ_MANAGER->get_game_object("OgreEnt", "temporary")->rotate_world(90 * e.timeSinceLastFrame, 0, 1, 0);

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