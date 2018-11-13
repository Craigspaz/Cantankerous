#include "Game.h"
#include "Messages.h"

using namespace Ogre;
using namespace OgreBites;

Game::Game() : ApplicationContext("Cantankerous") // Initializes the Ogre context
{
	deltaTime = 0;
	if (Messages::initMessages() != 0)
	{
		printf("Unexpected error initializing socket framework...\n");
		exit(-1);
	}
	gameMode = -1;
	camera = NULL;
	client = NULL;
	server = NULL;
}
Game::~Game()
{
	sceneManager->getRootSceneNode()->removeAndDestroyAllChildren();
	delete currentLevel;
	//TODO: Clean up memory allocated
}
	
void Game::setup()
{
	// do not forget to call the base first
	ApplicationContext::setup();
	addInputListener(this);

	// get a pointer to the already created root
	Root* root = getRoot();

	sceneManager = root->createSceneManager();

	//this is necessary to view OGRE tray GUI...
	sceneManager->addRenderQueueListener(mOverlaySystem);

	// register our scene with the RTSS
	RTShader::ShaderGenerator* shadergen = RTShader::ShaderGenerator::getSingletonPtr();
	shadergen->addSceneManager(sceneManager);

	//create ambient light
	sceneManager->setAmbientLight(ColourValue(0.5, 0.5, 0.5));

	//create point light
	Light* light = sceneManager->createLight("MainLight");
	light->setType(Light::LT_POINT);
	SceneNode* lightNode = sceneManager->getRootSceneNode()->createChildSceneNode();
	lightNode->attachObject(light);
	lightNode->setPosition(20, 180, 50);

	this->cameraNode = sceneManager->getRootSceneNode()->createChildSceneNode();

	// create the camera
	this->camera = sceneManager->createCamera("camera");
	camera->setNearClipDistance(5);
	camera->setAutoAspectRatio(true);
	camera->lookAt(0, 0, 0);
	this->cameraNode->attachObject(camera);
	this->cameraNode->setPosition(0, 200, 100);
	camera->rotate(Ogre::Vector3::UNIT_X, Ogre::Radian(Ogre::Degree(-50)));

	// and tell it to render into the main window
	getRenderWindow()->addViewport(camera);

	//create camera man
	this->cameraMan = new OgreBites::CameraMan(this->cameraNode);

	// create tray manager and show stats and logo and (hide the cursor)
	trayManager = new OgreBites::TrayManager("Controls", getRenderWindow(), this);  // create a tray interface
	trayManager->showFrameStats(TL_BOTTOMLEFT);
	trayManager->showLogo(TL_BOTTOMRIGHT);
	//m_Tray_Mgr->hideCursor();
	trayManager->setListener(this);

	Button* b = trayManager->createButton(TL_CENTER, "Host", "Host Game");
	Button* b1 = trayManager->createButton(TL_CENTER, "Join", "Join Game");
	Button* b2 = trayManager->createButton(TL_CENTER, "Exit", "Exit Game");

	controls = new AdvancedRenderControls(trayManager, camera);

	sceneManager->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
	sceneManager->setShadowColour(ColourValue(0.5, 0.5, 0.5));
	sceneManager->setShadowTextureSize(1024);
	sceneManager->setShadowTextureCount(1);

	//sceneManager->setSkyDome(true, "Examples/CloudySky", 5, 8);

	cameraMan->setStyle(OgreBites::CS_MANUAL);

	// use small amount of ambient lighting
	sceneManager->setAmbientLight(ColourValue(0.3f, 0.3f, 0.3f));

	// add a bright light above the scene
	Light* light1 = sceneManager->createLight();
	light1->setType(Light::LT_POINT);
	light1->setPosition(-10, 40, 20);
	light1->setSpecularColour(ColourValue::White);

	// Create a test plane to verify everything is working
	/// TMP
	//Plane floorPlane(Vector3::UNIT_Y, 0);
	//MeshManager::getSingleton().createPlane("floor", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, floorPlane, 1000.0, 1000.0, 10, 10, true, 1, 20.0, 20.0, Vector3::UNIT_Z);
	//Entity* floor = sceneManager->createEntity("floor");
	//floor->setMaterialName("Examples/Rockwall");
	//SceneNode* node = sceneManager->getRootSceneNode()->createChildSceneNode();
	//node->attachObject(floor);
	//node->setPosition(Vector3(0, 0, 0));
	/// END TMP

	/*Entity* tmpEntity = sceneManager->createEntity("treds_Cube.mesh");
	SceneNode* tmpSceneNode = sceneManager->getRootSceneNode()->createChildSceneNode();
	tmpSceneNode->setPosition(Ogre::Vector3(0, 10, 0));
	tmpSceneNode->scale(Ogre::Vector3(3,3,3));
	//tmpSceneNode->rotate(Ogre::Vector3::UNIT_X, Ogre::Radian(Ogre::Degree(-90)));
	//tmpSceneNode->scale(Ogre::Vector3(1 / 10., 1 / 10., 1 / 10.));
	tmpSceneNode->attachObject(tmpEntity);

	tmpEntity = sceneManager->createEntity("turret_Cube.001.mesh");
	tmpSceneNode = sceneManager->getRootSceneNode()->createChildSceneNode();
	tmpSceneNode->setPosition(Ogre::Vector3(0, 10, 0));
	tmpSceneNode->scale(Ogre::Vector3(3, 3, 3));
	tmpSceneNode->rotate(Ogre::Vector3::UNIT_Y, Ogre::Radian(Ogre::Degree(100)));
	tmpSceneNode->attachObject(tmpEntity);
	*/

	std::string path = __FILE__; //gets the current cpp file's path with the cpp file
	path = path.substr(0, 1 + path.find_last_of('\\')); //removes filename to leave path
	currentLevel = new Level(path, "testLevel.txt",sceneManager);
}

