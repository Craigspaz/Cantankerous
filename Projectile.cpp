#include "Projectile.h"
#include "Unit.h"
#include "Building.h"
#include "Tile.h"



Projectile::Projectile(Ogre::Vector3 startingPosition, double damage, double missRate, Ogre::SceneManager* manager, double movementSpeed, int controllingPlayer, bool lockedOn, int id)
{
	this->sceneManager = manager;
	this->node = manager->getRootSceneNode()->createChildSceneNode();
	this->node->setPosition(startingPosition);
	entity = manager->createEntity(Ogre::SceneManager::PT_CUBE);
	node->attachObject(entity);
	node->setScale(node->getScale() / 80.0);
	this->damage = damage;
	this->missRate = missRate;
	this->movementSpeed = movementSpeed;
	this->lockedOn = lockedOn;
	unitTarget = NULL;
	buildingTarget = NULL;
	buildingTargetTile = NULL;
	destroyed = false;
	if (id == -1)
	{

	}
	else
	{
		this->id = id;
	}
	this->controllingPlayer = controllingPlayer;
}


Projectile::~Projectile()
{
	this->node->removeAndDestroyAllChildren();
	this->sceneManager->getRootSceneNode()->removeAndDestroyChild(this->node);
	this->sceneManager->destroyEntity(this->entity);
}


void Projectile::update()
{
	if (unitTarget != NULL)
	{
		if (lockedOn)
		{
			Ogre::Vector3 movementDirection = unitTarget->getPosition() - this->getPosition();
			if (movementDirection.length() < movementSpeed)
			{
				int random = rand() % 100;
				int rate = this->missRate * 100;
				if (random > rate)
				{
					unitTarget->takeDamage(damage);
				}
				this->entity->setVisible(false);
				this->destroyed = true;
			}
			Ogre::Vector3 movementAmount = movementDirection.normalisedCopy() * movementSpeed;
			this->setPosition(this->getPosition() + movementAmount);
		}
		else
		{
			static Tile* targetTile = unitTarget->getCurrentTile();
			Ogre::Vector3 targetPosition = targetTile->getPosition() + Ogre::Vector3(0, unitTarget->getPosition().y, 0);
			Ogre::Vector3 movementDirection = targetPosition - this->getPosition();
			if (movementDirection.length() < movementSpeed)
			{
				//handle miss rate
				unitTarget->takeDamage(damage);
				this->entity->setVisible(false);
				this->destroyed = true;
			}
			Ogre::Vector3 movementAmount = movementDirection.normalisedCopy() * movementSpeed;
			this->setPosition(this->getPosition() + movementAmount);
		}
	}
	else if (buildingTarget != NULL)
	{
		if (lockedOn)
		{
			Ogre::Vector3 movementDirection = buildingTarget->getPosition() - this->getPosition();
			if (movementDirection.length() < movementSpeed)
			{
				int random = rand() % 100;
				int rate = this->missRate * 100;
				if (random > rate)
				{
					buildingTarget->takeDamage(damage);
				}
				this->entity->setVisible(false);
				this->destroyed = true;
			}
			Ogre::Vector3 movementAmount = movementDirection.normalisedCopy() * movementSpeed;
			this->setPosition(this->getPosition() + movementAmount);
		}
		else
		{
			static Tile* targetTile = buildingTargetTile;
			Ogre::Vector3 targetPosition = targetTile->getPosition() + Ogre::Vector3(0, buildingTarget->getPosition().y, 0);
			Ogre::Vector3 movementDirection = targetPosition - this->getPosition();
			if (movementDirection.length() < movementSpeed)
			{
				//handle miss rate
				buildingTarget->takeDamage(damage);
				this->entity->setVisible(false);
				this->destroyed = true;
			}
			Ogre::Vector3 movementAmount = movementDirection.normalisedCopy() * movementSpeed;
			this->setPosition(this->getPosition() + movementAmount);
		}
	}
}

void Projectile::setTarget(Unit* unit)
{
	unitTarget = unit;
}

void Projectile::setTarget(Building* building, Tile* tile)
{
	buildingTarget = building;
	buildingTargetTile = tile;
}

void Projectile::setPosition(Ogre::Vector3 position)
{
	node->setPosition(position);
}

Ogre::Vector3 Projectile::getPosition()
{
	return node->getPosition();
}

bool Projectile::isDestroyed()
{
	return destroyed;
}

void Projectile::setDestroyed(bool a)
{
	this->destroyed = a;
}

int Projectile::getID()
{
	return id;
}


int Projectile::getControllingPlayer()
{
	return controllingPlayer;
}


void Projectile::setVisible(bool a)
{
	this->entity->setVisible(a);
}