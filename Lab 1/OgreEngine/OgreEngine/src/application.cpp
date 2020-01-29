#include "application.h"

using namespace OgreEngine;

Application* Application::Singleton::msSingleton = nullptr;

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

	//add light to the scene
	Ogre::Light* light = mScnMgr->createLight("MainLight");
	Ogre::SceneNode* lightNode = mScnMgr->getRootSceneNode()->createChildSceneNode();
	lightNode->setPosition(20, 80, 50);
	lightNode->attachObject(light);

	//add Spotlight to the scene
	Ogre::Light* spotLight = mScnMgr->createLight("SpotLight");
	spotLight->setDiffuseColour(.1, .1, .1);
	spotLight->setSpecularColour(.1, .1, .1);
	spotLight->setType(Ogre::Light::LT_SPOTLIGHT);
	spotLight->setSpotlightRange(Ogre::Degree(35), Ogre::Degree(50));

	//attach spotlight to new node and set direction and position
	Ogre::SceneNode* spotLightNode = mScnMgr->getRootSceneNode()->createChildSceneNode();
	spotLightNode->attachObject(spotLight);
	spotLightNode->setDirection(-1, -1, 0);
	spotLightNode->setPosition(Ogre::Vector3(200, 200, 0));

	//add Directional Light
	Ogre::Light* dirLight = mScnMgr->createLight("DirectionalLight");
	dirLight->setType(Ogre::Light::LT_DIRECTIONAL);
	dirLight->setDiffuseColour(Ogre::ColourValue(.2, .2, .2));
	dirLight->setSpecularColour(Ogre::ColourValue(.2, .2, .2));

	//set Directional Light in scene
	Ogre::SceneNode* dirLightNode = mScnMgr->getRootSceneNode()->createChildSceneNode();
	dirLightNode->attachObject(dirLight);
	dirLightNode->setDirection(Ogre::Vector3(0, -1, 1));

	//add Point Light
	Ogre::Light* pointLight = mScnMgr->createLight("PointLight");
	pointLight->setType(Ogre::Light::LT_POINT);
	pointLight->setDiffuseColour(.3, .3, .3);
	pointLight->setSpecularColour(.3, .3, .3);

	//set Point light in scene
	Ogre::SceneNode* pointLightNode = mScnMgr->getRootSceneNode()->createChildSceneNode();
	pointLightNode->attachObject(pointLight);
	pointLightNode->setPosition(Ogre::Vector3(0, 150, 250));

	///////////////////////////////////////////////////////////////////////////////////////////// Add Camera and ViewPort
	//create the camera
	Ogre::Camera* cam = mScnMgr->createCamera("myCam");
	cam->setNearClipDistance(5);	//can be changed, specific for this sample
	cam->setAutoAspectRatio(true);

	//Create CameraNode and attach camera to be put into the scene
	Ogre::SceneNode* camNode = mScnMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(0, 15, 30));
	camNode->lookAt(Ogre::Vector3::ZERO, Ogre::Node::TS_WORLD);
	camNode->attachObject(cam);

	//set camera to render to main window by creating a viewport and attaching the camera
	Ogre::Viewport* vp = getRenderWindow()->addViewport(cam);
	vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));

	//set camera aspect Ratio
	cam->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

	///////////////////////////////////////////////////////////////////////////////////////////// Add Entitys and plane
	//create Ogre Entity to render and set to cast shadows
	Ogre::Entity* ogreEnt = mScnMgr->createEntity("Sinbad.mesh");
	ogreEnt->setCastShadows(true);
	Ogre::SceneNode* ogreNode = mScnMgr->getRootSceneNode()->createChildSceneNode("OgreEnt", Ogre::Vector3(0, 5, 0), Ogre::Quaternion::IDENTITY);
	ogreNode->lookAt(Ogre::Vector3(0, 0, 0), Ogre::Node::TransformSpace::TS_WORLD);
	ogreNode->pitch(Ogre::Degree(90));
	ogreNode->attachObject(ogreEnt);

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
	mScnMgr->getRootSceneNode()->createChildSceneNode()->attachObject(groundEntity);
	groundEntity->setCastShadows(false);
	groundEntity->setMaterialName("Examples/Rockwall");


	///////////////////////////////////////////////////////////////////////////////////////////// Enable and Add SkyBox
	mScnMgr->setSkyBox(true, "Examples/SpaceSkyBox", 300, false);

	mLogger = new LogManager(vp->getActualHeight());
}

bool Application::frameStarted(const Ogre::FrameEvent& e)
{
	OgreBites::ApplicationContext::frameStarted(e);

	if(mKeysDown.find(OgreBites::SDLK_LEFT) != mKeysDown.end())
		mScnMgr->getSceneNode("OgreEnt")->yaw(Ogre::Degree(-90) * e.timeSinceLastFrame);
	
	if (mKeysDown.find(OgreBites::SDLK_RIGHT) != mKeysDown.end())
		mScnMgr->getSceneNode("OgreEnt")->yaw(Ogre::Degree(90) * e.timeSinceLastFrame);

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