#include "BadRobot.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "BadFoodGame.h"
#include "GameSimsPhysics.h"
#include "GameMap.h"
#include "PlayerCharacter.h"
#include "Laser.h"
#include <vector>

using namespace NCL;
using namespace CSC3222;

Vector4 animFramesGreen[] = {
	Vector4(71,142,18,18),
	Vector4(102,142,20,18),
	Vector4(135,142,19,18),
	Vector4(166,142,19,18),
	Vector4(198,140,20,18)
};

Vector4 animFramesWhite[] = {
	Vector4(71,266,17,22),
	Vector4(104,265,19,22),
	Vector4(137,266,17,22),
	Vector4(168,266,17,22),
	Vector4(200,264,17,23)
};

BadRobot::BadRobot() : SimObject() {
	texture = texManager->GetTexture("TL_Creatures.png");
	animFrameCount	= 4;

	
	state = State::PATROL;

	int r = rand() % 2;

	if (r == 0) {
		type = RobotType::Green;
	}
	else {
		type = RobotType::White;
	}

	
}

BadRobot::~BadRobot() {

}

bool BadRobot::UpdateObject(float dt) {
	UpdateAnimFrame(dt);

	if (stunnedTimer > 0) {
		stunnedTimer -= dt;
		//std::cout << "\nI got hit :(";
	}

	

	if (stunnedTimer <= 0) {

		if (pathTimer <= 0) {
			pathFound = false;
		}

		switch (state) {
		case State::ATTACK: AttackPlayer(dt); break;
		case State::PROTECT: ProtectShop(); break;
		case State::PATROL: PatrolMap(); break;
		}
		
		pathTimer -= dt;
		
		

		if (!pathFound) {
			std::vector<Vector2> path = SimObject::game->currentMap->InitialiseNode(position, targetPos);
			pathFound = true;

			pathTimer = 5;
		}

		

		/*
		for (int i = path.size(); i > 0; i--) {

			while (position != path.at(i)) {
				Vector2 normalVector = Vector2((path.at(i).x - position.x), (path.at(i).y - position.y));
				float length = normalVector.Length();
				velocity = normalVector.Normalised() / 2;
				std::cout << "\nhelo";
				
			}

		}
		*/
	}
	
	

	if (hitByLaser == true) {
		stunnedTimer += 3;
		HitByLaser();
	}

	if (collided == true) {
		SimObject::game->physics->RemoveCollider(GetCollider());
		SimObject::game->physics->RemoveRigidBody(this);
		SimObject::game->allRobots.erase(std::remove(game->allRobots.begin(), game->allRobots.end(), this), game->allRobots.end());
		return false;
	}
	

	return true;
}

void BadRobot::DrawObject(GameSimsRenderer& r) {
	Vector2	texPos;
	Vector2 texSize;

	switch (type) {
		case RobotType::Green:{
			texPos	= Vector2(animFramesGreen[currentanimFrame].x, animFramesGreen[currentanimFrame].y);
			texSize = Vector2(animFramesGreen[currentanimFrame].z, animFramesGreen[currentanimFrame].w);
		}break;
		case RobotType::White:{
			texPos	= Vector2(animFramesWhite[currentanimFrame].x, animFramesWhite[currentanimFrame].y);
			texSize = Vector2(animFramesWhite[currentanimFrame].z, animFramesWhite[currentanimFrame].w);
		}break;
	}

	r.DrawTextureArea((OGLTexture*)texture, texPos, texSize, position, false);
}

void BadRobot::AttackPlayer(float dt) {
	Vector2 targetPos = SimObject::game->player->GetPosition();
	Vector2 playerPos = SimObject::game->player->GetPosition();
	Vector2 normalVector = Vector2((SimObject::game->player->GetPosition().x - position.x), (SimObject::game->player->GetPosition().y - position.y));

	shootTimer += dt;

	
	//std::cout << "\nattack mode";
	//std::cout << "\n" << shootTimer;

	if (shootTimer >= 2) {
		
		Laser* firedLaser = new Laser(normalVector);
		firedLaser->SetPosition(Vector2(position.x, position.y));
		firedLaser->SetCollider(new CollisionVolume(firedLaser, 8, 4, 10.0f, 0.9f, CollisionVolume::Type::LASER));
		SimObject::game->AddNewObject(firedLaser);
		shootTimer -= 2;


	}

	float magnitude = sqrt(((SimObject::game->player->GetPosition().x - position.x) * (SimObject::game->player->GetPosition().x - position.x))
		+ ((SimObject::game->player->GetPosition().y - position.y) * (SimObject::game->player->GetPosition().y - position.y)));

	if (magnitude > 80) {
		shootTimer = 0;
		state = State::PATROL;
	}

}

