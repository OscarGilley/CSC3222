#pragma once
#include <vector>
#include "Math.h"
#include "SimObject.h"
#include "../../Common/Vector2.h"

namespace NCL {
	namespace CSC3222 {
		class RigidBody;
		class CollisionVolume;

		class GameSimsPhysics	{
		public:
			GameSimsPhysics(BadFoodGame* badFood);
			~GameSimsPhysics();

			void Update(float dt);

			std::vector<CollisionVolume*> getColliders();
			void AddRigidBody(RigidBody* b);
			void RemoveRigidBody(RigidBody* b);

			void AddCollider(CollisionVolume* c);
			void RemoveCollider(CollisionVolume* c);
			void CheckCollision(CollisionVolume* c1, CollisionVolume* c2);

		protected:
			void Integration(float dt);
			void CollisionDetection(float dt);
		
			BadFoodGame* badFood;
			std::vector<RigidBody*>			allBodies;
			std::vector<CollisionVolume*>	allColliders;
		};
	}
}

