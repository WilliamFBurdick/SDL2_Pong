#pragma once

#include <SDL.h>

const int PADDLE_WIDTH = 10, PADDLE_HEIGHT = 120;
const float PADDLE_SPEED = 300.0f;

class Paddle
{
public:
	Paddle();
	Paddle(float x, float y, int screenWidth, int screenHeight);
	~Paddle();

	void Update(float deltaTime);
	void Render(SDL_Renderer* renderer);
	float GetX() { return mX; }
	void SetDY(float dy);
	const SDL_Rect GetRect() { return mRect; }
private:
	float mX, mY;
	float mDY;

	int screenWidth, screenHeight;

	SDL_Rect mRect;
};

