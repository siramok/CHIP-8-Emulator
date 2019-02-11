#pragma once
#include <string>
#include <bitset>
#include <vector>
#include <SDL.h>

class chip8
{
	private:
		//Display Stuff
		static const uint8_t WIDTH = 64;
		static const uint8_t HEIGHT = 32;
		static const uint8_t SCALE = 12;
		SDL_Rect screen;
		SDL_Window *window;
		SDL_Renderer *renderer;
		std::bitset<WIDTH * HEIGHT> display;
		uint16_t x_coord;
		uint16_t y_coord;
		uint8_t	pixel;

		//System Stuff
		uint8_t		O, X, Y, N, KK;
		uint16_t	NNN;
		uint16_t	instruction;
		uint8_t memory[4096];
		uint8_t		V[16];
		uint16_t	I;
		uint16_t	PC;
		uint8_t		delay_timer;
		uint8_t		sound_timer;
		uint16_t	stack[16];
		int			SP;
		uint8_t		RND;
		bool		key_is_pressed;


	public:
		chip8();
		~chip8();
		void initializeDisplay();
		void updateScreen();
		void updateScreen(SDL_Rect &);
		uint8_t blit(uint8_t *, uint8_t, uint8_t, uint8_t);
		bool setPixel(uint16_t x, uint16_t y);
		void loadGame(std::string);
		void emulateCycle();
		
		void setValues();
		void memtest();
		void debug();

		bool		drawFlag;
		uint8_t		key[16];

		const uint8_t fontset[80] =
		{
			0xF0, 0x90, 0x90, 0x90, 0xF0,		// 0
			0x20, 0x60, 0x20, 0x20, 0x70,		// 1
			0xF0, 0x10, 0xF0, 0x80, 0xF0,		// 2
			0xF0, 0x10, 0xF0, 0x10, 0xF0,		// 3
			0x90, 0x90, 0xF0, 0x10, 0x10,		// 4
			0xF0, 0x80, 0xF0, 0x10, 0xF0,		// 5
			0xF0, 0x80, 0xF0, 0x90, 0xF0,		// 6
			0xF0, 0x10, 0x20, 0x40, 0x40,		// 7
			0xF0, 0x90, 0xF0, 0x90, 0xF0,		// 8
			0xF0, 0x90, 0xF0, 0x10, 0xF0,		// 9
			0xF0, 0x90, 0xF0, 0x90, 0x90,		// A
			0xE0, 0x90, 0xE0, 0x90, 0xE0,		// B
			0xF0, 0x80, 0x80, 0x80, 0xF0,		// C
			0xE0, 0x90, 0x90, 0x90, 0xE0,		// D
			0xF0, 0x80, 0xF0, 0x80, 0xF0,		// E
			0xF0, 0x80, 0xF0, 0x80, 0x80		// F
		};
};