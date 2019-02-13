#include "CHIP8.h"
#include <iostream>
#include <fstream>
#include <bitset>

chip8::chip8()
{
	initializeDisplay();

	PC = 0x0200;
	instruction = 0x0000;
	I = 0x0000;
	SP = 0;

	for (int i = 0; i < 16; i++)
	{
		V[i] = 0;
		key[i] = 0;
		stack[i] = 0;
	}

	for (int i = 0; i < 4096; i++)
	{
		memory[i] = 0x00;
	}
	
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

	for (int i = 0; i < 80; ++i)
	{
		memory[i] = fontset[i];
	}

	delay_timer = 0x00;
	sound_timer = 0x00;
}

chip8::~chip8()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}

void chip8::initializeDisplay()
{
	//Initialize SDL subsystems
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cout << " SDL error: " << SDL_GetError() << std::endl;
	}

	//Initialize an SDL window
	window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH * SCALE, HEIGHT * SCALE, SDL_WINDOW_SHOWN);
	if (window == nullptr)
	{
		std::cout << " SDL error: " << SDL_GetError() << std::endl;
		SDL_Quit();
	}

	//Initialize an SDL renderer
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == nullptr)
	{
		std::cout << " SDL error: " << SDL_GetError() << std::endl;
		SDL_DestroyWindow(window);
		SDL_Quit();
	}

	screen.x = 0;
	screen.y = 0;
	screen.w = WIDTH;
	screen.h = HEIGHT;

	display.reset();
}

void chip8::loadGame(std::string path)
{
	char *buffer;
	std::ifstream rom;
	rom.open(path, std::ios::binary | std::ios::ate);
	if (rom.is_open())
	{
		std::filebuf * pbuf = rom.rdbuf();
		std::streamsize size = pbuf->pubseekoff(0, rom.end);
		pbuf->pubseekoff(0, rom.beg);
		buffer = new char[size];
		pbuf->sgetn(buffer, size);
		for (int i = 0; i < size; i++)
		{
			memory[0x0200 + i] = buffer[i];
		}
		delete buffer;
	}
	rom.close();
}

void chip8::updateScreen()
{
	SDL_SetRenderDrawColor(renderer, 53, 53, 53, 255); //Background color
	SDL_RenderClear(renderer);
	updateScreen(screen);
}

void chip8::updateScreen(const SDL_Rect &scr)
{
	SDL_SetRenderDrawColor(renderer, 53, 53, 53, 255);
	SDL_Rect rect;
	rect.x = scr.x * SCALE;
	rect.y = scr.y * SCALE;
	rect.w = scr.w * SCALE;
	rect.h = scr.h * SCALE;
	SDL_RenderFillRect(renderer, &rect);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200); //Pixel Color
	for (int i = 0; i < scr.w * scr.h; i++)
	{
		uint16_t x_pos = scr.x + (i % scr.w);
		uint16_t y_pos = scr.y + (i / scr.w) * WIDTH;
		if (display[x_pos + y_pos])
		{
			SDL_Rect pixel;
			pixel.x = (scr.x * SCALE) + (i % scr.w) * SCALE;
			pixel.y = (scr.y * SCALE) + (i / scr.w) * SCALE;
			pixel.w = SCALE;
			pixel.h = SCALE;
			SDL_RenderFillRect(renderer, &pixel);
		}
	}
	SDL_RenderPresent(renderer);
}

uint8_t chip8::blit(uint8_t *temp, uint8_t size, uint8_t x, uint8_t y)
{
	bool clear = false;
	for (int j = 0; j < size; j++)
	{
		auto pixels = temp[j];
		for (int i = 0; i < 8; i++)
		{
			if (pixels & 0x80)
			{
				clear = setPixel(x + i, y + j) || clear;
			}
			pixels = pixels << 1;
		}
	}
	updateScreen();
	return clear;
}

bool chip8::setPixel(uint16_t x, uint16_t y)
{
	while (x > WIDTH - 1)
	{
		x -= WIDTH;
	}
	while (x < 0)
	{
		x += WIDTH;
	}
	while (y > HEIGHT - 1)
	{
		y -= HEIGHT;
	}
	while (y < 0)
	{
		y += HEIGHT;
	}
	display.flip(y * WIDTH + x);
	return !display[y * WIDTH + x];
}

void chip8::setValues()
{
	O = (instruction & 0xF000) >> 12;
	X = (instruction & 0x0F00) >> 8;
	Y = (instruction & 0x00F0) >> 4;
	N = (instruction & 0x000F);
	KK = (instruction & 0x00FF);
	NNN = (instruction & 0x0FFF);
}

