#pragma once
#include <vector>
#include "Math.h"
#include "CollisionVolume.h"
#include "Food.h"
#include "Coin.h"
#include "BadRobot.h"


namespace NCL {
	namespace Maths {
		class Vector2;
	}
	namespace CSC3222 {
		class GameMap;
		class SimObject;
		class GameSimsRenderer;
		class GameSimsPhysics;
		class TextureBase;
		class TextureManager;
		class PlayerCharacter;

		class BadFoodGame {
		public:
			BadFoodGame();
			~BadFoodGame();

			void Update(float dt);
			void ResolveCollision(CollisionVolume* c1, CollisionVolume* c2, Vector2 v, float f);
			void AddNewObject(SimObject* object);
			void ResetGame();
			GameSimsPhysics* physics;
			int getLives();
			std::vector<Coin*> allCoins;
			std::vector<Food*> allFood;
			std::vector<BadRobot*> allRobots;
			int foodCount;
			PlayerCharacter* player;
			GameMap* currentMap;
			void Flocking(std::vector<BadRobot*> allRobots, float dt);
			GameSimsRenderer* renderer;

		protected:
			void InitialiseGame();

			TextureManager*		texManager;
			
			

			

			float gameTime;
			int currentScore;
			
			int coins;
			int balloons;
			int lives;
			std::vector<SimObject*> gameObjects;
			std::vector<SimObject*> newObjects;
			
			
		};
	}
}

