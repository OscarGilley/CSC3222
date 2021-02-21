#pragma once
#include "SimObject.h"

namespace NCL {
	namespace CSC3222 {
		class Balloon : public SimObject
		{
		public:
			Balloon(Vector2*);
			~Balloon();

			bool UpdateObject(float dt) override;
			void DrawObject(GameSimsRenderer& r) override;

		protected:
			Vector2* anchorPoint;
			float dampConst = 0.8;
			float springConst = 50;
			float restLength = 15;
			float timeToLive = 10;
			bool dying = false;
		};
	}
}

