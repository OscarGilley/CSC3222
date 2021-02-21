#include "GameMap.h"
#include "GameSimsPhysics.h"
#include "CollisionVolume.h"
#include "GameSimsRenderer.h"
#include "TextureManager.h"
#include "../../Common/Maths.h"
#include "../../Common/Assets.h"
#include "../../Common/TextureLoader.h"
#include <fstream>
#include <iostream>
#include <cmath>

using namespace NCL;
using namespace CSC3222;
using namespace Rendering;

Vector4 buildingTypes[4] = {
	Vector4(320,16,64,80),	//tree
	Vector4(16,16,96,80), //blue building
	Vector4(128,32,64,64), //red building
	Vector4(208,32,96,64)  //green building
};

GameMap::GameMap(const std::string& filename, std::vector<SimObject*>& objects, TextureManager& texManager, GameSimsPhysics *physics)	{
	tileTexture = texManager.GetTexture("badfoodTiles.png");

	std::ifstream mapFile(Assets::DATADIR + filename);

	if (!mapFile) {
		std::cout << "GameMap can't be loaded in!" << std::endl;
		return;
	}

	mapFile >> mapWidth;
	mapFile >> mapHeight;

	mapData		= new char[mapWidth * mapHeight];
	mapCosts	= new int[mapWidth * mapHeight];

	for (int y = 0; y < mapHeight; ++y) {
		for (int x = 0; x < mapWidth; ++x) {
			int tileIndex = (y * mapWidth) + x;
			mapFile >> mapData[tileIndex];
		}
	}

	for (int y = 0; y < mapHeight; ++y) {
		for (int x = 0; x < mapWidth; ++x) {
			int tileIndex = (y * mapWidth) + x;
			char c;
			mapFile >> c;
			mapCosts[tileIndex] = c - '0';

			MapNode* node = new MapNode();
			node->bestParent = nullptr;
			node->posX = x;
			node->posY = y;
			node->g = mapCosts[tileIndex];
			node->h = 0;
			node->f = 0;
			node->closed = false;

			nodeData.push_back(node);
		}
	}

	//generating outer perimeter
	CollisionVolume* topRow = new CollisionVolume(new Vector2(240, 8), 224, 8, 0.0f, 0.2f, CollisionVolume::Type::WALL);
	CollisionVolume* leftCol = new CollisionVolume(new Vector2(8, 160), 8, 144, 0.0f, 0.2f, CollisionVolume::Type::WALL);
	CollisionVolume* botRow = new CollisionVolume(new Vector2(240, 312), 224, 8, 0.0f, 0.2f, CollisionVolume::Type::WALL);
	CollisionVolume* rightCol = new CollisionVolume(new Vector2(472, 160), 8, 144, 0.0f, 0.2f, CollisionVolume::Type::WALL);

	physics->AddCollider(topRow);
	physics->AddCollider(leftCol);
	physics->AddCollider(botRow);
	physics->AddCollider(rightCol);


	for (int y = 1; y < mapHeight - 1; ++y) {
		for (int x = 1; x < mapWidth - 1; ++x) {
			int tileIndex = (y * mapWidth) + x;
			if (mapCosts[tileIndex] == 0) {
				
				CollisionVolume* vol = new CollisionVolume(new Vector2((x * 16) + 8, (y * 16) + 8), 8, 8, 0, 0.9f, CollisionVolume::Type::WALL); //(0 * 16) + 8, (0 * 16) + 8 centrepoint = (8,8) halfX = 8, halfY = 8, inverseMass = 0
				physics->AddCollider(vol);
			}

			if (mapCosts[tileIndex] == 6) {
				
				CollisionVolume* vol = new CollisionVolume(new Vector2((x * 16) + 8, (y * 16) + 8), 8, 8, 0, 0.2f, CollisionVolume::Type::BALLOON_SHOP);
				physics->AddCollider(vol);
			}

			if (mapCosts[tileIndex] == 7) {

				CollisionVolume* vol = new CollisionVolume(new Vector2((x * 16) + 8, (y * 16) + 8), 8, 8, 0, 0.2f, CollisionVolume::Type::FOOD_SHOP);
				physics->AddCollider(vol);
			}
		}
	}

	BuildMapMesh();

	mapFile >> structureCount;

	structureData = new StructureData[structureCount];

	for (int i = 0; i < structureCount; ++i) {
		int type  = 0;
		int xTile = 0;
		int yTile = 0;

		mapFile >> type;
		mapFile >> xTile;
		mapFile >> yTile;

		structureData[i].structureType = (MapStructureType)type;
		structureData[i].startPos.x = xTile * 16.0f;
		structureData[i].startPos.y = yTile * 16.0f;
	}
}

GameMap::~GameMap()	{
	delete[] mapData;
	delete[] mapCosts;
	delete[] mapTexCoords;

	delete mapMesh;
}

