#pragma once
#include "SimObject.h"
#include <vector>

namespace NCL {
	namespace CSC3222 {
		class BadRobot : public SimObject
		{
		public:
			BadRobot();
			~BadRobot();

			bool UpdateObject(float dt) override;
			void DrawObject(GameSimsRenderer& r) override;
			void ProtectShop();
			void AttackPlayer(float dt);
			void PatrolMap();

			Vector3 Allignment(std::vector<BadRobot*> allRobots);
			Vector3 Separation(std::vector<BadRobot*> allRobots);
			Vector3 Cohesion(std::vector<BadRobot*> allRobots);
			Vector3 Avoidance(std::vector<BadRobot*> allRobots);

			Vector3 Flocking(std::vector<BadRobot*> allRobots);
			float stunnedTimer = 0;

		protected:
			enum class RobotType {
				Green,
				White,
			};
			RobotType type;
			enum class State {
				PATROL,
				PROTECT,
				ATTACK
			};

			Vector2 targetPos;
			State state;
			static std::vector<BadRobot*> allRobots;
			float shootTimer = 0;
			float alignmentThreshold = 10;
			float separationThreshold = 25;
			float cohesionThreshold = 5;
			float avoidanceThreshold = 10;
			float pathTimer = 0;
			bool pathFound = false;
			std::vector<Vector2> path;
			

		};
	}
}

