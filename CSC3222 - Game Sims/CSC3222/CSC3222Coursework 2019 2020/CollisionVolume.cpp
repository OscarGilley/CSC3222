#include "CollisionVolume.h"

using namespace NCL;
using namespace CSC3222;


CollisionVolume::CollisionVolume(SimObject* object, int halfX, int halfY, float inverseMass, float elasticity, Type type)
{
	this->simObject = object;
	this->position = object->GetPositionPointer();
	this->halfX = halfX;
	this->halfY = halfY;
	this->shape = SQUARE;
	this->radius = 0;
	this->inverseMass = inverseMass;
	this->elasticity = elasticity;
	this->type = type;
	
}

CollisionVolume::CollisionVolume(SimObject* object, int radius, float inverseMass, float elasticity, Type type)
{
	this->simObject = object;
	this->position = object->GetPositionPointer();
	this->radius = radius;
	this->shape = CIRCLE;
	this->halfX = 0;
	this->halfY = 0;
	this->inverseMass = inverseMass;
	this->elasticity = elasticity;
	this->type = type;
	
}

CollisionVolume::CollisionVolume(Vector2* position, int halfX, int halfY, float inverseMass, float elasticity, Type type) {
	this->simObject = nullptr;
	this->position = position;
	this->halfX = halfX;
	this->halfY = halfY;
	this->shape = SQUARE;
	this->radius = 0;
	this->inverseMass = inverseMass;
	this->elasticity = elasticity;
	this->type = type;

}

CollisionVolume::CollisionVolume(Vector2* position, int radius, float inverseMass, float elasticity, Type type)
{
	this->simObject = nullptr;
	this->position = position;
	this->radius = radius;
	this->shape = CIRCLE;
	this->halfX = 0;
	this->halfY = 0;
	this->inverseMass = inverseMass;
	this->elasticity = elasticity;
	this->type = type;
}

CollisionVolume::~CollisionVolume()
{

}

Vector2* CollisionVolume::getPosition() const {
	return position;
}

CollisionVolume::Shape CollisionVolume::getShape() const {
	return shape;
}

int CollisionVolume::getRadius() const {
	return radius;
}

int CollisionVolume::getHalfY() const {
	return halfY;
}

int CollisionVolume::getHalfX() const {
	return halfX;
}

float CollisionVolume::getInverseMass() const {
	return inverseMass;
}

SimObject* CollisionVolume::getSimObject() const{
	return simObject;
}

float CollisionVolume::getElasticity() const {
	return elasticity;
}

CollisionVolume::Type CollisionVolume::getType() const {
	return type;
}