void Game::joinGame()
{
	OgreBites::TextBox* box = (OgreBites::TextBox*)trayManager->getWidget("IP Address");
	std::cout << box->getText() << std::endl;
	client = new Client(box->getText(), 1234, this, sceneManager);
	trayManager->destroyAllWidgets();
	gameMode = 1;
}

std::string Game::getCurrentLevelFileName()
{
	return currentLevel->getFileName();
}

void Game::buttonHit(Button* button)
{
	// Checks the name of the button to determine what action to take
	if (button->getName() == "Host")
	{
		trayManager->destroyAllWidgets();
		std::string path = __FILE__; //gets the current cpp file's path with the cpp file
		path = path.substr(0, 1 + path.find_last_of('\\')); //removes filename to leave path
		this->setLevel(new Level(path,"testLevel2.txt", sceneManager));
		//std::thread createServerThread(&Game::createServer, this);
		//createServerThread.detach();
		server = new Server(this, sceneManager);
		clientIP = "127.0.0.1";
		client = new Client(clientIP, 1234, this, sceneManager);
		//std::thread createClientThread(&Game::createClient, this);
		//createClientThread.detach();
		gameMode = 2;
		std::cout << "Initialized server and host client" << std::endl;
	}
	else if (button->getName() == "Join")
	{
		trayManager->destroyAllWidgets();
		trayManager->createTextBox(TL_CENTER, "IP Address", "IP Address", 400, 100);
		trayManager->createButton(TL_CENTER, "Submit", "Submit");
		gameMode = 0;
	}
	else if (button->getName() == "Submit")
	{
		joinGame();
	}
	else if (button->getName() == "Resume")
	{
		trayManager->destroyAllWidgets();
	}
	else if (button->getName() == "Exit")
	{
		exit(0);
	}
}


void Game::createServer()
{
	server = new Server(this, sceneManager);
}

void Game::createClient()
{
	client = new Client(clientIP, 1234, this, sceneManager);
}


void Game::update()
{
	if (gameMode == 1) // Client
	{

	}
	else if (gameMode == 2) // Server
	{
		//std::cout << "Updating..." << std::endl;
		server->update();
	}
	else if (gameMode == 0) // getting IP
	{

	}
}


void Game::setLevel(Level* level)
{
	delete this->currentLevel;
	this->currentLevel = level;
}
	
