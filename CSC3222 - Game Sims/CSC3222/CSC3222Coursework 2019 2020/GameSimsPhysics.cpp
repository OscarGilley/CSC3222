#include "GameSimsPhysics.h"
#include "BadFoodGame.h"
#include "RigidBody.h"
#include "Math.h"
#include "CollisionVolume.h"
#include "../../Common/Vector2.h"
#include <chrono>
#include <ctime>
#include <algorithm>

using namespace NCL;
using namespace CSC3222;
float timeRemaining;
auto start = std::chrono::system_clock::now();
const float subTimeDelta = 1.0 / 120; //120fps

GameSimsPhysics::GameSimsPhysics(BadFoodGame* badFoodGame)	{
	badFood = badFoodGame;
}

GameSimsPhysics::~GameSimsPhysics()	{

}

void GameSimsPhysics::Update(float dt) {

	timeRemaining += dt;

	

	while (timeRemaining > subTimeDelta) {

		Integration(subTimeDelta);
		CollisionDetection(subTimeDelta);
		for (int i = 0; i < allBodies.size(); i++) {
			allBodies.at(i)->force = Vector2(0, 0);
		}

		timeRemaining -= subTimeDelta;
	}

	
}

std::vector<CollisionVolume*> GameSimsPhysics::getColliders() {
	return allColliders;
}

void GameSimsPhysics::AddRigidBody(RigidBody* b) {
	allBodies.emplace_back(b);
}

void GameSimsPhysics::RemoveRigidBody(RigidBody* b) {
	auto at = std::find(allBodies.begin(), allBodies.end(), b);

	if (at != allBodies.end()) {
		//maybe delete too?
		allBodies.erase(at);
	}
}

void GameSimsPhysics::AddCollider(CollisionVolume* c) {
	allColliders.emplace_back(c);
}

void GameSimsPhysics::RemoveCollider(CollisionVolume* c) {
	auto at = std::find(allColliders.begin(), allColliders.end(), c);

	if (at != allColliders.end()) {
		//maybe delete too?
		allColliders.erase(at);
	}
}

float clamp(float val, float minValue, float maxValue) {

	val = std::min(val, maxValue);
	val = std::max(val, minValue);
	return val;
}

void GameSimsPhysics::Integration(float dt) {
	for (int i = 0; i < allBodies.size(); i++) {
		Vector2 acceleration = allBodies.at(i)->force * allBodies.at(i)->inverseMass;
		allBodies.at(i)->velocity = allBodies.at(i)->velocity + (acceleration * dt);
		allBodies.at(i)->position = allBodies.at(i)->position + (allBodies.at(i)->velocity * dt);
		if (allBodies.at(i)->isLaser == false) {
			allBodies.at(i)->velocity *= 0.98f;
		}
	}
}

void GameSimsPhysics::CollisionDetection(float dt) {
	for (int i = 0; i < allColliders.size() - 1; i++) {
		for (int j = i + 1; j < allColliders.size(); j++) {
			bool bothWalls = false;

			if (allColliders.at(i)->getType() == CollisionVolume::Type::WALL && allColliders.at(j)->getType() == CollisionVolume::Type::WALL) {

				bothWalls = true;


			}

			if (!bothWalls) {
				CheckCollision(allColliders.at(i), allColliders.at(j));
			}
			
		}


	}
}

