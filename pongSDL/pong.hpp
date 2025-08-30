#include <SDL3/SDL.h>
#include <SDL3/SDL_ttf.h>
#include <iostream>
#include <random>
#include <string>

struct Paddle {
	SDL_FRect rect;
	float speed;
	int score = 0;
};

struct Ball {
	SDL_FRect rect;
	float speed;
	float accel;
	float dx;
	float dy;
};

void updatePlayer1(Paddle &p1, float delta, const bool* state);
void updatePlayer2(Paddle &p2, float delta, const bool* state);
void updateBall(Ball& ball, Paddle& p1, Paddle& p2, float delta);
void displayScore(Paddle p1, Paddle p2, TTF_Font* font, SDL_Renderer* renderer);