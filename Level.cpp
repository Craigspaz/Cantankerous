#include "Level.h"
#include <fstream>
#include "SystemDefines.h"

Level::Level(std::string filename, Ogre::SceneManager* manager)
{
	this->filename = filename;
	this->sceneManager = manager;
	tiles = new std::vector<Tile*>();

	loadLevel();
}

Level::~Level()
{
	
}

void Level::loadLevel()
{
	std::ifstream file(filename);
	std::string line;
	int row = 0;
	int scale = 25;
	while (std::getline(file, line))
	{
		for (int i = 0; i < line.length(); i++)
		{
			if (line.at(i) == 'g') // grass
			{
				Tile* tile = new Tile(Ogre::Vector3(i, 0, row) , sceneManager, GRASS_TILE, scale);
				tiles->push_back(tile);
			}
			else if (line.at(i) == 'd') // dirt
			{
				Tile* tile = new Tile(Ogre::Vector3(i, 0, row), sceneManager, DIRT_TILE, scale);
				tiles->push_back(tile);
			}
			else if (line.at(i) == '\n' || line.at(i) == '\r\n')
			{

			}
		}
		row++;
	}
	file.close();
}