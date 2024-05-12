#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <string>

#include "Ball.h"
#include "Paddle.h"

const int SCREEN_WIDTH = 1280, SCREEN_HEIGHT = 720;

enum GameState
{
	START, PLAY, SERVE, COMPLETE
};

struct Score
{
	Score()
	{
		x = 0;
		y = 0;
		score = 0;
	}
	Score(SDL_Renderer* renderer, TTF_Font* font, float x, float y)
	{
		this->x = x;
		this->y = y;
		this->font = font;
		score = 0;

		SDL_Surface* surface = TTF_RenderText_Solid(this->font, "0", {0xFF, 0xFF, 0xFF, 0xFF});
		texture = SDL_CreateTextureFromSurface(renderer, surface);
	}
	void Render(SDL_Renderer* renderer)
	{
		SDL_Surface* surface = TTF_RenderText_Solid(this->font, std::to_string(score).c_str(), { 0xFF, 0xFF, 0xFF, 0xFF });
		texture = SDL_CreateTextureFromSurface(renderer, surface);

		SDL_Rect rect;
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
		rect.x = x;
		rect.y = y;
		SDL_RenderCopy(renderer, texture, NULL, &rect);
	}
	float x, y;
	TTF_Font* font;
	SDL_Texture* texture;
	int score;
};

class Game
{
public:
	Game();
	~Game();
	void Run();
	GameState GetCurrentState() { return currentState; }
private:
	void Input();
	void Update(float deltaTime);
	void Render();

	void Update_Play(float deltaTime);
	void Update_Start(float deltaTime);
	void Update_Serve(float deltaTime);
	void Update_Complete(float deltaTime);

	void RenderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, float x, float y, bool centerAligned);
private:
	SDL_Renderer* mRenderer;
	SDL_Window* mWindow;
	TTF_Font* mScoreFont;
	TTF_Font* mBigFont;
	TTF_Font* mSmallFont;
	Mix_Chunk* mHitSound;
	Mix_Chunk* mScoreSound;
	const Uint8* mKeyStates;

	bool mRunning;

	Paddle leftPaddle, rightPaddle;
	Ball ball;
	Score leftScore, rightScore;

	GameState currentState;

	bool returnDown;
	bool leftServe;
	bool leftVictory;
};

