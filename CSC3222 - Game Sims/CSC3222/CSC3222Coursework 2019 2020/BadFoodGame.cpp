#include "BadFoodGame.h"
#include "SimObject.h"
#include "GameMap.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "GameSimsPhysics.h"
#include "CollisionVolume.h"
#include "PlayerCharacter.h"
#include "BadRobot.h"
#include "Laser.h"
#include "Food.h"
#include "Coin.h"
#include "Balloon.h"

#include "../../Common/Window.h"
#include "../../Common/TextureLoader.h"

using namespace NCL;
using namespace CSC3222;
bool drawCollisionBoxes = false;
float coinTimer = 0;
float foodTimer = 0;
float robotTimer = 0;
float invulnTimer = 0;
bool invincible = false;


BadFoodGame::BadFoodGame()	{
	renderer	= new GameSimsRenderer();
	texManager	= new TextureManager();
	physics		= new GameSimsPhysics(this);
	SimObject::InitObjects(this, texManager);
	InitialiseGame();
}

BadFoodGame::~BadFoodGame()	{
	delete currentMap;
	delete texManager;
	delete renderer;
	delete physics;
}

void BadFoodGame::Update(float dt) {
	for (auto i : newObjects) {
		gameObjects.emplace_back(i);
	}
	newObjects.clear();

	gameTime += dt;

	renderer->Update(dt);
	physics->Update(dt);
	currentMap->DrawMap(*renderer);
	Flocking(allRobots, dt);

	srand((int)(gameTime * 1000.0f));

	for (auto i = gameObjects.begin(); i != gameObjects.end(); ) {
		if (!(*i)->UpdateObject(dt)) { //object has said its finished with
			physics->RemoveRigidBody(*i);
			//physics->RemoveCollider();
			delete (*i);
			i = gameObjects.erase(i);
			
		}
		else {
			(*i)->DrawObject(*renderer);
			++i;
		}
	}	
	renderer->DrawString("B4DF00D", Vector2(420, 700));

	renderer->DrawString("Score: " + std::to_string(currentScore), Vector2(10, 10));
	renderer->DrawString("Lives left: " + std::to_string(lives), Vector2(10, 30));
	renderer->DrawString("Food: " + std::to_string(foodCount), Vector2(10, 50));
	renderer->DrawString("Coins: " + std::to_string(coins), Vector2(10, 70));
	renderer->DrawString("Balloons: " + std::to_string(balloons), Vector2(10, 90));
	renderer->DrawString("Press F1 to show colliders!", Vector2(10, 110));
	/*

	Some examples of debug rendering!

	*/
	//renderer->DrawBox(Vector2(16,16), Vector2(8, 8), Vector4(1, 0, 0, 1));
	//renderer->DrawLine(Vector2(16, 16), Vector2(192, 192), Vector4(1, 1, 0, 1));
	//renderer->DrawCircle(Vector2(100, 100), 10.0f, Vector4(1, 0, 1, 1));
	

	

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1)) {
		drawCollisionBoxes = true;
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F2)) {
		drawCollisionBoxes = false;
	}

	if (drawCollisionBoxes) {
		for (auto collider : physics->getColliders()) {
			if (collider->getShape() == CollisionVolume::Shape::SQUARE) {
				renderer->DrawBox(*collider->getPosition(), Vector2(collider->getHalfX(), collider->getHalfY()), Vector4(0, 1, 1, 1));
			}

			if (collider->getShape() == CollisionVolume::Shape::CIRCLE) {
				renderer->DrawCircle(*collider->getPosition(), collider->getRadius(), Vector4(0, 1, 1, 1));
			}
		}
	}

	


	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE)) {
		if (balloons > 0) {
			balloons--;
			invincible = true;
			invulnTimer = 5;
		}
	}
	
	if (invulnTimer > 0) {
		invulnTimer -= dt;
	}
	

	if (invulnTimer <= 0) {
		invincible = false;
	}

	coinTimer += dt;
	foodTimer += dt;
	robotTimer += dt;

	if ((int)coinTimer == 15) {
		
		bool notPlaced = true;

		while (notPlaced) {

			int x = rand() % currentMap->GetMapWidth();
			int y = rand() % currentMap->GetMapHeight();
			int tileIndex = (y * currentMap->GetMapWidth() + x);

			if (currentMap->mapCosts[tileIndex] == 2 || currentMap->mapCosts[tileIndex] == 5) {
				Coin* coin = new Coin();
				coin->SetPosition(Vector2(x * 16 + 8, y * 16 + 8));
				coin->SetCollider(new CollisionVolume(coin, 8, 10.0f, 0.9f, CollisionVolume::Type::COIN));
				AddNewObject(coin);
				notPlaced = false;
				allCoins.emplace_back(coin);
				
			}

			
			
		}
		
		coinTimer -= 15;
	}

	if ( (int) foodTimer == 5) {

		bool notPlaced = true;

		while (notPlaced) {

			int x = rand() % currentMap->GetMapWidth();
			int y = rand() % currentMap->GetMapHeight();
			int tileIndex = (y * currentMap->GetMapWidth() + x);

			if (currentMap->mapCosts[tileIndex] == 2 || currentMap->mapCosts[tileIndex] == 5) {
				Food* food = new Food();
				food->SetPosition(Vector2(x * 16 + 8, y * 16 + 8));
				food->SetCollider(new CollisionVolume(food, 8, 10.0f, 0.9f, CollisionVolume::Type::FOOD));
				AddNewObject(food);
				notPlaced = false;
				allFood.emplace_back(food);
			}
		
		}
		foodTimer -= 5;
	}

	if ((int)robotTimer == 15 && allRobots.size() <= 15) {

		bool notPlaced = true;

		while (notPlaced) {

			int x = 14 + rand() % 2;
			int y = 5 + rand() % 1;
			int tileIndex = (y * currentMap->GetMapWidth() + x);

			if (currentMap->mapCosts[tileIndex] == 2) {
				BadRobot* robot = new BadRobot();
				robot->SetPosition(Vector2(x * 16 + 8, y * 16 + 8));
				robot->SetCollider(new CollisionVolume(robot, 8, 10.0f, 0.9f, CollisionVolume::Type::BADROBOT));
				AddNewObject(robot);
				notPlaced = false;
				allRobots.emplace_back(robot);
			}

		}
		robotTimer -= 15;
	}

	

	renderer->Render();
}

