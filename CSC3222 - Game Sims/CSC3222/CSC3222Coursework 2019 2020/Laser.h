#pragma once
#include "SimObject.h"

namespace NCL {
	namespace Maths {
		class Vector2;
	}
	namespace CSC3222 {
		class Laser : public SimObject {
		public:
			Laser(Vector2 direction);
			~Laser();

			void DrawObject(GameSimsRenderer &r) override;
			bool UpdateObject(float dt) override;
			int noBounces = 4;
			void LaserBounced();
			float activeIn;

		protected:
			Vector2 normal;
			
			
			
		};
	}
}

