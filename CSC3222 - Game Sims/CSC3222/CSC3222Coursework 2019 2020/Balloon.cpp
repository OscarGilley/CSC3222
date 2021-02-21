#include "Balloon.h"
#include "TextureManager.h"
#include "GameSimsRenderer.h"
#include "BadFoodGame.h"
#include "GameSimsPhysics.h"

using namespace NCL;
using namespace CSC3222;

Balloon::Balloon(Vector2* anchorPoint) : SimObject() {
	this->anchorPoint = anchorPoint;
	texture = texManager->GetTexture("TL_Creatures.png");
}

Balloon::~Balloon() {

}

bool Balloon::UpdateObject(float dt) {

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE)) {
		dying = true;
	}

	if (dying) {
		timeToLive -= dt;
	}

	if (timeToLive <= 5) {
		Vector2 force = Vector2((-100 * (5 - timeToLive)), -1000 * (5 - timeToLive) * (5 - timeToLive));
		AddForce(force);
	} 

	if (timeToLive <= 0) {
		SimObject::game->physics->RemoveRigidBody(this);	
	}

	if (timeToLive > 5) {
		Vector2 force;

		float deltaX = position.x - anchorPoint->x + restLength;
		float deltaY = position.y - anchorPoint->y + restLength;

		float dist = sqrt((deltaX * deltaX) + (deltaY * deltaY));

		Vector2 normal = Vector2(deltaX, deltaY);

		normal.Normalise();

		//float cDotV = dampConst * velocity.x + dampConst * velocity.y;

		float forceMagnitude = (dist * -springConst) - dampConst * Vector2::Dot(velocity, normal);

		force = normal * forceMagnitude;

		AddForce(force);
	}

	

	return true;
}

void Balloon::DrawObject(GameSimsRenderer& r) {
	Vector2 texPos	= Vector2(73, 99);
	Vector2 texSize = Vector2(14, 28);
	r.DrawTextureArea((OGLTexture*)texture, texPos, texSize, position);
}