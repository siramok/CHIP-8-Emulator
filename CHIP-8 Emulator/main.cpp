#define SDL_MAIN_HANDLED
#include <iostream>
#include <filesystem>
#include <bitset>
#include <chrono>
#include <thread>
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
	std::cout << "2. SPACE INVADERS\n" << std::endl;
	std::cin >> choice;

	chip8 cpu;
	switch (choice)
	{
		case 1:
			dir /= "roms\\PONG";
			cpu.loadGame(dir.u8string());
			break;

		case 2:
			dir /= "roms\\INVADERS";
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
			switch (event.type)
			{
				case SDL_QUIT:
					run = false;
					break;
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_1:
							cpu.key[0] = 0x1;
							cpu.waiting_for_key = false;
							break;

						case SDLK_2:
							cpu.key[1] = 0x1;
							cpu.waiting_for_key = false;
							break;

						case SDLK_3:
							cpu.key[2] = 0x1;
							cpu.waiting_for_key = false;
							break;

						case SDLK_4:
							cpu.key[3] = 0x1;
							cpu.waiting_for_key = false;
							break;

						case SDLK_q:
							cpu.key[4] = 0x1;
							cpu.waiting_for_key = false;
							break;

						case SDLK_w:
							cpu.key[5] = 0x1;
							cpu.waiting_for_key = false;
							break;

						case SDLK_e:
							cpu.key[6] = 0x1;
							cpu.waiting_for_key = false;
							break;

						case SDLK_r:
							cpu.key[7] = 0x1;
							cpu.waiting_for_key = false;
							break;

						case SDLK_a:
							cpu.key[8] = 0x1;
							cpu.waiting_for_key = false;
							break;

						case SDLK_s:
							cpu.key[9] = 0x1;
							cpu.waiting_for_key = false;
							break;

						case SDLK_d:
							cpu.key[10] = 0x1;
							cpu.waiting_for_key = false;
							break;

						case SDLK_f:
							cpu.key[11] = 0x1;
							cpu.waiting_for_key = false;
							break;

						case SDLK_z:
							cpu.key[12] = 0x1;
							cpu.waiting_for_key = false;
							break;

						case SDLK_x:
							cpu.key[13] = 0x1;
							cpu.waiting_for_key = false;
							break;

						case SDLK_c:
							cpu.key[14] = 0x1;
							cpu.waiting_for_key = false;
							break;

						case SDLK_v:
							cpu.key[15] = 0x1;
							cpu.waiting_for_key = false;
							break;

						default:
							break;
					}
					break;

				case SDL_KEYUP:
					switch (event.key.keysym.sym)
					{
					case SDLK_1:
						cpu.key[0] = 0x0;
						break;

					case SDLK_2:
						cpu.key[1] = 0x0;
						break;

					case SDLK_3:
						cpu.key[2] = 0x0;
						break;

					case SDLK_4:
						cpu.key[3] = 0x0;
						break;

					case SDLK_q:
						cpu.key[4] = 0x0;
						break;

					case SDLK_w:
						cpu.key[5] = 0x0;
						break;

					case SDLK_e:
						cpu.key[6] = 0x0;
						break;

					case SDLK_r:
						cpu.key[7] = 0x0;
						break;

					case SDLK_a:
						cpu.key[8] = 0x0;
						break;

					case SDLK_s:
						cpu.key[9] = 0x0;
						break;

					case SDLK_d:
						cpu.key[10] = 0x0;
						break;

					case SDLK_f:
						cpu.key[11] = 0x0;
						break;

					case SDLK_z:
						cpu.key[12] = 0x0;
						break;

					case SDLK_x:
						cpu.key[13] = 0x0;
						break;

					case SDLK_c:
						cpu.key[14] = 0x0;
						break;

					case SDLK_v:
						cpu.key[15] = 0x0;
						break;

					default:
						break;
					}
					break;

				default:
					break;
			}
		}
		if (!(cpu.waiting_for_key))
		{
			cpu.emulateCycle();
		}
		std::this_thread::sleep_for(std::chrono::microseconds(800));
	}
	return 0;
}