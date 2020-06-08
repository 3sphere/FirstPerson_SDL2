#include <SDL.h>
#include <cstdio>
#include "Player.h"
#include <string>
#include "Constants.h"
#include "Map.h"
#include <vector>
#include <utility>
#include <algorithm>
#include "Texture.h"
#include <SDL_image.h>

SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;
Texture gBrickWallTexture;

bool init();
void close();
void handleInput();
void logic(float deltaTime);
void draw();

bool isRunning = true;

Player gPlayer;
float maxRayDistance = 16.0f;
float currentFrame = 0.0f;
float previousFrame = 0.0f;

int main(int argc, char* argv[])
{
	if (!init())
	{
		printf("Failed to initialise\n");
	}
	else
	{
		gBrickWallTexture.LoadFromFile("bricks.png", gRenderer);
		// Game loop
		while (isRunning)
		{
			currentFrame = SDL_GetTicks() / 1000.0f;
			float deltaTime = currentFrame - previousFrame;
			previousFrame = currentFrame;

			handleInput();
			logic(deltaTime);
			draw();
		}
	}

	return 0;
}

bool init()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("Failed to initialise SDL. Error: %S", SDL_GetError());
		return false;
	}
	
	gWindow = SDL_CreateWindow("First Person", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (gWindow == nullptr)
	{
		printf("Failed to create window. Error: %s", SDL_GetError());
		return false;
	}

	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
	if (gRenderer == nullptr)
	{
		printf("Failed to create renderer. Error: %s", SDL_GetError());
		return false;
	}

	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		printf("Failed to initialise SDL_image. Error: %s\n", IMG_GetError());
		return false;
	}

	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);

	return true;
}

void close()
{
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = nullptr;
	gRenderer = nullptr;

	SDL_Quit();
}

void handleInput()
{
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0)
	{
		if (e.type == SDL_QUIT)
		{
			isRunning = false;
		}
		
		gPlayer.AngularSpeed = 0.0f;
		gPlayer.ForwardSpeed = 0.0f;
		const Uint8* currentKeyStates = SDL_GetKeyboardState(nullptr);
		if (currentKeyStates[SDL_SCANCODE_A])
			gPlayer.AngularSpeed = -2.0f;
		if (currentKeyStates[SDL_SCANCODE_D])
			gPlayer.AngularSpeed = 2.0f;
		if (currentKeyStates[SDL_SCANCODE_W])
			gPlayer.ForwardSpeed = 5.0f;
		if (currentKeyStates[SDL_SCANCODE_S])
			gPlayer.ForwardSpeed = -5.0f;
	}
}

void logic(float deltaTime)
{
	gPlayer.Angle += gPlayer.AngularSpeed * deltaTime;
	gPlayer.PosX += sinf(gPlayer.Angle) * gPlayer.ForwardSpeed * deltaTime;
	gPlayer.PosY += cosf(gPlayer.Angle) * gPlayer.ForwardSpeed * deltaTime;

	if (map[(int)gPlayer.PosY * MAP_WIDTH + (int)gPlayer.PosX] == '#')
	{
		gPlayer.PosX -= sinf(gPlayer.Angle) * gPlayer.ForwardSpeed * deltaTime;
		gPlayer.PosY -= cosf(gPlayer.Angle) * gPlayer.ForwardSpeed * deltaTime;
	}
}

