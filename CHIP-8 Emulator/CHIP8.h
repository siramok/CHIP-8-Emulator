#pragma once
#include <string>
#include <bitset>
#include <vector>
#include <SDL.h>

class chip8
{
	private:
		//SDL
		static const uint8_t WIDTH = 64;
		static const uint8_t HEIGHT = 32;
		static const uint8_t SCALE = 12;
		SDL_Rect screen;
		SDL_Window *window;
		SDL_Renderer *renderer;
		std::bitset<WIDTH * HEIGHT> display;
		uint8_t	pixel;

		//Core
		uint8_t	O, X, Y, N, KK;
		uint16_t NNN;
		uint16_t instruction;
		uint8_t	memory[4096];
		uint8_t	V[16];
		uint16_t I;
		uint16_t PC;
		uint8_t	delay_timer;
		uint8_t	sound_timer;
		uint16_t stack[16];
		uint8_t	SP;

	public:
		chip8();
		~chip8();

		//SDL
		void initializeDisplay();
		void updateScreen();
		void updateScreen(const SDL_Rect &scr);
		uint8_t blit(uint8_t *, uint8_t, uint8_t, uint8_t);
		bool setPixel(uint16_t x, uint16_t y);

		//Core
		void loadGame(std::string);
		void setValues();
		void emulateCycle();
		uint8_t waitForKey();
		void memtest();
		void debug();

		//Public Variables
		std::bitset<16> key;
		std::vector<uint8_t> keyList = {
			SDLK_x,
			SDLK_1,
			SDLK_2,
			SDLK_3,
			SDLK_q,
			SDLK_w,
			SDLK_e,
			SDLK_a,
			SDLK_s,
			SDLK_d,
			SDLK_z,
			SDLK_c,
			SDLK_4,
			SDLK_r,
			SDLK_f,
			SDLK_v,
		};
};