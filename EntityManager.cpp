#include "EntityManager.h"
#include <iostream>

EntityManager::EntityManager()
{

}

void EntityManager::update()
{
	// TODO: add entities from m_entitiesToAdd the proper location(s)
	//		 - add them to the vector of all entities
	//		 - add them to the vector inside the map, with tag as a key
	for (auto& e : m_entitiesToAdd)
	{
		std::cout << "Adding entity with tag: " << e->tag() << std::endl;
		m_entityMap[e->tag()].push_back(e);
		m_entities.push_back(e);
	}
	m_entitiesToAdd.clear();

	// remove dead entities from the vector of all entities
	removeDeadEntities(m_entities);

	// remove dead entities from each vector in the entity map
	// C++17 way of iterating through [key,value] pairs in a map
	for (auto& [tag, entityVec] : m_entityMap)
	{
		removeDeadEntities(entityVec);
	}
}

void EntityManager::removeDeadEntities(EntityVec& vec)
{
	// remove all dead entities from the input vector
	// this is called by the update function
	vec.erase(
		std::remove_if(vec.begin(), vec.end(), [](std::shared_ptr<Entity>& entity) 
		{
			return !entity->isActive();
		}), 
		vec.end()
	);
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
	auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));

	m_entitiesToAdd.push_back(entity);

	return entity;
}

const EntityVec& EntityManager::getEntities()
{
	return m_entities;
}

const EntityVec& EntityManager::getEntities(const std::string& tag)
{
	// return the vector from the map
	return m_entityMap[tag];
}