float GameMap::getHeuristic(MapNode startPos, MapNode goalPos) {
	float test = abs(startPos.posX - goalPos.posX) + abs(startPos.posY - goalPos.posY);
	return test;
}

std::vector<Vector2> GameMap::InitialiseNode(Vector2 startPos, Vector2 goalPos) {
	Vector2 startTile = WorldPosToTilePos(startPos);
	Vector2 goalTile = WorldPosToTilePos(goalPos);
	MapNode* startNode = new MapNode();

	startNode->bestParent = nullptr;
	startNode->posX = startTile.x;
	startNode->posY = startTile.y;
	startNode->g = nodeData[(startTile.y * mapWidth) + startTile.x]->g;
	startNode->h = getHeuristic(*nodeData[(startTile.y * mapWidth) + startTile.x], *nodeData[(goalTile.y * mapWidth) + goalTile.x]);
	startNode->f = startNode->g + startNode->h;
	startNode->closed = false;

	std::vector<MapNode*> openList;

	MapNode* goalNode = new MapNode();

	goalNode->bestParent = nullptr;
	goalNode->posX = goalTile.x;
	goalNode->posY = goalTile.y;
	goalNode->g = nodeData[(goalTile.y * mapWidth) + goalTile.x]->g;
	goalNode->h = 0;
	goalNode->f = goalNode->g + goalNode->h;
	goalNode->closed = false;

	openList.push_back(startNode);

	return AStarSearchLoop(openList, startNode, goalNode, nodeData);
	
}

MapNode* GameMap::GetBestF(std::vector <MapNode*> openList) {

	float bestF = openList.at(0)->g + openList.at(0)->h;
	int index = 0;

	for (int i = 0; i < openList.size(); i++) {

		float iF = openList.at(i)->g + openList.at(i)->h;

		if (iF < bestF) {

			//std::cout << "\nBestF = " << bestF << "\niF = " << iF;
			bestF = iF;
			index = i;
		}

	}

	return openList.at(index);

}

std::vector<Vector2> GameMap::AStarSearchLoop(std::vector<MapNode*> openList, MapNode* startNode, MapNode* goalNode, std::vector<MapNode*> &dataNode) {

	std::vector<MapNode*> closedList;
	std::vector<Vector2> path;

	while (openList.size() > 0) {

		MapNode* bestNode = GetBestF(openList);

		//std::cout << "\n" << openList.size();
		//std::cout << "\nGoal Node: " << Vector2(goalNode->posX, goalNode->posY);
		//std::cout << "\nBest Node: " << Vector2(bestNode->posX, bestNode->posY);
		
		if (Vector2(bestNode->posX, bestNode->posY) == Vector2(goalNode->posX, goalNode->posY)) {
			return GeneratePath(startNode, bestNode, path);
		}

		else {
			std::vector<MapNode*> activeNeighbours;

			for (int y = bestNode->posY - 1; y <= bestNode->posY + 1; ++y) {
				for (int x = bestNode->posX - 1; x <= bestNode->posX + 1; ++x) {

					if (x == bestNode->posX && y == bestNode->posY) {
						continue;
					}

					else if (x < mapWidth - 2 && y < mapHeight - 2) {

						int tileType = mapCosts[((y) * mapWidth) + x];

						if (tileType == 2 || tileType == 5) {

							MapNode* neighbour = dataNode[( (y)*mapWidth + x)];
							
							

							if (neighbour->bestParent == nullptr) {
								neighbour->bestParent = bestNode;
							}

							activeNeighbours.emplace_back(neighbour);
						}
					}
				}
			}

			if (std::find(closedList.begin(), closedList.end(), bestNode) == closedList.end()) {
				closedList.emplace_back(bestNode);
			}
			openList.erase(std::remove(openList.begin(), openList.end(), bestNode), openList.end());
			

			//i think Q is neighbour
			for (auto neighbour : activeNeighbours) {

				//if neighbour is not in closed list

				if (std::find(closedList.begin(), closedList.end(), neighbour) == closedList.end()) {
					float g = neighbour->bestParent->g + neighbour->g; //g = QParentg + cost from P to Q
					float h = getHeuristic(*neighbour, *goalNode);
					float f = g + h;

					bool added = false;
					if (std::find(openList.begin(), openList.end(), neighbour) == openList.end()) { //checks if neighbour IS NOT in openlist
						openList.emplace_back(neighbour);
						added = true;
					}

					/*
					if (std::find(openList.begin(), openList.end(), neighbour) != openList.end()) { //checks if neighbour IS in openlist
						continue;
					}
					*/

					if (f > neighbour->f && !added) {
						continue;
					}

					else {
						neighbour->g = g;
						neighbour->h = h;
						neighbour->f = f;
						neighbour->bestParent = bestNode;
					}


				}


			}

			
			
			//std::cout << "added to closed list";
		}
	}
}