void BadRobot::ProtectShop() {

	if (!pathFound) {
		targetPos = SimObject::game->currentMap->GetRandomPositionWithinRadius(Vector2(416, 80), 120);
	}
	//std::cout << "\nprotect mode";

	float magnitude = sqrt(((SimObject::game->player->GetPosition().x - position.x) * (SimObject::game->player->GetPosition().x - position.x))
		+ ((SimObject::game->player->GetPosition().y - position.y) * (SimObject::game->player->GetPosition().y - position.y)));

	if (SimObject::game->foodCount < 5) {

		Vector2 playerPos = SimObject::game->player->GetPosition();

		if (magnitude <= 80) {
			state = State::ATTACK;
		}

		else {
			state = State::PATROL;
		}
	}

	if (magnitude <= 80) {
		state = State::ATTACK;
	}
}

void BadRobot::PatrolMap() {

	if (!pathFound) {
		targetPos = SimObject::game->currentMap->GetRandomPosition();
	}

	//std::cout << "\npatrol mode";

	float magnitude = sqrt(((SimObject::game->player->GetPosition().x - position.x) * (SimObject::game->player->GetPosition().x - position.x))
		+ ((SimObject::game->player->GetPosition().y - position.y) * (SimObject::game->player->GetPosition().y - position.y)));

	//std::cout << "\n" << magnitude;

	if (magnitude <= 80) {
		state = State::ATTACK;
	}

	if (SimObject::game->foodCount >= 5) {
		state = State::PROTECT;
	}
}

//flocking

Vector3 BadRobot::Allignment(std::vector<BadRobot*> allRobots) {
	Vector3 dir = this->targetPos - this->position;

	for (BadRobot* b : allRobots) {
		if (b == this) {
			continue;
		}

		Vector2 vectDist = this->position - b->position;
		float distance = vectDist.Length();

		if (distance > alignmentThreshold) {
			continue;
		}

		dir += b->position;
	}

	//std::cout << "\nAllignment: " << dir.Normalised();

	return dir.Normalised();
}

Vector3 BadRobot::Separation(std::vector<BadRobot*> allRobots) {
	Vector3 dir;

	for (BadRobot* b : allRobots) {
		if (b == this) {
			continue;
		}
		Vector2 vectDist = this->position - b->position;
		float distance = vectDist.Length();

		if (distance > separationThreshold) {
			continue;
		}

		float strength = 1.0f - (distance / separationThreshold);
		dir += (this->position - b->position).Normalised() * strength;
	}

	//std::cout << "\nSeparation: " << dir.Normalised();

	return dir.Normalised();

}

Vector3 BadRobot::Cohesion(std::vector<BadRobot*> allRobots) {
	Vector3 avgPos = this->position;
	float count = 1;

	for (BadRobot* b : allRobots) {
		if (b == this) {
			continue;
		}
		Vector2 vectDist = this->position - b->position;
		float distance = vectDist.Length();
		
		if (distance > cohesionThreshold) {
			continue;
		}

		avgPos += b->position;
		count++;
	}

	avgPos /= count;
	Vector3 dir = avgPos - this->position;

	//std::cout << "\nCohesion: " << dir.Normalised();

	return dir.Normalised();
}

Vector3 BadRobot::Avoidance(std::vector<BadRobot*> allRobots) {
	Vector3 dir;

	for (BadRobot* b : allRobots) {
		if (b == this) {
			continue;
		}
		Vector2 vectDist = this->position - b->position;
		float distance = vectDist.Length();

		if (distance > avoidanceThreshold) {
			continue;
		}

		dir += (this->position - b->position).Normalised();
	}

	//std::cout << "\nAvoidance: " << dir.Normalised();

	return dir.Normalised();
}