#define SDL_MAIN_HANDLED
#include <iostream>
#include <filesystem>
#include <SDL.h>
#include "CHIP8.h"
namespace fs = std::filesystem;

int main(int argc, char **argv[])
{
	chip8 cpu;
	int choice;
	fs::path dir = fs::current_path();

	std::cout << "CHOOSE A GAME TO LOAD" << std::endl;
	std::cout << "Default rom path: " << dir << "\\" << "\\roms" << std::endl;
	std::cout << "\n1. PONG" << std::endl;
	std::cin >> choice;

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

	while (true)
	{
		cpu.emulateCycle();

		if (cpu.drawFlag)
		{

		}

	}

	return 0;
}