int BadFoodGame::getLives() {
	return lives;
}

void BadFoodGame::InitialiseGame() {
	delete currentMap;
	for (auto o : gameObjects) {
		delete o;
	}
	gameObjects.clear();
	

	currentMap = new GameMap("BadFoodMap.txt", gameObjects, *texManager, physics);

	renderer->SetScreenProperties(16, currentMap->GetMapWidth(), currentMap->GetMapHeight());

	player = new PlayerCharacter();
	player->SetCharacterType(PlayerCharacter::CharacterType::TYPE_B);
	player->SetPosition(Vector2(50, 120));
	player->SetCollider(new CollisionVolume(player, 8, 10.0f, 0.1f, CollisionVolume::Type::PLAYER));
	AddNewObject(player);

	/*
	BadRobot* testRobot1 = new BadRobot();
	allRobots.emplace_back(testRobot1);
	testRobot1->SetPosition(Vector2(180, 200));
	testRobot1->SetCollider(new CollisionVolume(testRobot1, 8, 8, 10.0f, 0.9f, CollisionVolume::Type::BADROBOT));
	AddNewObject(testRobot1);

	BadRobot* testRobot2 = new BadRobot();
	allRobots.emplace_back(testRobot2);
	testRobot2->SetPosition(Vector2(210, 200));
	testRobot2->SetCollider(new CollisionVolume(testRobot2, 8, 8, 10.0f, 0.9f, CollisionVolume::Type::BADROBOT));
	AddNewObject(testRobot2);

	BadRobot* testRobot3 = new BadRobot();
	allRobots.emplace_back(testRobot3);
	testRobot3->SetPosition(Vector2(240, 200));
	testRobot3->SetCollider(new CollisionVolume(testRobot3, 8, 8, 10.0f, 0.9f, CollisionVolume::Type::BADROBOT));
	AddNewObject(testRobot3);

	BadRobot* testRobot4 = new BadRobot();
	allRobots.emplace_back(testRobot4);
	testRobot4->SetPosition(Vector2(270, 200));
	testRobot4->SetCollider(new CollisionVolume(testRobot4, 8, 8, 10.0f, 0.9f, CollisionVolume::Type::BADROBOT));
	AddNewObject(testRobot4);
	*/
	/*
	Food* testFood = new Food();
	testFood->SetPosition(Vector2(250, 200));
	testFood->SetCollider(new CollisionVolume(testFood, 8, 10.0f, 0.9f, CollisionVolume::Type::FOOD));
	AddNewObject(testFood);

	Coin* testCoin = new Coin();
	testCoin->SetPosition(Vector2(300, 200));
	testCoin->SetCollider(new CollisionVolume(testCoin, 8, 8, 10.0f, 0.9f, CollisionVolume::Type::COIN));
	AddNewObject(testCoin);
	*/

	currentScore = 0;
	foodCount = 0;
	coins = 0;
	balloons = 0;
	lives = 3;
	
}

