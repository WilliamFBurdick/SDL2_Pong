#include "Game.h"
#include <stdio.h>
#include <stdexcept>

Game::Game()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		printf("SDL Initialization failed: %s\n", SDL_GetError());
		throw std::exception("SDL Initialization Error");
	}
	if (TTF_Init() < 0)
	{
		printf("SDL_ttf Initialization failed: %s\n", TTF_GetError());
		return;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		printf("SDL_mixer Initialization failed: %s\n", Mix_GetError());
		throw std::exception("SDL_mixer Initialization Error");
	}

	mWindow = SDL_CreateWindow("Pong!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (mWindow == nullptr)
	{
		printf("Failed to create SDL Window: %s\n", SDL_GetError());
		throw std::exception("SDL Error");
	}

	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (mRenderer == nullptr)
	{
		printf("Failed to create SDL Renderer: %s\n", SDL_GetError());
		throw std::exception("SDL Error");
	}

	mScoreFont = TTF_OpenFont("ARCADECLASSIC.TTF", 40);
	mBigFont = TTF_OpenFont("ARCADECLASSIC.TTF", 60);
	mSmallFont = TTF_OpenFont("ARCADECLASSIC.TTF", 30);

	mHitSound = Mix_LoadWAV("Hit.wav");
	mScoreSound = Mix_LoadWAV("Score.wav");

	leftPaddle = Paddle((SCREEN_WIDTH / 16) - (PADDLE_WIDTH / 2), SCREEN_HEIGHT / 2 - (PADDLE_HEIGHT / 2), SCREEN_WIDTH, SCREEN_HEIGHT);
	rightPaddle = Paddle(15 * SCREEN_WIDTH / 16 - (PADDLE_WIDTH / 2), SCREEN_HEIGHT / 2 - (PADDLE_HEIGHT / 2), SCREEN_WIDTH, SCREEN_HEIGHT);

	ball = Ball(SCREEN_WIDTH / 2 - (BALL_WIDTH / 2), SCREEN_HEIGHT / 2 - (BALL_HEIGHT / 2), SCREEN_WIDTH, SCREEN_HEIGHT);

	leftScore = Score(mRenderer, mScoreFont, (SCREEN_WIDTH / 4), (SCREEN_HEIGHT / 16));
	rightScore = Score(mRenderer, mScoreFont, 3 * (SCREEN_WIDTH / 4), (SCREEN_HEIGHT / 16));

	currentState = GameState::START;

	mRunning = false;
}

Game::~Game()
{
	Mix_FreeChunk(mHitSound);
	Mix_FreeChunk(mScoreSound);
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);

	Mix_Quit();
	TTF_Quit();
	SDL_Quit();
}

void Game::Run()
{
	mRunning = true;
	float lastFrame = 0.0f;
	while (mRunning)
	{
		float currentFrame = SDL_GetTicks();
		float deltaTime = (currentFrame - lastFrame) / 1000.0f;
		lastFrame = currentFrame;
		Input();
		Update(deltaTime);
		Render();
	}
}

void Game::Input()
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
		case SDL_QUIT:
			mRunning = false;
			break;
		case SDL_KEYDOWN:
			if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
			{
				mRunning = false;
			}
			if (e.key.keysym.scancode == SDL_SCANCODE_RETURN)
			{
				returnDown = true;
			}
			break;
		}
	}
	mKeyStates = SDL_GetKeyboardState(NULL);
}

void Game::Update(float deltaTime)
{
	switch (currentState)
	{
	case GameState::START:
		Update_Start(deltaTime);
		break;
	case GameState::PLAY:
		Update_Play(deltaTime);
		break;
	case GameState::SERVE:
		Update_Serve(deltaTime);
		break;
	case GameState::COMPLETE:
		Update_Complete(deltaTime);
		break;
	}

	ball.Update(deltaTime, currentState == GameState::PLAY);

	// Paddle movement is independent of game state
	if (mKeyStates[SDL_SCANCODE_UP])
	{
		rightPaddle.SetDY(-1);
	}
	else if (mKeyStates[SDL_SCANCODE_DOWN])
	{
		rightPaddle.SetDY(1);
	}
	else
	{
		rightPaddle.SetDY(0);
	}

	if (mKeyStates[SDL_SCANCODE_W])
	{
		leftPaddle.SetDY(-1);
	}
	else if (mKeyStates[SDL_SCANCODE_S])
	{
		leftPaddle.SetDY(1);
	}
	else
	{
		leftPaddle.SetDY(0);
	}

	leftPaddle.Update(deltaTime);
	rightPaddle.Update(deltaTime);
}

