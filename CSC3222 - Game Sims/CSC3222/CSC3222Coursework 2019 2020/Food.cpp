#include "Food.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "BadFoodGame.h"
#include "GameSimsPhysics.h"

using namespace NCL;
using namespace CSC3222;

Vector4 foodFrames[] = {
	Vector4(0,0,15,16), //Apple
	Vector4(17,0,14,16),//Can
	Vector4(31,0,18,16),//Pizza
	Vector4(49,0,15,16),//Sundae
	Vector4(64,0,16,16) //Bread
};

Food::Food() : SimObject() {
	texture = texManager->GetTexture("food_items16x16.png");

	foodItem = rand() % 5;
}

Food::~Food() {

}

bool Food::UpdateObject(float dt) {
	
	timeAlive += dt;

	if (timeAlive >= timeToLive) {
		SimObject::game->physics->RemoveCollider(GetCollider());
		SimObject::game->physics->RemoveRigidBody(this);
		SimObject::game->allFood.erase(std::remove(game->allFood.begin(), game->allFood.end(), this), game->allFood.end());
		return false;
		return false;
	}

	if (collided == true) {
		SimObject::game->physics->RemoveCollider(GetCollider());
		SimObject::game->physics->RemoveRigidBody(this);
		SimObject::game->allFood.erase(std::remove(game->allFood.begin(), game->allFood.end(), this), game->allFood.end());
		return false;
	}

	
	return true;
}

void Food::DrawObject(GameSimsRenderer& r) {
	Vector2 texPos	= Vector2(foodFrames[currentanimFrame].x, foodFrames[currentanimFrame].y);
	Vector2 texSize = Vector2(foodFrames[currentanimFrame].z, foodFrames[currentanimFrame].w);
	r.DrawTextureArea((OGLTexture*)texture, texPos, texSize, position);
}