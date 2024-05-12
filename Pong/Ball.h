#pragma once

#include <SDL.h>
#include "Paddle.h"

const int BALL_WIDTH = 15, BALL_HEIGHT = 15;
const float BALL_SPEED = 100;

class Ball
{
public:
	Ball();
	Ball(float x, float y, int screenWidth, int screenHeight);
	void Reset(float xDir = 0);
	void Render(SDL_Renderer* renderer);
	void Update(float deltaTime, bool playState);
	bool Collides(Paddle other);

	void SetX(float x) { mX = x; }
	void SetY(float y) { mY = y; }
	void SetDX(float dx) { mDX = dx; }
	void SetDY(float dy) { mDY = dy; }
	float GetX() { return mX; }
	float GetY() { return mY; }
	float GetDX() { return mDX; }
	float GetDY() { return mDY; }

	float GetSpeed() { return BALL_SPEED; }
private:
	SDL_Rect mRect;
	float mX, mY;
	float mDX, mDY;

	int screenWidth, screenHeight;
};