void Game::Render()
{
	// Clear the screen
	SDL_SetRenderDrawColor(mRenderer, 0x28, 0x2E, 0x34, 0xFF);
	SDL_RenderClear(mRenderer);

	// Draw the net
	SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	for (int y = 0; y < SCREEN_HEIGHT; ++y)
	{
		if (y % 5)
		{
			SDL_RenderDrawPoint(mRenderer, SCREEN_WIDTH / 2, y);
		}
	}

	// Draw the ball
	ball.Render(mRenderer);

	// Draw the paddles
	leftPaddle.Render(mRenderer);
	rightPaddle.Render(mRenderer);

	// Draw the scores
	leftScore.Render(mRenderer);
	rightScore.Render(mRenderer);

	// State-dependent text
	// Start state
	if (currentState == GameState::START)
	{
		RenderText(mRenderer, mSmallFont, "Press  ENTER  to  start!", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 8 + 80, true);
	}
	// Serve state
	if (currentState == GameState::SERVE)
	{
		if (leftServe)
		{
			RenderText(mRenderer, mBigFont, "PLAYER  2  scored!", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 8, true);
			RenderText(mRenderer, mSmallFont, "PLAYER  1  press  ENTER  to  serve!", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 8 + 80, true);
		}
		else
		{
			RenderText(mRenderer, mBigFont, "PLAYER  1  scored!", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 8, true);
			RenderText(mRenderer, mSmallFont, "PLAYER  2  press  ENTER  to  serve!", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 8 + 80, true);
		}
	}
	// Complete state
	if (currentState == GameState::COMPLETE)
	{
		if (leftVictory)
		{
			RenderText(mRenderer, mBigFont, "PLAYER  1  wins!", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 8, true);
			RenderText(mRenderer, mSmallFont, "Press  ENTER  to  restart!", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 8 + 80, true);
		}
		else
		{
			RenderText(mRenderer, mBigFont, "PLAYER  2  wins!", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 8, true);
			RenderText(mRenderer, mSmallFont, "Press  ENTER  to  restart!", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 8 + 80, true);
		}
	}


	SDL_RenderPresent(mRenderer);
}

void Game::Update_Start(float deltaTime)
{
	if (returnDown)
	{
		currentState = GameState::PLAY;
		returnDown = false;
		return;
	}
}

void Game::Update_Play(float deltaTime)
{
	if (ball.Collides(leftPaddle))
	{
		ball.SetDX(ball.GetDX() * -1.1);
		ball.SetX(leftPaddle.GetX() + PADDLE_WIDTH);

		float newDY = rand() % 140 + 10;
		if (ball.GetDY() < 0)
		{
			ball.SetDY(-newDY);
		}
		else
		{
			ball.SetDY(newDY);
		}
		Mix_PlayChannel(-1, mHitSound, 0);
	}
	if (ball.Collides(rightPaddle))
	{
		ball.SetDX(ball.GetDX() * -1.1);
		ball.SetX(rightPaddle.GetX() - BALL_WIDTH);

		float newDY = rand() % 140 + 10;
		if (ball.GetDY() < 0)
		{
			ball.SetDY(-newDY);
		}
		else
		{
			ball.SetDY(newDY);
		}
		Mix_PlayChannel(-1, mHitSound, 0);
	}

	if (ball.GetY() < 0)
	{
		ball.SetY(0);
		ball.SetDY(-ball.GetDY());
		Mix_PlayChannel(-1, mHitSound, 0);
	}
	if (ball.GetY() > SCREEN_HEIGHT - BALL_HEIGHT)
	{
		ball.SetY(SCREEN_HEIGHT - BALL_HEIGHT);
		ball.SetDY(-ball.GetDY());
		Mix_PlayChannel(-1, mHitSound, 0);
	}

	if (ball.GetX() + BALL_WIDTH < 0)
	{
		rightScore.score += 1;
		if (rightScore.score == 10)
		{
			currentState = GameState::COMPLETE;
			leftVictory = false;
			return;
		}
		ball.Reset();
		leftServe = true;
		currentState = GameState::SERVE;
		Mix_PlayChannel(-1, mScoreSound, 0);
	}
	if (ball.GetX() > SCREEN_WIDTH)
	{
		leftScore.score += 1;
		if (leftScore.score == 10)
		{
			currentState = GameState::COMPLETE;
			leftVictory = true;
			return;
		}
		ball.Reset();
		leftServe = false;
		currentState = GameState::SERVE;
		Mix_PlayChannel(-1, mScoreSound, 0);
	}
}

void Game::Update_Serve(float deltaTime)
{
	if (returnDown)
	{
		if (leftServe)
		{
			ball.SetDX(ball.GetSpeed() * 1);
		}
		else
		{
			ball.SetDX(ball.GetSpeed() * -1);
		}
		currentState = GameState::PLAY;
		returnDown = false;
		return;
	}
}

void Game::Update_Complete(float deltaTime)
{
	if (returnDown)
	{
		if (leftVictory)
		{
			ball.Reset(ball.GetSpeed());
		}
		else
		{
			ball.Reset(-ball.GetSpeed());
		}
		currentState = GameState::START;
		returnDown = false;
		return;
	}
}

void Game::RenderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, float x, float y, bool centerAligned)
{
	SDL_Surface* surface = TTF_RenderText_Solid(font, text, { 0xFF, 0xFF, 0xFF, 0xFF });
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

	SDL_Rect rect;
	SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	if (centerAligned)
	{
		rect.x = x - rect.w / 2;
		rect.y = y - rect.h / 2;
	}
	else
	{
		rect.x = x;
		rect.y = y;
	}
	SDL_RenderCopy(renderer, texture, NULL, &rect);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}