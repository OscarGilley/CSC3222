#include "PlayerCharacter.h"
#include "TextureManager.h"
#include "../../Common/Window.h"
#include "GameSimsRenderer.h"

using namespace NCL;
using namespace CSC3222;

int  charARunY		= 128;
int  charARunX[]	= { 32,64,96,128,160,192,224,256 };
bool charAFlips[]	= { false, true, true, false, false };

Vector2 charAIdle(64,64);
Vector2 charBIdle(64,64);

int  charBRunY		= 96;
int  charBRunX[]	= { 128,160,192,224,256,288 };
bool charBFlips[]	= { false, true, true, false, false };

PlayerCharacter::PlayerCharacter() : SimObject() {
	currentAnimDir		= MovementDir::Left;
	SetCharacterType(CharacterType::TYPE_A);
}

PlayerCharacter::~PlayerCharacter() {

}

bool PlayerCharacter::UpdateObject(float dt) {
	float testSpeed = 400;

	Vector2 newForce;

	currentAnimDir = MovementDir::Idle;

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
		currentAnimDir = MovementDir::Up;
		UpdateAnimFrame(dt);

		newForce.y = -testSpeed;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
		currentAnimDir = MovementDir::Down;
		UpdateAnimFrame(dt);

		newForce.y =  testSpeed;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
		currentAnimDir = MovementDir::Left;
		UpdateAnimFrame(dt);

		newForce.x = -testSpeed;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
		currentAnimDir = MovementDir::Right;
		UpdateAnimFrame(dt);

		newForce.x = testSpeed;
	}

	AddForce(newForce);

	//position += force * dt;

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE)) {

	}

	//AddForce(-newForce);

	return true;
	
	

	
}

void PlayerCharacter::DrawObject(GameSimsRenderer& r) {
	switch (charType) {
		case CharacterType::TYPE_A:DrawCharacterA(r); break;
		case CharacterType::TYPE_B:DrawCharacterB(r); break;
	}
}

void PlayerCharacter::DrawCharacterA(GameSimsRenderer& r) {
	Vector2 screenPos	= position;
	Vector2 texSize		= Vector2(32, 32);

	Vector2	texPos;
	texPos.x = (float)charARunX[currentanimFrame];
	texPos.y = (float)charARunY;

	if (currentAnimDir == MovementDir::Idle) {
		texPos = charAIdle;
	}
	r.DrawTextureArea((OGLTexture*)texture, texPos, texSize, screenPos, charAFlips[(int)currentAnimDir]);
}

void PlayerCharacter::DrawCharacterB(GameSimsRenderer& r) {
	Vector2 screenPos	= position;
	Vector2 texSize		= Vector2(32, 32);

	Vector2	texPos;
	texPos.x = (float)charBRunX[currentanimFrame];
	texPos.y = (float)charBRunY;

	if (currentAnimDir == MovementDir::Idle) {
		texPos = charBIdle;
	}

	r.DrawTextureArea((OGLTexture*)texture, texPos, texSize, screenPos, charBFlips[(int)currentAnimDir]);
}

void PlayerCharacter::SetCharacterType(CharacterType t) {
	charType = t;
	switch (charType) {
		case CharacterType::TYPE_A:
			texture = texManager->GetTexture("TL_oldcharacters.png"); 
			animFrameCount = 8;
			break;
		case CharacterType::TYPE_B:
			texture = texManager->GetTexture("Twiggy_spritesheet.png"); 
			animFrameCount = 6;
			break;
	}
}