#include "Level.h"
#include <fstream>
#include "SystemDefines.h"

Level::Level(std::string path, std::string filename, Ogre::SceneManager* manager)
{
	this->path = path;
	this->filename = filename;
	this->sceneManager = manager;
	tiles = new std::vector<Tile*>();
	minCorner = Ogre::Vector3(0, 0, 0);
	maxCorner = Ogre::Vector3(0, 0, 0);
	loadLevel();
}

Level::~Level()
{
	for (auto tile : *tiles)
	{
		delete tile;
	}
	delete tiles;
}

std::string Level::getFileName()
{
	return filename;
}

Ogre::Vector3 Level::getMinBoundary()
{
	return minCorner;
}

Ogre::Vector3 Level::getMaxBoundary()
{
	return maxCorner;
}

void Level::loadLevel()
{
	std::ifstream file(path + filename);
	std::string line;
	int row = 0;
	int scale = 25;
	while (std::getline(file, line))
	{
		width = line.length();
		for (int i = 0; i < line.length(); i++)
		{
			if (line.at(i) == 'g') // grass
			{
				Tile* tile = new Tile(Ogre::Vector3(i, 0, row) , sceneManager, GRASS_TILE, scale);
				tile->setGridPosition(Ogre::Vector2(i, row));
				tiles->push_back(tile);
				if (row == 0 && i == 0)
				{
					minCorner = tile->getPosition();
				}
				maxCorner = tile->getPosition();
			}
			else if (line.at(i) == 'd') // dirt
			{
				Tile* tile = new Tile(Ogre::Vector3(i, 0, row), sceneManager, DIRT_TILE, scale);
				tile->setGridPosition(Ogre::Vector2(i, row));
				tiles->push_back(tile);
				maxCorner = tile->getPosition();
			}
			else if (line.at(i) == '\n' || line.at(i) == '\r\n')
			{

			}
		}
		row++;
	}
	height = row;
	file.close();
}


std::vector<Tile*>* Level::getTiles()
{
	return tiles;
}


void Level::setWidth(int width)
{
	this->width = width;
}
void Level::setHeight(int height)
{
	this->height = height;
}
int Level::getWidth()
{
	return width;
}
int Level::getHeight()
{
	return height;
}