void BadFoodGame::ResetGame() {

	for (auto o : gameObjects) {
		o->IsDeleted();
	}
	

	for (int i = 0; i < allCoins.size(); i++){
		allCoins.at(i)->IsDeleted();
		
	}

	for (int j = 0; j < allFood.size(); j++) {
		allFood.at(j)->IsDeleted();
		
	}

	
	/*
	Food* testFood = new Food();
	testFood->SetPosition(Vector2(250, 200));
	testFood->SetCollider(new CollisionVolume(testFood, 8, 10.0f, 0.9f, CollisionVolume::Type::FOOD));
	AddNewObject(testFood);

	Coin* testCoin = new Coin();
	testCoin->SetPosition(Vector2(300, 200));
	testCoin->SetCollider(new CollisionVolume(testCoin, 8, 8, 10.0f, 0.9f, CollisionVolume::Type::COIN));
	AddNewObject(testCoin);
	*/

	std::cout << "\nResetting game";

	coinTimer = 0;
	foodTimer = 0;
	robotTimer = 0;

	
	foodCount = 0;
	coins = 0;
	balloons = 0;

	if (lives == 0) {
		std::cout << "\nGame Over! Final Score: " << currentScore;
		currentScore = 0;
		lives = 3;
	}
	

}

void BadFoodGame::AddNewObject(SimObject* object) {
	newObjects.emplace_back(object);
	physics->AddRigidBody(object);
	if (object->GetCollider()) {
		physics->AddCollider(object->GetCollider());
	}
}

void BadFoodGame::Flocking(std::vector<BadRobot*> allRobots, float dt) {
	for (BadRobot* b : allRobots) {
		Vector3 dir;
		dir += b->Allignment(allRobots);
		dir += b->Separation(allRobots);
		dir += b->Cohesion(allRobots);
		dir += b->Avoidance(allRobots);

		//std::cout << "\nFinal dir: " << dir * 10 * dt;

		b->SetPosition(b->GetPosition() + (dir * 15 * dt));
	}
}