void GameSimsPhysics::CheckCollision(CollisionVolume* attacker, CollisionVolume* defender) {

	//square square collision
	if (attacker->getShape() == CollisionVolume::Shape::SQUARE && defender->getShape() == CollisionVolume::Shape::SQUARE) {

		float deltaX = attacker->getPosition()->x - defender->getPosition()->x; //yes
		float deltaY = attacker->getPosition()->y - defender->getPosition()->y;


		if (attacker->getHalfX() + defender->getHalfX() > abs(deltaX)){
		
			if (attacker->getHalfY() + defender->getHalfY() > abs(deltaY)){
			
				auto collisionTime = std::chrono::system_clock::now();
				std::chrono::duration<double> elapsedTime = collisionTime - start;

				Vector2 normal;

				float penetrationX = attacker->getHalfX() + defender->getHalfX() - abs(deltaX);
				float penetrationY = attacker->getHalfY() + defender->getHalfY() - abs(deltaY);

				if (penetrationX > penetrationY) {

					if (attacker->getPosition()->y > defender->getPosition()->y) {
						normal = Vector2(0, -1);
					}

					else {
						normal = Vector2(0, 1);
					}

					badFood->ResolveCollision(attacker, defender, normal, penetrationY);

				}

				else {

					if (attacker->getPosition()->x > defender->getPosition()->x) {
						normal = Vector2(-1, 0);
					}

					else {
						normal = Vector2(1, 0);
					}

					badFood->ResolveCollision(attacker, defender, normal, penetrationX);

				}

				

				//std::cout << "\nSquare-Square hit detected! At time: " << elapsedTime.count() << "s";
			}
		}

	}

	//circle-circle collision
	else if (attacker->getShape() == CollisionVolume::Shape::CIRCLE && defender->getShape() == CollisionVolume::Shape::CIRCLE) {
		float deltaX = (attacker->getPosition()->x - defender->getPosition()->x);
		float deltaY = (attacker->getPosition()->y - defender->getPosition()->y);

		//if i've actually collided, resolve that in a new method
		if (attacker->getRadius() + defender->getRadius() >= sqrt((deltaX * deltaX) + (deltaY * deltaY))) {
			auto collisionTime = std::chrono::system_clock::now();


			Vector2 normal = Vector2(-deltaX, -deltaY);
			normal.Normalise();

			float penetration = (attacker->getRadius() + defender->getRadius()) - sqrt((deltaX * deltaX) + (deltaY * deltaY));

			badFood->ResolveCollision(attacker, defender, normal, penetration);

			std::chrono::duration<double> elapsedTime = collisionTime - start;

			//std::cout << "\nCircle-Circle hit detected! At time: " << elapsedTime.count() << "s";
		}

	}

	//square-circle collision 
	else if (attacker->getShape() != defender->getShape()) {

		bool csCollide = false;


		//square-circle attacker is square, defender is circle
		if (attacker->getShape() == CollisionVolume::Shape::SQUARE) {

			float closestX = clamp(defender->getPosition()->x, attacker->getPosition()->x - attacker->getHalfX(), attacker->getPosition()->x + attacker->getHalfX());
			float closestY = clamp(defender->getPosition()->y, attacker->getPosition()->y - attacker->getHalfY(), attacker->getPosition()->y + attacker->getHalfY());

			float betweenX = defender->getPosition()->x - closestX;
			float betweenY = defender->getPosition()->y - closestY;

			float closestDist = sqrt((betweenX * betweenX) + (betweenY * betweenY));

			if (defender->getRadius() > closestDist) {
				auto collisionTime = std::chrono::system_clock::now();

				Vector2 normal = Vector2(betweenX, betweenY);
				normal.Normalise();

				float penetration = abs(closestDist - defender->getRadius());

				badFood->ResolveCollision(attacker, defender, normal, penetration);

				std::chrono::duration<double> elapsedTime = collisionTime - start;



				//std::cout << "\nSquare-Circle hit detected! At time: " << elapsedTime.count() << "s";
			}
		}

		//circle-square attacker is circle, defender is square
		else {
			
			float closestX = clamp(attacker->getPosition()->x, defender->getPosition()->x - defender->getHalfX(), defender->getPosition()->x + defender->getHalfX());
			float closestY = clamp(attacker->getPosition()->y, defender->getPosition()->y - defender->getHalfY(), defender->getPosition()->y + defender->getHalfY());

			float betweenX = attacker->getPosition()->x - closestX;
			float betweenY = attacker->getPosition()->y - closestY;

			float closestDist = sqrt((betweenX * betweenX) + (betweenY * betweenY));

			if (attacker->getRadius() > closestDist) {
				auto collisionTime = std::chrono::system_clock::now();

				Vector2 normal = Vector2(-betweenX, -betweenY);
				normal.Normalise();

				float penetration = abs(closestDist - attacker->getRadius());

				badFood->ResolveCollision(attacker, defender, normal, penetration);


				std::chrono::duration<double> elapsedTime = collisionTime - start;

				//std::cout << "\nCircle-Square hit detected! At time: " << elapsedTime.count() << "s";
			}
		}
	}

}

/*
void GameSimsPhysics::ResolveCollision(CollisionVolume* attacker, CollisionVolume* defender, Vector2 normal, float penetration) {
	float totalMass = attacker->getInverseMass() + defender->getInverseMass();

	Vector2 aPos = (normal * penetration * (attacker->getInverseMass() / totalMass));
	Vector2 bPos = (normal * penetration * (defender->getInverseMass() / totalMass));
	attacker->getPosition()->x -= aPos.x;
	attacker->getPosition()->y -= aPos.y;
	defender->getPosition()->x += bPos.x;
	defender->getPosition()->y += bPos.y;

	Vector2 attVelocity;
	Vector2 defVelocity;

	if (attacker->getSimObject() == nullptr) {
		attVelocity = Vector2(0, 0);
	}

	else {
		attVelocity = attacker->getSimObject()->GetVelocity();
	}

	if (defender->getSimObject() == nullptr) {
		defVelocity = Vector2(0, 0);
	}

	else{
		defVelocity = defender->getSimObject()->GetVelocity();
	}

	float e = attacker->getElasticity() * defender->getElasticity();

	if (e > 0) {

		Vector2 relativeVelocity = attVelocity - defVelocity;

		float impulse = (Vector2::Dot(normal, relativeVelocity)) * -(1 + e) / totalMass;

		if (attacker->getSimObject() != nullptr) {
			attacker->getSimObject()->SetVelocity(Vector2(attVelocity.x - (attacker->getInverseMass() * impulse) * -normal.x,
				attVelocity.y - (attacker->getInverseMass() * impulse) * -normal.y));
		}

		if(defender->getSimObject() != nullptr){
			defender->getSimObject()->SetVelocity(Vector2(defVelocity.x + (defender->getInverseMass() * impulse) * -normal.x,
				defVelocity.y + (defender->getInverseMass() * impulse) * -normal.y));
		}
		
	}

}
*/



