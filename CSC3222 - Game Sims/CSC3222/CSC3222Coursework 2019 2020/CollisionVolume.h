#pragma once
#include "SimObject.h"
#include "RigidBody.h"
namespace NCL {
	namespace CSC3222 {
		class CollisionVolume {
		public:
			//class SimObject;
			enum Shape {
				CIRCLE,
				SQUARE
			};

			enum Type {
				PLAYER,
				BALLOON,
				BADROBOT,
				COIN,
				LASER,
				FOOD,
				WALL,
				BALLOON_SHOP,
				FOOD_SHOP
			};

			CollisionVolume(SimObject*, int, int, float, float, Type);
			CollisionVolume(SimObject*, int, float, float, Type);
			CollisionVolume(Vector2*, int, int, float, float, Type);
			CollisionVolume(Vector2*, int, float, float, Type);
			~CollisionVolume();
			SimObject* getSimObject() const;
			Vector2* getPosition() const;
			Shape getShape() const;
			int getRadius() const;
			int getHalfY() const;
			int getHalfX() const;
			Type getType() const;
			float getInverseMass() const;
			float getElasticity() const;
			

		protected:
			SimObject* simObject;
			Vector2* position;
			Shape shape;
			Type type;
			int halfX;
			int halfY;
			int radius;
			float inverseMass;
			float elasticity;


		};
	}
}