void BadFoodGame::ResolveCollision(CollisionVolume* attacker, CollisionVolume* defender, Vector2 normal, float penetration) {

	bool noResolution = false;
	bool scoreIncreased = false;


	if (attacker->getType() == CollisionVolume::Type::LASER) {
		if (defender->getType() == CollisionVolume::Type::BADROBOT) {

			Laser* test = dynamic_cast<Laser*>(attacker->getSimObject());

			if (test->activeIn <= 0) {
				defender->getSimObject()->HitByLaser();
			}
		}

		if (defender->getType() == CollisionVolume::Type::LASER) {
			//noResolution = true;
		}
	}

	

	if (defender->getType() == CollisionVolume::Type::LASER) {
		if (attacker->getType() == CollisionVolume::Type::BADROBOT) {

			Laser* test = dynamic_cast<Laser*>(defender->getSimObject());

			if (test->activeIn <= 0) {
				attacker->getSimObject()->HitByLaser();
			}
		}

		if (attacker->getType() == CollisionVolume::Type::LASER) {
			//noResolution = true;
		}

		
	}

	if (attacker->getType() == CollisionVolume::Type::PLAYER) {
		if (defender->getType() == CollisionVolume::Type::COIN) {
			coins++;

			physics->RemoveCollider(defender);
			physics->RemoveRigidBody(defender->getSimObject());

			for (int i = 0; i < gameObjects.size(); i++) {
				if (gameObjects.at(i)->GetCollider() == defender) {
					defender->getSimObject()->IsDeleted();
				}
			}

			noResolution = true;

		}

		if (defender->getType() == CollisionVolume::Type::FOOD) {

			if (foodCount < 5) {
				foodCount++;

				physics->RemoveCollider(defender);
				physics->RemoveRigidBody(defender->getSimObject());

				for (int i = 0; i < gameObjects.size(); i++) {
					if (gameObjects.at(i)->GetCollider() == defender) {
						defender->getSimObject()->IsDeleted();
					}
				}


				noResolution = true;
			}
		}

		if (defender->getType() == CollisionVolume::Type::BADROBOT) {
			if (invincible) {
				noResolution = true;
			}
		}

		if (defender->getType() == CollisionVolume::Type::LASER) {

			if (!invincible) {
				lives--;

				attacker->getSimObject()->SetPosition(Vector2(50, 120));

				noResolution = true;

				
				ResetGame();
				
			}

			if (invincible) {
				noResolution = true;
			}
		}

		if (defender->getType() == CollisionVolume::Type::BALLOON_SHOP) {

			if (coins > 0 && balloons == 0) {
				balloons++;
				Balloon* balloon = new Balloon(player->GetPositionPointer());
				balloon->SetPosition(Vector2(player->GetPosition()));
				AddNewObject(balloon);
				coins--;
			}
		}

		
		if (defender->getType() == CollisionVolume::Type::FOOD_SHOP && !scoreIncreased) {

			if (foodCount > 0) {

				int first = 0;
				int second = 1;
				int third;

				for (int i = 2; i < foodCount + 3; i++) {
					third = first + second;
					first = second;
					second = third;
				}

				currentScore += 100 * (third);

				foodCount = 0;
				scoreIncreased = true;
			}
		}

	}

	if (defender->getType() == CollisionVolume::Type::PLAYER) {
		if (attacker->getType() == CollisionVolume::Type::COIN) {
			coins++;

			physics->RemoveCollider(attacker);
			physics->RemoveRigidBody(attacker->getSimObject());

			for (int i = 0; i < gameObjects.size(); i++) {
				if (gameObjects.at(i)->GetCollider() == attacker) {
					attacker->getSimObject()->IsDeleted();
				}
			}

			noResolution = true;

		}

		if (attacker->getType() == CollisionVolume::Type::FOOD) {
			if (foodCount < 5) {
				foodCount++;

				physics->RemoveCollider(attacker);
				physics->RemoveRigidBody(attacker->getSimObject());

				for (int i = 0; i < gameObjects.size(); i++) {
					if (gameObjects.at(i)->GetCollider() == attacker) {
						attacker->getSimObject()->IsDeleted();
					}
				}

				noResolution = true;
			}
		}

		if (attacker->getType() == CollisionVolume::Type::BADROBOT) {
			if (invincible) {
				noResolution = true;
			}
		}

		if (attacker->getType() == CollisionVolume::Type::LASER) {
			if (!invincible) {
				lives--;

				attacker->getSimObject()->SetPosition(Vector2(50, 120));

				noResolution = true;

				
				ResetGame();
				
			}

			if (invincible) {
				noResolution = true;
			}
		}

		if (attacker->getType() == CollisionVolume::Type::BALLOON_SHOP) {

			if (coins > 0 && balloons == 0) {
				balloons++;
				Balloon* balloon = new Balloon(player->GetPositionPointer());
				balloon->SetPosition(Vector2(player->GetPosition()));
				AddNewObject(balloon);
				coins--;
			}
		}

		if (attacker->getType() == CollisionVolume::Type::FOOD_SHOP && !scoreIncreased) {

			if (foodCount > 0) {

				int first = 0;
				int second = 1;
				int third;

				for (int i = 2; i < foodCount + 3; i++) {
					third = first + second;
					first = second;
					second = third;
				}

				currentScore += 100 * (third);

				foodCount = 0;
				scoreIncreased = true;
			}
		}
	}

	float e = attacker->getElasticity() * defender->getElasticity();

	if (attacker->getType() == CollisionVolume::Type::LASER) {
		e = 1;
		if (defender->getType() != CollisionVolume::Type::PLAYER) {

			Laser* test = dynamic_cast<Laser*>(attacker->getSimObject());
			if (test->activeIn <= 0) {
				test->LaserBounced();
			}
		
		}
	}

	if (defender->getType() == CollisionVolume::Type::LASER) {
		e = 1;
		if (attacker->getType() != CollisionVolume::Type::PLAYER) {
			
			Laser* test = dynamic_cast<Laser*>(defender->getSimObject());
			if (test->activeIn <= 0) {
				test->noBounces = test->noBounces - 1;
			}
		}
	}

	if (!noResolution) {
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

		else {
			defVelocity = defender->getSimObject()->GetVelocity();
		}

		if (e > 0) {

			Vector2 relativeVelocity = attVelocity - defVelocity;

			float impulse = (Vector2::Dot(normal, relativeVelocity)) * -(1 + e) / totalMass;

			if (attacker->getSimObject() != nullptr) {
				attacker->getSimObject()->SetVelocity(Vector2(attVelocity.x - (attacker->getInverseMass() * impulse) * -normal.x,
					attVelocity.y - (attacker->getInverseMass() * impulse) * -normal.y));
			}

			if (defender->getSimObject() != nullptr) {
				defender->getSimObject()->SetVelocity(Vector2(defVelocity.x + (defender->getInverseMass() * impulse) * -normal.x,
					defVelocity.y + (defender->getInverseMass() * impulse) * -normal.y));
			}

		}
	}


	
}