std::vector<Vector2> GameMap::GeneratePath(MapNode* from, MapNode* to, std::vector<Vector2> path) const {
	MapNode* R = to;

	
	while (Vector2(R->posX, R->posY) != Vector2(from->posX, from->posY)) {
		path.emplace_back(TilePosToWorldPos(Vector2((R->posX), (R->posY))));
		R = R->bestParent;
	}

	return path;
}


Vector2 GameMap::GetRandomPosition() const {
	bool notChosen = true;

	while (notChosen) {

		int x = rand() % GetMapWidth();
		int y = rand() % GetMapHeight();
		int tileIndex = (y * GetMapWidth() + x);

		if (mapCosts[tileIndex] == 2 || mapCosts[tileIndex] == 5) {
			notChosen = false;
			return Vector2(x * 16 + 8, y * 16 + 8);
		}

	}
}

Vector2 GameMap::GetRandomPositionWithinRadius(Vector2 pos, float r) const {
	bool notChosen = true;

	while (notChosen) {



		float angle = ((float)rand() / RAND_MAX) * _CMATH_::PI * 2;

		float x = _CMATH_::cos(angle) * r;
		float y = _CMATH_::sin(angle) * r;

		int tileIndex = ((int)(y / 16) * GetMapWidth() + (int)(x / 16));

		if (mapCosts[tileIndex] == 2 || mapCosts[tileIndex] == 5) {
			if ((x + pos.x) < 480 && (y + pos.y) < 320) {
				notChosen = false;
				return Vector2(pos.x + x, pos.y + y);
			}
		}
	}
}

Vector2 GameMap::WorldPosToTilePos(Vector2 worldPos) const {
	return Vector2((int) worldPos.x / 16, (int) worldPos.y / 16);
}

Vector2 GameMap::TilePosToWorldPos(Vector2 tilePos) const {
	return Vector2((tilePos.x * 16) + 8, (tilePos.y * 16) + 8);
}

void GameMap::DrawMap(GameSimsRenderer & r) {
	r.DrawMesh((OGLMesh*)mapMesh, (OGLTexture*)tileTexture);
	//structures then go on top
	for (int i = 0; i < structureCount; ++i) {
		Vector4 buildingData = buildingTypes[structureData[i].structureType];

		Vector2 texPos  = Vector2(buildingData.x, buildingData.y);
		Vector2 texSize = Vector2(buildingData.z, buildingData.w);

		Vector2 screenPos = structureData[i].startPos;

		r.DrawTextureArea((OGLTexture*)tileTexture, texPos, texSize, screenPos, false);
	}
}

