#pragma once
#include "../../Common/Vector2.h"
#include "RigidBody.h"

namespace NCL {
	using namespace Maths;
	namespace Rendering {
		class TextureBase;
	}
	namespace CSC3222 {
		class GameSimsRenderer;
		class TextureManager;
		class BadFoodGame;
		class RigidBody;
		class CollisionVolume;

		class SimObject	: public RigidBody {
		public:
			SimObject();
			~SimObject();

			void SetCollider(CollisionVolume * c) {
				collider = c;
			}

			void IsDeleted() {
				collided = true;
			}

			void HitByLaser() {
				if (!hitByLaser) {
					hitByLaser = true;
				}

				else {
					hitByLaser = false;
				}
			}

			CollisionVolume* GetCollider() const {
				return collider;
			}

			virtual bool UpdateObject(float dt) = 0;

			virtual void DrawObject(GameSimsRenderer &r) = 0;

			static void InitObjects(BadFoodGame* game, TextureManager* manager);

		protected:

			CollisionVolume*		collider;
			Rendering::TextureBase* texture;

			void UpdateAnimFrame(float dt);

			int				currentanimFrame;
			int				animFrameCount;
			float			frameTime;
			bool			collided = false;
			bool			hitByLaser = false;
			static TextureManager*  texManager;
			static BadFoodGame*		game;
		};
	}
}

