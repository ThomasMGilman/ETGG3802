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

	//add lights to the scene
	GameObject* lightNode = GAME_OBJ_MANAGER->create_game_object("temporary", "PointLight0", nullptr, 0, Ogre::Vector3(20, 80, 50));
	lightNode->create_light("MainLight", OgreEngine::LightType::POINT);

	//attach spotlight to new node and set direction and position
	GameObject* spotLightNode = GAME_OBJ_MANAGER->create_game_object("temporary", "SpotLights", nullptr, 0, Ogre::Vector3(200, 200, 0));
	
	//create spotlight in scene
	LightComponent* spotLight = spotLightNode->create_light("SpotLight", OgreEngine::LightType::SPOT);
	spotLight->set_diffuse_color(.1, .1, .1);
	spotLight->set_specular_color(.1, .1, .1);
	spotLight->set_spotlight_params(35, 50);
	spotLight->set_direction(-1, -1, 0);

	//add Directional Light
	GameObject* dirLightNode = GAME_OBJ_MANAGER->create_game_object("temporary", "DirectionLights");
	LightComponent* dirLight = dirLightNode->create_light("DirectionLight", OgreEngine::LightType::DIRECTIONAL);//mScnMgr->createLight("DirectionalLight");
	dirLight->set_diffuse_color(.2, .2, .2);
	dirLight->set_specular_color(.2, .2, .2);
	dirLight->set_direction(Ogre::Vector3(0, -1, 1));

	//add Point Light
	GameObject* pointLightNode = GAME_OBJ_MANAGER->create_game_object("temporary", "PointLight1", nullptr, 0, Ogre::Vector3(0, 150, 250));
	LightComponent* pointLight = pointLightNode->create_light("PointLight", OgreEngine::LightType::POINT);
	pointLight->set_diffuse_color(.3, .3, .3);
	pointLight->set_specular_color(.3, .3, .3);
	
	///////////////////////////////////////////////////////////////////////////////////////////// Add Camera and ViewPort
	// Create Viewport
	mVp = getRenderWindow()->addViewport(nullptr);
	mVp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
	
	// Create the camera
	// Create CameraNode and attach camera to be put into the scene
	GameObject* camNode = GAME_OBJ_MANAGER->create_game_object("temporary", "CameraNode", nullptr, 0, Ogre::Vector3(0, 15, 30));
	camNode->look_at(Ogre::Vector3::ZERO);

	CameraComponent* cam = camNode->create_camera("MainCamera");
	cam->set_clip_distances(5, 100);
	cam->set_auto_aspect_ratio(true);
	cam->set_main_camera(mVp);

	///////////////////////////////////////////////////////////////////////////////////////////// Add Entitys and plane
	//create Ogre Entity to render and set to cast shadows
	
	GameObject* ogreNode = GAME_OBJ_MANAGER->create_game_object("temporary", "OgreEnt", nullptr, 0, Ogre::Vector3(0, 5, 0));
	ogreNode->look_at();
	ogreNode->rotate_world(90, 1, 0, 0);

	MeshComponent* ogreEnt = ogreNode->create_mesh("OgreMesh", "Sinbad.mesh");
	ogreEnt->set_cast_shadows(true);

	//create Peguin Entity to render that is a child of Ogre
	GameObject* penguinNode = GAME_OBJ_MANAGER->create_game_object("temporary", "PenguinEnt", ogreNode, 0, Ogre::Vector3(1.75, 3.5, 0));
	penguinNode->look_at();
	penguinNode->set_scale(Ogre::Vector3(.05));
	penguinNode->rotate_world(90, 1, 0, 0);
	penguinNode->rotate_world(-25, 0, 0, 1);

	MeshComponent* penguinEnt = penguinNode->create_mesh("PenguinMesh", "penguin.mesh");
	penguinEnt->set_cast_shadows(true);
	penguinEnt->play_animation("amuse", true, true);

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

	GameObject* groundNode = GAME_OBJ_MANAGER->create_game_object("temporary", "GroundNode");
	MeshComponent* groundEnt = groundNode->create_mesh("groundMesh", "ground");
	groundEnt->set_cast_shadows(true);
	groundEnt->set_material("Examples/Rockwall");

	///////////////////////////////////////////////////////////////////////////////////////////// Enable and Add SkyBox
	mScnMgr->setSkyBox(true, "Examples/SpaceSkyBox", 300, false);
	
	mLogger = new LogManager(mVp->getActualHeight());

	//GAME_OBJ_MANAGER->load_scene("Cube.mesh");
}

bool Application::frameStarted(const Ogre::FrameEvent& e)
{
	OgreBites::ApplicationContext::frameStarted(e);

	// Update current Animation being played by penguin
	GAME_OBJ_MANAGER->update(e.timeSinceLastFrame);
	//mAnimationState->addTime(e.timeSinceLastFrame);
	
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