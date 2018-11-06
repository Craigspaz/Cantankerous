#include "Game.h"
#include <iostream>

int main(int argc, char** argv)
{
	try
	{
		/*Ogre::Root* root = new Ogre::Root();
		//root->restoreConfig();
		//root->saveConfig();
		root->setRenderSystem(root->getAvailableRenderers()[0]);
		root->initialise(true);
		Ogre::RenderWindow* window = root->createRenderWindow("Cantankerous", 1366, 768, false);
		window->setVisible(true);

		Ogre::SceneManager* sceneManager = root->createSceneManager(Ogre::ST_GENERIC);

		// register our scene with the RTSS
		//RTShader::ShaderGenerator* shadergen = RTShader::ShaderGenerator::getSingletonPtr();
		//shadergen->addSceneManager(sceneManager);

		//create ambient light
		//sceneManager->setAmbientLight(ColourValue(0.5, 0.5, 0.5));


		Ogre::Camera* camera = sceneManager->createCamera("Camera");
		camera->setNearClipDistance(5); // specific to this sample
		camera->setAutoAspectRatio(true);
		camera->lookAt(0, 0, 0);

		window->addViewport(camera);

		Light* light = sceneManager->createLight("MainLight");
		light->setType(Light::LT_POINT);
		SceneNode* lightNode = sceneManager->getRootSceneNode()->createChildSceneNode();
		lightNode->attachObject(light);
		lightNode->setPosition(20, 180, 50);

		//Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media/packs/Sinbad.zip", "Zip");
		//Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../media/materials/scripts/", "FileSystem");
		Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
		//Ogre::ResourceGroupManager::getSingleton().loadResourceGroup("General");

		Plane floorPlane(Vector3::UNIT_Y, 0);
		MeshManager::getSingleton().createPlane("floor", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, floorPlane, 1000.0, 1000.0, 10, 10, true, 1, 20.0, 20.0, Vector3::UNIT_Z);
		Entity* floor = sceneManager->createEntity("floor");
		floor->setMaterialName("Examples/Rockwall");
		SceneNode* node = sceneManager->getRootSceneNode()->createChildSceneNode();
		node->attachObject(floor);
		node->setPosition(Vector3(0, 0, 0));

		while (true)
		{
			root->renderOneFrame();
			//window->update();
		}
		delete root;*/
		Game game;
		game.initApp();
		game.getRoot()->startRendering();
		game.closeApp();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error occurred during execution: " << e.what() << '\n';
		return 1;
	}
	return 0;
}