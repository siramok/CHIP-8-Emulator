#define SDL_MAIN_HANDLED
#include <iostream>
#include <filesystem>
#include <bitset>
#include <SDL.h>
#include "CHIP8.h"
namespace fs = std::filesystem;

void initializeKeys()
{

}

int main(int argc, char **argv[])
{
	fs::path dir = fs::current_path();
	int choice;

	//A basic console menu that lets the user choose a rom to load
	std::cout << "CHOOSE A GAME TO LOAD" << std::endl;
	std::cout << "Default rom path: " << dir << "\\" << "\\roms" << std::endl;
	std::cout << "\n1. PONG" << std::endl;
	std::cin >> choice;

	chip8 cpu;
	switch (choice)
	{
		case 1:
			dir /= "roms\\PONG";
			cpu.loadGame(dir.u8string());
			break;

		default:
			std::cout << "Invalid Input" << std::endl;
			return 1;
			break;
	}

	//Event handler
	SDL_Event event;
	bool run = true;
	while (run)
	{
		if (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				run = false;
			}
		}
		cpu.emulateCycle();
	}

	return 0;
}