void GameMap::BuildMapMesh() {
	vector<Vector2> texCoords;
	vector<Vector3> positions;

	Vector2 flatGrassTile = Vector2(288, 144);
	Vector2 primaryTile;
	Vector2 secondaryTile = flatGrassTile; //some sit on top of another tile type, usually grass!

	for (int y = 0; y < mapHeight; ++y) {
		for (int x = 0; x < mapWidth; ++x) {
			int tileType = mapData[(y * mapWidth) + x];
			bool doSecondary = false;

			switch (tileType) {
				//the normal tile types first
				case 'A':primaryTile = Vector2(272,112);break; //top left grass
				case 'B':primaryTile = Vector2(288, 112); break;//top grass
				case 'C':primaryTile = Vector2(320, 112); break;//top right grass
				case 'D':primaryTile = Vector2(272, 128); break;//left grass
				case 'E':primaryTile = Vector2(320, 128); break;//right grass
				case 'F':primaryTile = Vector2(272, 160); break;//bottom left
				case 'G':primaryTile = Vector2(320, 160); break;//bottom right
				case 'H':primaryTile = Vector2(288, 160); break;//bottom grass

				case 'I':primaryTile = Vector2(336, 112); break; //top left grass
				case 'J':primaryTile = Vector2(352, 112); break; //top right grass
				case 'K':primaryTile = Vector2(336, 128); break; //bottom left grass
				case 'L':primaryTile = Vector2(352, 128); break; //bottom right grass

				case 'M':primaryTile = Vector2(208, 112); doSecondary = true;break; //top left fence
				case 'N':primaryTile = Vector2(224, 112); doSecondary = true;break; //top fence
				case 'O':primaryTile = Vector2(240, 112); doSecondary = true;break; //top right fence
				case 'P':primaryTile = Vector2(208, 128); doSecondary = true;break; //left fence
				case 'Q':primaryTile = Vector2(208, 144); doSecondary = true;break; //bottom left fence
				case 'R':primaryTile = Vector2(240, 144); doSecondary = true;break; //bottom right fence
//Stone wall bits
				case 'a':primaryTile = Vector2(16, 112); doSecondary = true; break; //top left grass
				case 'b':primaryTile = Vector2(32, 112); break;//Horizontal A
				case 'c':primaryTile = Vector2(32, 128); break;//Horizontal B
				case 'd':primaryTile = Vector2(144, 112); doSecondary = true; break;//top right grass
				case 'e':primaryTile = Vector2(144, 128); break;//Side
				case 'f':primaryTile = Vector2(16, 160); break;//bottom left A
				case 'g':primaryTile = Vector2(16, 176); doSecondary = true; break;//bottom left B
	
				case 'h':primaryTile = Vector2(64, 160); doSecondary = true; break;//right hand corner A
				case 'i':primaryTile = Vector2(64, 176); doSecondary = true; break;//right hand corner B

				case 'j':primaryTile = Vector2(96, 160); doSecondary = true; break;//Left hand corner A
				case 'k':primaryTile = Vector2(96, 176); doSecondary = true; break;//Left hand corner B

				case 'l':primaryTile = Vector2(144, 160); break;//Bottom right A
				case 'm':primaryTile = Vector2(144, 176); doSecondary = true;  break;//Bottom right B
				case 'n':primaryTile = Vector2(32, 176); doSecondary = true; break;//bottom horizontal

				case 'o':primaryTile = Vector2(80, 176); doSecondary = true; break;//Stone to grass transition
				case 'p':primaryTile = Vector2(80, 176); doSecondary = true; break;//stone to road transition

				case '1':primaryTile = Vector2(352, 144); break; //Road Tile
				case '2':primaryTile = Vector2(64, 144);  break;//Stone Tile
				case '3':primaryTile = Vector2(16, 192); doSecondary  = true; break; //Flower A
				case '4':primaryTile = Vector2(32, 192); doSecondary  = true; break;//Flower B
				case '5':primaryTile = Vector2(48, 192); doSecondary  = true; break;//Flower C
				case '6':primaryTile = Vector2(64, 192); doSecondary  = true; break;//Flower D
				case '7':primaryTile = Vector2(176, 176); doSecondary = true; break;//Sign A
				case '8':primaryTile = Vector2(192, 176); doSecondary = true; break;//Sign B
				case '9':primaryTile = Vector2(208, 176); doSecondary = true; break;//Sign C	
				case '0': {
					static Vector2 grassTiles[] = {
						Vector2(288,128),	//	
						Vector2(304,144),	//
						Vector2(304,128),	//
						Vector2(288,144),
					};
					primaryTile = grassTiles[Maths::Clamp(rand() % 20, 0, 3)];
				}break;//Random grass tiles
			}			
			if (doSecondary) {
				AddNewTilePos(x, y, positions);
				AddNewTileTex((int)secondaryTile.x, (int)secondaryTile.y, texCoords);
				secondaryTile = flatGrassTile;
			}
			AddNewTilePos(x, y, positions);
			AddNewTileTex((int)primaryTile.x, (int)primaryTile.y, texCoords);
		}
	}
	mapMesh = new OGLMesh();
	mapMesh->SetVertexPositions(positions);
	mapMesh->SetVertexTextureCoords(texCoords);
	mapMesh->SetPrimitiveType(NCL::GeometryPrimitive::Triangles);
	mapMesh->UploadToGPU();
}

void GameMap::AddNewTilePos(int x, int y, std::vector<Vector3>& pos) {
	Vector3 topLeft		= Vector3((x + 0) * 16.0f, (y + 0) * 16.0f, 0);
	Vector3 topRight	= Vector3((x + 1) * 16.0f, (y + 0) * 16.0f, 0);
	Vector3 bottomLeft	= Vector3((x + 0) * 16.0f, (y + 1) * 16.0f, 0);
	Vector3 bottomRight = Vector3((x + 1) * 16.0f, (y + 1) * 16.0f, 0);

	pos.emplace_back(topLeft);
	pos.emplace_back(bottomLeft);
	pos.emplace_back(topRight);

	pos.emplace_back(topRight);
	pos.emplace_back(bottomLeft);
	pos.emplace_back(bottomRight);
}

void GameMap::AddNewTileTex(int x, int y, std::vector<Vector2>& tex) {
	Vector2 topLeft		= Vector2((x + 0.0f) , (y + 0.0f));
	Vector2 topRight	= Vector2((x + 16.0f), (y + 0.0f));
	Vector2 bottomLeft	= Vector2((x + 0.0f) , (y + 16.0f));
	Vector2 bottomRight = Vector2((x + 16.0f), (y + 16.0f));

	tex.emplace_back(topLeft);
	tex.emplace_back(bottomLeft); 
	tex.emplace_back(topRight);

	tex.emplace_back(topRight);
	tex.emplace_back(bottomLeft);
	tex.emplace_back(bottomRight);
}