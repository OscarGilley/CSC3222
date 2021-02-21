#pragma once
#include <string>
#include <vector>
#include "GameSimsPhysics.h"
#include "CollisionVolume.h"
#include "../../Common/Vector2.h"
#include "../../Common/Vector3.h"
#include "../../Common/Vector4.h"
#include "../../Common/MeshGeometry.h"


namespace NCL {
	namespace Rendering {
		class TextureBase;
	}
	using namespace Maths;
	namespace CSC3222 {
		class GameSimsRenderer;
		class SimObject;
		class TextureManager;

		enum MapStructureType {
			Tree,
			BlueBuilding,
			RedBuilding,
			GreenBuilding
		};

		struct StructureData {
			Vector2				startPos;
			MapStructureType	structureType;
		};

		struct MapNode {
			MapNode* bestParent;
			int posX;
			int posY;
			float g;
			float h;
			float f;
			bool closed;
		};

		class GameMap	{
		public:
			GameMap(const std::string& filename, std::vector<SimObject*>& objects, TextureManager& texManager, GameSimsPhysics *physics);
			~GameMap();

			
			
			Vector2 GetRandomPosition() const;
			Vector2 GetRandomPositionWithinRadius(Vector2 pos, float r) const;

			Vector2 WorldPosToTilePos(Vector2 worldPos) const;
			Vector2 TilePosToWorldPos(Vector2 tilePos) const;

			std::vector<Vector2> InitialiseNode(Vector2 startPos, Vector2 goalPos);
			std::vector<Vector2> AStarSearchLoop(std::vector<MapNode*> openList, MapNode* startPos, MapNode* goalNode, std::vector<MapNode*> &nodeData);
			std::vector<Vector2> GeneratePath(MapNode* from, MapNode* to, std::vector<Vector2> path) const;

			float getHeuristic(MapNode startPos, MapNode goalPos);
			MapNode* GetBestF(std::vector <MapNode*> openList);

			void DrawMap(GameSimsRenderer & r);
			int* mapCosts;

			int GetMapWidth() const {
				return mapWidth;
			}

			int GetMapHeight() const {
				return mapHeight;
			}

			std::vector<MapNode*> nodeData;

		protected:
			void BuildMapMesh();
			void AddNewTilePos(int x, int y, std::vector<Vector3>& pos);
			void AddNewTileTex(int x, int y, std::vector<Vector2>& tex);

			int mapWidth;
			int mapHeight;
			int structureCount;

			Rendering::TextureBase*	tileTexture;
			char*	mapData;
			
			Vector2*		mapTexCoords;
			StructureData*	structureData;		

			MeshGeometry* mapMesh;
		};
	}
}