void chip8::emulateCycle()
{
	instruction = memory[PC] << 8 | memory[PC + 1];
	setValues();
	switch (instruction & 0xF000)
	{
	case 0x0000:
		switch (instruction & 0x000F)
		{
		case 0x0000: //* CLS - Clear the display
			std::cout << std::uppercase << std::hex << instruction << ": CLS" << std::endl;
			display.reset();
			updateScreen();
			PC += 2;
			break;

		case 0x000E: // RET - Return from a subroutine
			// The interpreter sets the program counter to the address at the top of the stack,
			// then subtracts 1 from the stack pointer.
			std::cout << std::uppercase << std::hex << "00" << instruction << ": RET" << std::endl;
			PC = stack[--SP];
			PC += 2;
			break;

		default:
			std::cout << std::uppercase << "Unknown opcode [" << std::hex << instruction << "]" << std::endl;
			break;
		}
		break;

	case 0x1000: //* JP addr - Jump to location nnn
		std::cout << std::uppercase << std::hex << instruction << ": JP " << unsigned(NNN) << std::endl;
		PC = NNN;
		break;

	case 0x2000: // CALL addr - Call subroutine at nnn
		// The interpreter increments the stack pointer,
		// then puts the current PC on the top of the stack. The PC is then set to nnn.
		std::cout << std::uppercase << std::hex << instruction << ": CALL " << unsigned(NNN) << std::endl;
		stack[SP++] = PC;
		PC = NNN;
		break;

	case 0x3000: //* SE Vx, byte - Skip next instruction if Vx == kk
		std::cout << std::uppercase << std::hex << instruction << ": SE V" << unsigned(X) << ", " << unsigned(KK) << std::endl;
		if (V[X] == KK)
		{
			PC += 2;
		}
		PC += 2;
		break;

	case 0x4000: //* SNE Vx, byte - Skip next instruction if Vx != kk
		std::cout << std::uppercase << std::hex << instruction << ": SNE V" << unsigned(X) << ", " << unsigned(KK) << std::endl;
		if (V[X] != KK)
		{
			PC += 2;
		}
		PC += 2;
		break;

	case 0x5000: //* SE Vx, Vy - Skip next instruction if Vx == Vy
		std::cout << std::uppercase << std::hex << instruction << ": SE V" << unsigned(X) << ", V" << unsigned(Y) << std::endl;
		if (V[X] == V[Y])
		{
			PC += 2;
		}
		PC += 2;
		break;

	case 0x6000: //* LD Vx, byte - Set Vx = kk
		std::cout << std::uppercase << std::uppercase << std::hex << instruction << ": LD V" << unsigned(X) << ", " << unsigned(KK) << std::endl;
		V[X] = KK;
		PC += 2;
		break;

	case 0x7000: //* ADD Vx, byte - Set Vx += kk
		std::cout << std::uppercase << std::hex << instruction << ": ADD V" << unsigned(X) << ", " << unsigned(KK) << std::endl;
		V[X] += KK;
		PC += 2;
		break;

	case 0x8000:
		switch (instruction & 0x000F)
		{
		case 0x0000: //* LD Vx, Vy - Set Vx = Vy
			std::cout << std::uppercase << std::hex << instruction << ": LD V" << unsigned(X) << ", V" << unsigned(Y) << std::endl;
			V[X] = V[Y];
			PC += 2;
			break;

		case 0x0001: //* OR Vx, Vy - Set Vx = Vx OR Vy
			std::cout << std::uppercase << std::hex << instruction << ": OR V" << unsigned(X) << ", V" << unsigned(Y) << std::endl;
			V[X] = (V[X] | V[Y]);
			PC += 2;
			break;

		case 0x0002: //* AND Vx, Vy - Set Vx = Vx AND Vy
			std::cout << std::uppercase << std::hex << instruction << ": AND V" << unsigned(X) << ", V" << unsigned(Y) << std::endl;
			V[X] = (V[X] & V[Y]);
			PC += 2;
			break;

		case 0x0003: //* XOR Vx, Vy - Set Vx = Vx XOR Vy
			std::cout << std::uppercase << std::hex << instruction << ": XOR V" << unsigned(X) << ", V" << unsigned(Y) << std::endl;
			V[X] = (V[X] ^ V[Y]);
			PC += 2;
			break;

		case 0x0004: //* ADD Vx, Vy - Set Vx = Vx + Vy, set VF = carry
			std::cout << std::uppercase << std::hex << instruction << ": ADD V" << unsigned(X) << ", V" << unsigned(Y) << std::endl;
			
			if (V[X] + V[Y] > 0xFF)
			{
				V[0xF] = 0x01;
			}
			else
			{
				V[0xF] = 0x00;
			}
			V[X] += V[Y];
			PC += 2;
			break;

		case 0x0005: //* SUB Vx, Vy - Set Vx = Vx - Vy, set VF = NOT borrow
			std::cout << std::uppercase << std::hex << instruction << ": SUB V" << unsigned(X) << ", V" << unsigned(Y) << std::endl;
			if (V[X] > V[Y])
			{
				V[0xF] = 0x01;
			}
			else
			{
				V[0xF] = 0x00;
			}
			V[X] -= V[Y];
			PC += 2;
			break;

		case 0x0006: //* SHR Vx, Vy - Set Vx = Vx SHR 1
			std::cout << std::uppercase << std::hex << instruction << ": SHR V" << unsigned(X) << ", V" << unsigned(Y) << std::endl;
			if (V[X] & 0x01)
			{
				V[0xF] = 0x01;
			}
			else
			{
				V[0xF] = 0x00;
			}
			V[X] /= 2;
			PC += 2;
			break;

		case 0x0007: //* SUBN Vx, Vy - Set Vx = Vy - Vx, set VF = NOT borrow
			std::cout << std::uppercase << std::hex << instruction << ": SUBN V" << unsigned(X) << ", V" << unsigned(Y) << std::endl;
			if (V[Y] > V[X])
			{
				V[0xF] = 0x01;
			}
			else
			{
				V[0xF] = 0x00;
			}
			V[X] = V[Y] - V[X];
			PC += 2;
			break;

		case 0x000E: //* SHL Vx, Vy - Set Vx = Vx SHL 1
			std::cout << std::uppercase << std::hex << instruction << ": SHL V" << unsigned(X) << ", V" << unsigned(Y) << std::endl;
			if (V[X] & 0x0001)
			{
				V[0xF] = 0x01;
			}
			else
			{
				V[0xF] = 0x00;
			}
			V[X] *= 2;
			PC += 2;
			break;

		default:
			std::cout << "Unknown opcode [" << std::hex << instruction << "]" << std::endl;
			break;
		}
		break;

	case 0x9000: //* SNE Vx, Vy - Skip next instruction if Vx != Vy
		std::cout << std::uppercase << std::hex << instruction << ": SNE V" << unsigned(X) << ", V" << unsigned(Y) << std::endl;
		if (V[X] != V[Y])
		{
			PC += 2;
		}
		PC += 2;
		break;

	case 0xA000: //* LD I, addr - Set I = nnn
		std::cout << std::uppercase << std::hex << instruction << ": LD I, " << unsigned(NNN) << std::endl;
		I = NNN;
		PC += 2;
		break;

	case 0xB000: //* JP V0, addr - Jump to location nnn + V0
		std::cout << std::uppercase << std::hex << instruction << ": JP V0, " << unsigned(NNN) << std::endl;
		PC = (NNN + V[0]);
		break;

	case 0xC000: //* RND Vx, byte - Set Vx = random byte AND kk
		std::cout << std::uppercase << std::hex << instruction << ": RND V" << unsigned(X) << ", " << unsigned(KK) << std::endl;
		V[X] = (std::rand() % 256) & KK;
		PC += 2;
		break;

	case 0xD000: //* DRW Vx, Vy, n - Display n-byte sprite starting at memory location I at (Vx, Vy),
		std::cout << std::uppercase << std::hex << instruction << ": DRW V" << unsigned(X) << ", V" << unsigned(Y) << ", " << unsigned(N) << std::endl;
		V[0xF] = blit(&memory[I], N, V[X], V[Y]);
		PC += 2;
		break;

	case 0xE000:
		switch (instruction & 0x00FF)
		{
		case 0x009E: //* SKP Vx - Skip next instruction if key with the value Vx is pressed
			std::cout << std::uppercase << std::hex << instruction << ": SKP V" << unsigned(X) << std::endl;
			if (key[V[X]] != 0x00)
			{
				PC += 2;
			}
			PC += 2;
			break;

		case 0x00A1: //* SKNP Vx - Skip next instruction if key with the value Vx is not pressed
			std::cout << std::uppercase << std::hex << instruction << ": SKNP V" << unsigned(X) << std::endl;
			if (key[V[X]] == 0x00)
			{
				PC += 2;
			}
			PC += 2;
			break;

		default:
			std::cout << "Unknown opcode [" << std::hex << instruction << "]" << std::endl;
			break;
		}
		break;

	case 0xF000:
		switch(instruction & 0x00FF)
		{
			case 0x0007: //* LD Vx, DT - Set Vx = delay timer value
				std::cout << std::uppercase << std::hex << instruction << ": LD V" << unsigned(X) << ", " << unsigned(delay_timer) << std::endl;
				V[X] = delay_timer;
				PC += 2;
				break;

			case 0x000A: // LD Vx, K - Wait for a key press, store the value of the key in Vx
				// All execution stops until a key is pressed, then the value of that key is stored in Vx.
				// For later: make a function that loops and returns the value of the key being pressed
				std::cout << std::uppercase << std::hex << instruction << ": LD V" << unsigned(X) << " K" << std::endl;
				V[X] = waitForKey();
				PC += 2;
				break;

			case 0x0015: //* LD DT, Vx - Set delay timer = Vx
				std::cout << std::uppercase << std::hex << instruction << ": LD " << unsigned(delay_timer) << ", V" << unsigned(X) << std::endl;
				delay_timer = V[X];
				PC += 2;
				break;

			case 0x0018: //* LD ST, Vx - Set sound timer = Vx
				std::cout << std::uppercase << std::hex << instruction << ": LD "  << unsigned(sound_timer) << ", V"  << unsigned(X) << std::endl;
				sound_timer = V[X];
				PC += 2;
				break;

			case 0x001E: //* ADD I, Vx - Set I = I + Vx
				std::cout << std::uppercase << std::hex << instruction << ": ADD I, V" << unsigned(X) << std::endl;
				I += V[X];
				PC += 2;
				break;

			case 0x0029: //* LD F, Vx - Set I = location of sprite for digit Vx
				std::cout << std::uppercase << std::hex << instruction << ": LD F, V" << unsigned(X) << std::endl;
				I = V[X] * 0x05;
				PC += 2;
				break;

			case 0x0033: // LD B, Vx - Store BCD representation of Vx in memory locations I, I+1, and I+2
				// The interpreter takes the decimal value of Vx, and places the hundreds digit in memory
				// at location in I, the tens digit at location I+1, and the ones digit at location I+2.
				std::cout << std::uppercase << std::hex << instruction << ": LD B, V" << unsigned(X) << std::endl;
				memory[I] = V[X] / 100;
				memory[I + 1] = (V[X] / 10) % 10;
				memory[I + 2] = (V[X] % 100) % 10;
				PC += 2;
				break;

			case 0x0055: //* LD [I], Vx - Store registers V0 through Vx in memory starting at location I
				std::cout << std::uppercase << std::hex << instruction << ": LD [I], V" << unsigned(X) << std::endl;
				for (int i = 0; i <= X; ++i)
				{
					memory[I + i] = V[i];
				}
				PC += 2;
				break;

			case 0x0065: // LD Vx, [I] - Read registers V0 through Vx from memory starting at location I
				// The interpreter reads values from memory starting at location I into registers V0 through Vx.
				std::cout << std::uppercase << std::hex << instruction << ": LD V" << unsigned(X) << ", [I]" << std::endl;
				for (int i = 0; i <= X; ++i)
				{
					V[i] = memory[I + i];
				}
				PC += 2;
				break;

			default:
				std::cout << "Unknown opcode [" << std::hex << instruction << "]" << std::endl;
				break;
		}
		break;

	default:
		std::cout << "Unknown opcode [" << std::hex << instruction << "]" << std::endl;
		break;
	}
	
	if (delay_timer > 0)
	{
		--delay_timer;
	}

	if (sound_timer > 0)
	{
		if (sound_timer == 1)
		{
			// Play a sound
		}
		--sound_timer;
	}
	
}

