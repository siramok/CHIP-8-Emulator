#define SDL_MAIN_HANDLED
#include <iostream>
#include <filesystem>
#include <bitset>
#include <chrono>
#include <thread>
#include <SDL.h>
#include "CHIP8.h"
namespace fs = std::filesystem;

int main(int argc, char **argv[])
{
	fs::path dir = fs::current_path();
	int choice;

	//A basic console menu that lets the user choose a rom to load
	std::cout << "CHOOSE A GAME TO LOAD" << std::endl;
	std::cout << "Default rom path: " << dir << "\\" << "\\roms" << std::endl;
	std::cout << "\n1. 15PUZZLE" << std::endl;
	std::cout << "2. BLINKY" << std::endl;
	std::cout << "3. BLITZ" << std::endl;
	std::cout << "4. BRIX" << std::endl;
	std::cout << "5. CONNECT4" << std::endl;
	std::cout << "6. GUESS" << std::endl;
	std::cout << "7. HIDDEN" << std::endl;
	std::cout << "8. INVADERS" << std::endl;
	std::cout << "9. KALEID" << std::endl;
	std::cout << "10. MAZE" << std::endl;
	std::cout << "11. MERLIN" << std::endl;
	std::cout << "12. MISSILE" << std::endl;
	std::cout << "13. PONG" << std::endl;
	std::cout << "14. PONG2" << std::endl;
	std::cout << "15. PUZZLE" << std::endl;
	std::cout << "16. SYZYGY" << std::endl;
	std::cout << "17. TANK" << std::endl;
	std::cout << "18. TETRIS" << std::endl;
	std::cout << "19. TICTAC" << std::endl;
	std::cout << "20. UFO" << std::endl;
	std::cout << "21. VBRIX" << std::endl;
	std::cout << "22. VERS" << std::endl;
	std::cout << "23. WIPEOFF" << std::endl;
	std::cout << "24. QUIT\n" << std::endl;
	std::cin >> choice;

	chip8 cpu;
	switch (choice)
	{
		case 1:
			dir /= "roms\\15PUZZLE";
			cpu.loadGame(dir.u8string());
			break;

		case 2:
			dir /= "roms\\BLINKY";
			cpu.loadGame(dir.u8string());
			break;

		case 3:
			dir /= "roms\\BLITZ";
			cpu.loadGame(dir.u8string());
			break;

		case 4:
			dir /= "roms\\BRIX";
			cpu.loadGame(dir.u8string());
			break;

		case 5:
			dir /= "roms\\CONNECT4";
			cpu.loadGame(dir.u8string());
			break;

		case 6:
			dir /= "roms\\GUESS";
			cpu.loadGame(dir.u8string());
			break;

		case 7:
			dir /= "roms\\HIDDEN";
			cpu.loadGame(dir.u8string());
			break;

		case 8:
			dir /= "roms\\INVADERS";
			cpu.loadGame(dir.u8string());
			break;

		case 9:
			dir /= "roms\\KALEID";
			cpu.loadGame(dir.u8string());
			break;

		case 10:
			dir /= "roms\\MAZE";
			cpu.loadGame(dir.u8string());
			break;

		case 11:
			dir /= "roms\\MERLIN";
			cpu.loadGame(dir.u8string());
			break;

		case 12:
			dir /= "roms\\MISSILE";
			cpu.loadGame(dir.u8string());
			break;

		case 13:
			dir /= "roms\\PONG";
			cpu.loadGame(dir.u8string());
			break;

		case 14:
			dir /= "roms\\PONG2";
			cpu.loadGame(dir.u8string());
			break;

		case 15:
			dir /= "roms\\PUZZLE";
			cpu.loadGame(dir.u8string());
			break;

		case 16:
			dir /= "roms\\SYZYGY";
			cpu.loadGame(dir.u8string());
			break;

		case 17:
			dir /= "roms\\TANK";
			cpu.loadGame(dir.u8string());
			break;

		case 18:
			dir /= "roms\\TETRIS";
			cpu.loadGame(dir.u8string());
			break;

		case 19:
			dir /= "roms\\TICTAC";
			cpu.loadGame(dir.u8string());
			break;

		case 20:
			dir /= "roms\\UFO";
			cpu.loadGame(dir.u8string());
			break;

		case 21:
			dir /= "roms\\VBRIX";
			cpu.loadGame(dir.u8string());
			break;

		case 22:
			dir /= "roms\\VERS";
			cpu.loadGame(dir.u8string());
			break;

		case 23:
			dir /= "roms\\WIPEOFF";
			cpu.loadGame(dir.u8string());
			break;

		case 24:
			SDL_Quit();
			return 0;
			break;

		default:
			std::cout << "Invalid Input: for now this causes the program to end" << std::endl;
			return 1;
			break;
	}

	//Event handler
	SDL_Event event;
	bool run = true;
	while (run)
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
					run = false;
					break;

				case SDL_KEYDOWN:
					for (int i = 0; i <= 15; ++i)
					{
						if (event.key.keysym.sym == cpu.keyList[i])
						{
							cpu.key.set(i);
						}
					}
					break;

				case SDL_KEYUP:
					for (int i = 0; i <= 15; ++i)
					{
						if (event.key.keysym.sym == cpu.keyList[i])
						{
							cpu.key.reset(i);
						}
					}
					break;
					
				default:
					break;
			}
		}

		cpu.emulateCycle();
		std::this_thread::sleep_for(std::chrono::microseconds(1200));
	}

	return 0;
}