bool Game::keyPressed(const KeyboardEvent& event)
{
	if(event.keysym.sym == SDLK_ESCAPE)
	{
		//getRoot()->queueEndRendering();
		//exit(0);
		if (gameMode == -1 || gameMode == 0)
		{
			exit(0);
		}
		else
		{
			trayManager->destroyAllWidgets();
			trayManager->createButton(TL_CENTER, "Resume", "Resume");
			trayManager->createButton(TL_CENTER, "Exit", "Exit");
		}
	}

	// if the user clicked join game handle all key presses as input to go to the textbox
	if (gameMode == 0)
	{
		OgreBites::TextBox* box = (OgreBites::TextBox*)trayManager->getWidget("IP Address");
		char characterToAdd = event.keysym.sym;
		if (characterToAdd == '\b')
		{
			std::string tmp = box->getText();
			tmp = tmp.substr(0, tmp.size() - 1);
			box->setText(tmp);
		}
		else if (characterToAdd == '\n' || characterToAdd == '\r\n' || characterToAdd == '\n\r' || characterToAdd == 13)
		{
			joinGame();
		}
		else
		{
			std::string str(1, characterToAdd);
			box->appendText(str);
		}
	}
	else if(gameMode != -1)
	{
		if (event.keysym.sym == SDLK_RIGHT)
		{
			if (cameraNode->getPosition().x + 100 > currentLevel->getMaxBoundary().x)
			{
				return true;
			}
			cameraNode->setPosition(cameraNode->getPosition() + Ogre::Vector3(10, 0, 0));
			return true;
		}
		else if (event.keysym.sym == SDLK_LEFT)
		{
			if (cameraNode->getPosition().x - 100 < currentLevel->getMinBoundary().x)
			{
				return true;
			}
			cameraNode->setPosition(cameraNode->getPosition() + Ogre::Vector3(-10, 0, 0));
			return true;
		}
		else if (event.keysym.sym == SDLK_UP)
		{
			if (cameraNode->getPosition().z - 10 - 150 < currentLevel->getMinBoundary().z)
			{
				return true;
			}
			cameraNode->setPosition(cameraNode->getPosition() + Ogre::Vector3(0, 0, -10));
			return true;
		}
		else if (event.keysym.sym == SDLK_DOWN)
		{
			if (cameraNode->getPosition().z + 10 - 100 > currentLevel->getMaxBoundary().z)
			{
				return true;
			}
			cameraNode->setPosition(cameraNode->getPosition() + Ogre::Vector3(0, 0, 10));
			return true;
		}
	}

	//cameraMan->keyPressed(event);
	return true;
}
bool Game::keyReleased(const KeyboardEvent& event)
{
	cameraMan->keyPressed(event);
	return true;
}
	
bool Game::frameRenderingQueued(const Ogre::FrameEvent& event)
{
	trayManager->frameRendered(event);
	//mControls->frameRendered(evt);
	deltaTime += event.timeSinceLastFrame;
	while (deltaTime >= 1.0f / 60.0f)
	{
		//update the game
		update();
		deltaTime = 0;
	}
	//std::cout << "event..." << std::endl;

	if (!trayManager->isDialogVisible())
	{
		cameraMan->frameRendered(event);   // if dialog isn't up, then update the camera
	}

	return true;
}
bool Game::mouseMoved(const MouseMotionEvent& event)
{
	if (trayManager->mouseMoved(event)) return true;

	cameraMan->mouseMoved(event);
	return true;
}
bool Game::mousePressed(const MouseButtonEvent& event)
{
	if (trayManager->mousePressed(event)) return true;

	//if (mDragLook && evt.button == BUTTON_LEFT)
	//{
	//	mCameraMan->setStyle(CS_FREELOOK);
	//	mTrayMgr->hideCursor();
	//}

	//cameraMan->mousePressed(event);
	trayManager->mousePressed(event);
	return true;
}
bool Game::mouseReleased(const MouseButtonEvent& event)
{
	if (trayManager->mouseReleased(event)) return true;

	//if (mDragLook && evt.button == BUTTON_LEFT)
	//{
	//	m_Camera_Man->setStyle(CS_MANUAL);
	//	mTrayMgr->showCursor();
	//}

	//cameraMan->mouseReleased(event);
	trayManager->mouseReleased(event);
	return true;
}
	
bool Game::touchReleased(const TouchFingerEvent& event)
{
	MouseButtonEvent e;
	e.button = BUTTON_LEFT;
	return mouseReleased(e);
}
bool Game::mouseWheelRolled(const MouseWheelEvent& event)
{
	//cameraMan->mouseWheelRolled(event);
	return true;
}