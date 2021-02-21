#pragma once
#include "SimObject.h"

namespace NCL {
	namespace CSC3222 {
		class Coin :	public SimObject
		{
		public:
			Coin();
			~Coin();

			bool UpdateObject(float dt) override;
			void DrawObject(GameSimsRenderer& r) override;

		protected:
			bool movement = false;
			bool moveRight = true;
			float timeToLive = 12.0f;
			float timeAlive;
		};
	}
}