uint8_t chip8::waitForKey()
{
	SDL_Event event;
	while (true)
	{
		if (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_x:
							return 0x00;
							break;

						case SDLK_1:
							return 0x01;
							break;

						case SDLK_2:
							return 0x02;
							break;

						case SDLK_3:
							return 0x03;
							break;

						case SDLK_q:
							return 0x04;
							break;

						case SDLK_w:
							return 0x05;
							break;

						case SDLK_e:
							return 0x06;
							break;

						case SDLK_a:
							return 0x07;
							break;

						case SDLK_s:
							return 0x08;
							break;

						case SDLK_d:
							return 0x09;
							break;

						case SDLK_z:
							return 0x0A;
							break;

						case SDLK_c:
							return 0x0B;
							break;

						case SDLK_4:
							return 0x0C;
							break;

						case SDLK_r:
							return 0x0D;
							break;

						case SDLK_f:
							return 0x0E;
							break;

						case SDLK_v:
							return 0x0F;
							break;

						default:
							break;
					}
				break;
			}
		}
	}
}

void chip8::memtest()
{
	for (int i = 0; i < 758; i++)
	{
		std::bitset<8> y(memory[i]);
		std::cout << std::hex << y << " " << i << std::endl;
	}
}

void chip8::debug()
{
	std::cout << '\n' << std::endl;
	std::cout << std::hex << "Cur PC = " << PC << std::endl;
	std::cout << "Cur Stack = " << stack[SP] << std::endl;
	std::cout << "Cur SP = " << SP << std::endl;
	std::cout << "Cur I = " << I << std::endl;
	std::cout << "Cur delay = " << delay_timer << std::endl;
	std::cout << "Cur sound = " << sound_timer << std::endl;
	for (int i = 0; i < 16; i++)
	{
		std::bitset<8> z(V[i]);
		std::cout << std::hex << "Register " << i << ": " << z << std::endl;
	}
}