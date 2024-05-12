#include "Paddle.h"

Paddle::Paddle()
{
	mX = mY = 0;
	mDY = 0;
	screenWidth = screenHeight = 0;

	mRect = SDL_Rect();
	mRect.w = PADDLE_WIDTH;
	mRect.h = PADDLE_HEIGHT;
	mRect.x = mX;
	mRect.y = mY;
}

Paddle::Paddle(float x, float y, int screenWidth, int screenHeight)
	:mX(x), mY(y), screenWidth(screenWidth), screenHeight(screenHeight)
{
	mDY = 0;
	mRect = SDL_Rect();
	mRect.w = PADDLE_WIDTH;
	mRect.h = PADDLE_HEIGHT;
	mRect.x = mX;
	mRect.y = mY;
}

Paddle::~Paddle()
{}

void Paddle::Update(float deltaTime)
{
	mY += mDY * deltaTime;
	if (mY >= screenHeight - PADDLE_HEIGHT)
		mY = screenHeight - PADDLE_HEIGHT;
	if (mY <= 0)
		mY = 0;
}

void Paddle::Render(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	mRect.x = mX;
	mRect.y = mY;
	SDL_RenderFillRect(renderer, &mRect);
}

void Paddle::SetDY(float dy)
{
	mDY = dy * PADDLE_SPEED;
}