void draw()
{
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
	SDL_RenderClear(gRenderer);

	for (int x = 0; x < NUM_COLUMNS; x++)
	{
		float rayAngle = (gPlayer.Angle - gPlayer.Fov / 2.0f) + ((float)x / (float)NUM_COLUMNS) * gPlayer.Fov;

		float distanceToWall = 0.0f;
		bool hitWall = false;
		bool boundary = false;

		float eyeX = sinf(rayAngle);
		float eyeY = cosf(rayAngle);

		while (!hitWall && distanceToWall < maxRayDistance)
		{
			distanceToWall += 0.1f;

			int testX = (int)(gPlayer.PosX + eyeX * distanceToWall);
			int testY = (int)(gPlayer.PosY + eyeY * distanceToWall);

			if (testX < 0 || testX >= MAP_WIDTH || testY < 0 || testY >= MAP_HEIGHT)
			{
				hitWall = true;
				distanceToWall = maxRayDistance;
			}
			else
			{
				if (map[testY * MAP_WIDTH + testX] == '#')
				{
					hitWall = true;

					std::vector<std::pair<float, float>> boundaryCorners; // distance, dot

					for (int dx = 0; dx < 2; dx++)
					{
						for (int dy = 0; dy < 2; dy++)
						{
							// calculate vector pointing from player to cell corner
							float vecY = (float)testY + dy - gPlayer.PosY;
							float vecX = (float)testX + dx - gPlayer.PosX;
							float d = sqrt(vecX * vecX + vecY * vecY);
							float dot = (eyeX * vecX / d) + (eyeY * vecY / d);
							boundaryCorners.push_back(std::make_pair(d, dot));
						}
					}

					std::sort(boundaryCorners.begin(), boundaryCorners.end(),
						[](const std::pair<float, float> &left, const std::pair<float, float> &right) {return left.first < right.first; });

					float bound = 0.001f;
					if (acos(boundaryCorners.at(0).second) < bound) boundary = true;
					if (acos(boundaryCorners.at(1).second) < bound) boundary = true;
				}
			}
		}

		int ceiling = NUM_ROWS / 2.0f - NUM_ROWS / (float)distanceToWall;
		int floor = NUM_ROWS - ceiling;

		int wallShade = 255;
		
		if (distanceToWall <= maxRayDistance / 8.0f) wallShade *= 0.9f;
		else if (distanceToWall < maxRayDistance / 7.0f) wallShade *= 0.8f;
		else if (distanceToWall < maxRayDistance / 6.0f) wallShade *= 0.7f;
		else if (distanceToWall < maxRayDistance / 5.0f) wallShade *= 0.6f;
		else if (distanceToWall < maxRayDistance / 4.0f) wallShade *= 0.5f;
		else if (distanceToWall < maxRayDistance / 3.0f) wallShade *= 0.4f;
		else if (distanceToWall < maxRayDistance / 2.0f) wallShade *= 0.3f;
		else wallShade *= 0.2f;

		if (boundary) wallShade = 0;

		SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
		for (int y = 0; y < NUM_ROWS; y++)
		{
			if (y < ceiling)
			{
				SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
			}
			else if (y >= ceiling && y <= floor) 
			{
				SDL_SetRenderDrawColor(gRenderer, wallShade, wallShade, wallShade, 255);
			}
			else
			{
				int floorShade = 128;
				float floorFraction = 1.0f - ((float)y - NUM_ROWS / 2.0f) / (NUM_ROWS / 2.0f);
				if (floorFraction < 0.1) floorShade *= 0.9f;
				else if (floorFraction < 0.2) floorShade *= 0.8f;
				else if (floorFraction < 0.3) floorShade *= 0.7f;
				else if (floorFraction < 0.4) floorShade *= 0.6f;
				else if (floorFraction < 0.5) floorShade *= 0.5f;
				else if (floorFraction < 0.6) floorShade *= 0.4f;
				else if (floorFraction < 0.7) floorShade *= 0.3f;
				else if (floorFraction < 0.8) floorShade *= 0.2f;
				else if (floorFraction < 0.9) floorShade *= 0.1f;
				else floorShade = 0;

				SDL_SetRenderDrawColor(gRenderer, 0, 0, floorShade, 255);
			}

			SDL_Rect cell = { x * CELL_WIDTH_X, y * CELL_WIDTH_Y, CELL_WIDTH_X, CELL_WIDTH_Y };
			SDL_RenderFillRect(gRenderer, &cell);
		}
	}

	
	SDL_RenderPresent(gRenderer);
}