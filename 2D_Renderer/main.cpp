#include "Canvas.h"
#include "Image.h"
#include "SDL.h"
#include <iostream>

int main(int, char**)
{

	const unsigned int width = 800;
	const unsigned int height = 600;
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		std::cout << "SDL Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	SDL_Window* window = SDL_CreateWindow("Poggers", 100, 100, width, height, SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		std::cout << "SDL Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}


	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr)
	{
		std::cout << "SDL Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	Canvas canvas(renderer, width, height);
	Image image;
	image.Load("cat.bmp");

	std::vector<SDL_Point> points;
	for (int i = 0; i < 10; i++)
	{
		points.push_back({ (rand() % width, rand() % height),  (rand() % width, rand() % height) });
	}

	bool quit = false;
	while (!quit)
	{
		SDL_Event event;
		SDL_PollEvent(&event);
		switch (event.type)
		{
		case SDL_QUIT:
			quit = true;
			break;
		}
		canvas.Clear({ 0, 0, 70, 255 });
		
		
		SDL_Color color = { rand() % 255, rand() % 255, rand() % 255,255};
		SDL_Color tricolor = {255, rand() % 255, rand() % 255,255};
		

		SDL_Rect rect;
		
		rect.x = 40;
		rect.y = 50;
		rect.w = 70;
		rect.h = 80;

		canvas.DrawLine(points[1], points[2], color);
		canvas.DrawLineList(points, color);
		canvas.DrawPoint(points[0], color);
		canvas.DrawTriangle(points[1], points[2], points[3], tricolor);
		canvas.DrawTriangle({ (rand() % width, rand() % height),  (rand() % width, rand() % height) }, { (rand() % width, rand() % height),  (rand() % width, rand() % height) }, { (rand() % width, rand() % height),  (rand() % width, rand() % height) }, tricolor);
		canvas.DrawRectangle(rect, color);
		canvas.DrawCircle(points[4], 100, tricolor);

		canvas.DrawImage(&image, { 200, 200 });
		//canvas.ProcessInvert();
		//canvas.ProcessMono();
		//canvas.ProcessNight();
		//const SDL_Color threshold = {150, 150, 150};
		//canvas.ProcessThreshhold(threshold);
		//int brightness = 100;
		//canvas.ProcessBrightness(brightness);
		//int r = 75, g = 150, b = 225;
		//canvas.ProcessColor(r, g, b);
		//canvas.ProcessRBSwap();
		canvas.Update();

		SDL_RenderClear(renderer);
		canvas.Draw(renderer);
		SDL_RenderPresent(renderer);
	}
	
	


	SDL_Quit();

	return 0;
}
