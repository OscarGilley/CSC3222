#include "Coin.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "CollisionVolume.h"
#include "GameSimsPhysics.h"
#include "BadFoodGame.h"

using namespace NCL;
using namespace CSC3222;

Vector2 animFrames[] = {
	Vector2(96 , 80),
	Vector2(112, 80),
	Vector2(128, 80),
	Vector2(144, 80),
};

Coin::Coin() : SimObject() {
	animFrameCount = 4;
	texture = texManager->GetTexture("Items.png");
}

Coin::~Coin() {

}

bool Coin::UpdateObject(float dt) {
	UpdateAnimFrame(dt);

	timeAlive += dt;

	if (timeAlive >= timeToLive) {
		SimObject::game->physics->RemoveCollider(GetCollider());
		SimObject::game->physics->RemoveRigidBody(this);
		SimObject::game->allCoins.erase(std::remove(game->allCoins.begin(), game->allCoins.end(), this), game->allCoins.end());

		return false;
	}
	

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F3)) {
		movement = true;
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F4)) {
		movement = false;
	}

	if (movement) {

		if (moveRight) {
			force = Vector2(1000, 0);
		}

		if (!moveRight) {
			force = Vector2(-1000, 0);
		}

		if (position.x > 300) {
			moveRight = false;
		}

		if (position.x < 200) {
			moveRight = true;
		}
	}

	


	
	//AddForce(force);

	if (collided == true) {
		SimObject::game->physics->RemoveCollider(GetCollider());
		SimObject::game->physics->RemoveRigidBody(this);
		SimObject::game->allCoins.erase(std::remove(game->allCoins.begin(), game->allCoins.end(), this), game->allCoins.end());
		return false;
	}

	return true;
}

void Coin::DrawObject(GameSimsRenderer& r) {
	Vector2 texPos	 = animFrames[currentanimFrame];
	Vector2 texSize	 = Vector2(16,16);
	r.DrawTextureArea((OGLTexture*)texture, texPos, texSize, position);
}