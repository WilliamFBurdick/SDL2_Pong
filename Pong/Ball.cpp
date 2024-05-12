#include "Ball.h"
#include <cstdlib>

Ball::Ball()
{
	mX = mY = 0;
	mDX = mDY = 0;
	mRect = SDL_Rect();
	mRect.x = mX;
	mRect.y = mY;
	mRect.w = BALL_WIDTH;
	mRect.h = BALL_HEIGHT;
}

Ball::Ball(float x, float y, int screenWidth, int screenHeight)
	:screenWidth(screenWidth), screenHeight(screenHeight)
{
	mX = x;
	mY = y;
	mDX = mDY = 0;
	mRect = SDL_Rect();
	mRect.x = mX;
	mRect.y = mY;
	mRect.w = BALL_WIDTH;
	mRect.h = BALL_HEIGHT;

	mDX = (rand() % 2 == 0) ? BALL_SPEED : -BALL_SPEED;
	mDY = (rand() % 100 - 50) * 1.5f;
}

void Ball::Reset(float xDir)
{
	mX = screenWidth / 2 - BALL_WIDTH / 2;
	mY = screenHeight / 2 - BALL_HEIGHT / 2;

	mDX = (rand() % 2 == 0) ? BALL_SPEED : -BALL_SPEED;
	mDY = (rand() % 100 - 50) * 1.5f;
}

void Ball::Render(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	mRect.x = mX;
	mRect.y = mY;
	SDL_RenderFillRect(renderer, &mRect);
}

void Ball::Update(float deltaTime, bool playState)
{
	if (playState)
	{
		mX += mDX * deltaTime;
		mY += mDY * deltaTime;
	}
}

bool Ball::Collides(Paddle other)
{
	SDL_Rect otherRect = other.GetRect();
	if (mX > otherRect.x + otherRect.w || otherRect.x > mX + mRect.w)
		return false;
	if (mY > otherRect.y + otherRect.h || otherRect.y > mY + mRect.h)
		return false;
	return true;
}