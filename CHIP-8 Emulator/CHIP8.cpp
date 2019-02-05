#include "CHIP8.h"
#include <iostream>
#include <fstream>
#include <bitset>

chip8::chip8()
{

	PC = 0x0200;
	instruction = 0x0000;
	I = 0x0000;
	SP = 0;
	
	display.reset();

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
	
	for (int i = 0; i < 80; ++i)
	{
		memory[i] = fontset[i];
	}

	delay_timer = 0x00;
	sound_timer = 0x00;
}

chip8::~chip8()
{
	
}

void chip8::loadGame(std::string path)
{
	char * buffer;
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

void chip8::emulateCycle()
{
	instruction = memory[PC] << 8 | memory[PC + 1];
	setValues();

	switch (instruction & 0xF000)
	{
		case 0x0000:
			switch (instruction & 0x000F)
			{
				case 0x0000: // CLS - Clear the display
					std::cout << std::uppercase << std::hex << instruction << ": CLS" << std::endl;
					display.reset();
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

		case 0x1000: // JP addr - Jump to location nnn
			// The interpreter sets the program counter to nnn.
			std::cout << std::uppercase << std::hex << instruction << ": JP " << unsigned(NNN) << std::endl;
			PC = NNN;
			break;

		case 0x2000: // CALL addr - Call subroutine at nnn
			// The interpreter increments the stack pointer,
			// then puts the current PC on the top of the stack. The PC is then set to nnn.
			std::cout << std::uppercase << std::hex << instruction << ": CALL " << unsigned(NNN) << std::endl;
			stack[SP] = PC;
			++SP;
			PC = NNN;
			break;

		case 0x3000: // SE Vx, byte - Skip next instruction if Vx == kk
			// The interpreter compares register Vx to kk, and if they are equal,
			// increments the program counter by 2.
			std::cout << std::uppercase << std::hex << instruction << ": SE V" << unsigned(X) << ", " << unsigned(KK) << std::endl;
			if (V[X] == KK)
			{
				PC += 2;
			}
			PC += 2;
			break;

		case 0x4000: // SNE Vx, byte - Skip next instruction if Vx != kk
			// The interpreter compares register Vx to kk, and if they are not equal,
			// increments the program counter by 2.
			std::cout << std::uppercase << std::hex << instruction << ": SNE V" << unsigned(X) << ", " << unsigned(KK) << std::endl;
			if (V[X] != KK)
			{
				PC += 2;
			}
			PC += 2;
			break;

		case 0x5000: // SE Vx, Vy - Skip next instruction if Vx == Vy
			// The interpreter compares register Vx to register Vy,
			// and if they are equal, increments the program counter by 2.
			std::cout << std::uppercase << std::hex << instruction << ": SE V" << unsigned(X) << ", V" << unsigned(Y) << std::endl;
			if (V[X] == V[Y])
			{
				PC += 2;
			}
			PC += 2;
			break;

		case 0x6000: // LD Vx, byte - Set Vx = kk
			// The interpreter puts the value kk into register Vx.
			std::cout << std::uppercase << std::uppercase << std::hex << instruction << ": LD V" << unsigned(X) << ", " << unsigned(KK) << std::endl;
			V[X] = KK;
			PC += 2;
			break;

		case 0x7000: // ADD Vx, byte - Set Vx += kk
			// Adds the value kk to the value of register Vx, then stores the result in Vx.
			std::cout << std::uppercase << std::hex << instruction << ": ADD V" << unsigned(X) << ", " << unsigned(KK) << std::endl;
			V[X] += KK;
			PC += 2;
			break;

		case 0x8000:
			switch (instruction & 0x000F)
			{
			case 0x0000: // LD Vx, Vy - Set Vx = Vy
				// Stores the value of register Vy in register Vx.
				std::cout << std::uppercase << std::hex << instruction << ": LD V" << unsigned(X) << ", V" << unsigned(Y) << std::endl;
				V[X] = V[Y];
				PC += 2;
				break;

			case 0x0001: // OR Vx, Vy - Set Vx = Vx OR Vy
				// Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx.
				// A bitwise OR compares the corrseponding bits from two values,
				// and if either bit is 1, then the same bit in the result is also 1. Otherwise, it is 0.
				std::cout << std::uppercase << std::hex << instruction << ": OR V" << unsigned(X) << ", V" << unsigned(Y) << std::endl;
				V[X] = (V[X] | V[Y]);
				PC += 2;
				break;

			case 0x0002: // AND Vx, Vy - Set Vx = Vx AND Vy
				// Performs a bitwise AND on the values of Vx and Vy, then stores the result in Vx.
				std::cout << std::uppercase << std::hex << instruction << ": AND V" << unsigned(X) << ", V" << unsigned(Y) << std::endl;
				V[X] = (V[X] & V[Y]);
				PC += 2;
				break;

			case 0x0003: // XOR Vx, Vy - Set Vx = Vx XOR Vy
				// Performs a bitwise exclusive OR on the values of Vx and Vy, then stores the result in Vx.
				std::cout << std::uppercase << std::hex << instruction << ": XOR V" << unsigned(X) << ", V" << unsigned(Y) << std::endl;
				V[X] = (V[X] ^ V[Y]);
				PC += 2;
				break;

			case 0x0004: // ADD Vx, Vy - Set Vx = Vx + Vy, set VF = carry
				// The values of Vx and Vy are added together.
				// If the result is greater than 8 bits (i.e., > 255,) VF is set to 1,
				// otherwise 0. Only the lowest 8 bits of the result are kept, and stored in Vx.
				std::cout << std::uppercase << std::hex << instruction << ": ADD V" << unsigned(X) << ", V" << unsigned(Y) << std::endl;
				if ((V[X] + V[Y]) > 0xFF)
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

			case 0x0005: // SUB Vx, Vy - Set Vx = Vx - Vy, set VF = NOT borrow
				// If Vx > Vy, then VF is set to 1, otherwise 0.
				// Then Vy is subtracted from Vx, and the results stored in Vx.
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

			case 0x0006: // SHR Vx, Vy - Set Vx = Vx SHR 1
				// If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0.
				// Then Vx is divided by 2.
				std::cout << std::uppercase << std::hex << instruction << ": SHR V" << unsigned(X) << ", V" << unsigned(Y) << std::endl;
				if ((V[X] & 0x0001) == 0x0001)
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

			case 0x0007: // SUBN Vx, Vy - Set Vx = Vy - Vx, set VF = NOT borrow
				// If Vy > Vx, then VF is set to 1, otherwise 0.
				// Then Vx is subtracted from Vy, and the results stored in Vx.
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

			case 0x000E: // SHL Vx, Vy - Set Vx = Vx SHL 1
				// If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0.
				// Then Vx is multiplied by 2.
				std::cout << std::uppercase << std::hex << instruction << ": SHL V" << unsigned(X) << ", V" << unsigned(Y) << std::endl;
				if ((V[X] & 0x0001) == 0x0001)
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

		case 0x9000: // SNE Vx, Vy - Skip next instruction if Vx != Vy
			// The values of Vx and Vy are compared, and if they are not equal,
			// the program counter is increased by 2.
			std::cout << std::uppercase << std::hex << instruction << ": SNE V" << unsigned(X) << ", V" << unsigned(Y) << std::endl;
			if (V[X] != V[Y])
			{
				PC += 2;
			}
			PC += 2;
			break;

		case 0xA000: // LD I, addr - Set I = nnn
			// The value of register I is set to nnn.
			std::cout << std::uppercase << std::hex << instruction << ": LD I, " << unsigned(NNN) << std::endl;
			I = NNN;
			PC += 2;
			break;

		case 0xB000: // JP V0, addr - Jump to location nnn + V0
			// The program counter is set to nnn plus the value of V0.
			std::cout << std::uppercase << std::hex << instruction << ": JP V0, " << unsigned(NNN) << std::endl;
			PC = NNN + V[0];
			break;

		case 0xC000: // RND Vx, byte - Set Vx = random byte AND kk
			// The interpreter generates a random number from 0 to 255,
			// which is then ANDed with the value kk. The results are stored in Vx.
			std::cout << std::uppercase << std::hex << instruction << ": RND V" << unsigned(X) << ", " << unsigned(KK) << std::endl;
			RND = std::rand() % 256;
			V[X] = KK & RND;
			PC += 2;
			break;

		case 0xD000: // DRW Vx, Vy, n - Display n-byte sprite starting at memory location I at (Vx, Vy),
					 // set VF = collision
			// The interpreter reads n bytes from memory, starting at the address stored in I.
			// These bytes are then displayed as sprites on screen at coordinates (Vx, Vy).
			// Sprites are XORed onto the existing screen.
			// If this causes any pixels to be erased, VF is set to 1, otherwise it is set to 0.
			// If the sprite is positioned so part of it is outside the coordinates of the display,
			// it wraps around to the opposite side of the screen.
			// See section 2.4, Display, for more information on the Chip-8 screen and sprites.
			std::cout << std::uppercase << std::hex << instruction << ": DRW V" << unsigned(X) << ", V" << unsigned(Y) << ", " << unsigned(N) << std::endl;
			x_coord = V[X];
			y_coord = V[Y];

			V[0xF] = 0x00;
			for (int j = 0; j < N; ++j)
			{
				for (int i = 0; i < 8; ++i)
				{
					if ((memory[I + j] & (0x80 >> i)) != 0)
					{
						pixel = ((V[X] + i) + ((V[Y] + j) << 6)) % 2048;
						V[0xF] |= display[pixel] & 1;
						display[pixel] = ~display[pixel];
					}
				}
			}

			drawFlag = true;
			PC += 2;
			break;

		case 0xE000:
			switch (instruction & 0x000F)
			{
			case 0x000E: // SKP Vx - Skip next instruction if key with the value Vx is pressed
				// Checks the keyboard, and if the key corresponding to the value
				// of Vx is currently in the down position, PC is increased by 2.
				std::cout << std::uppercase << std::hex << instruction << ": SKP V" << unsigned(X) << std::endl;
				if (key[V[X]] == 0x01)
				{
					PC += 2;
				}
				PC += 2;
				break;

			case 0x0001: // SKNP Vx - Skip next instruction if key with the value Vx is not pressed
				// Checks the keyboard, and if the key corresponding to the value
				// of Vx is currently in the up position, PC is increased by 2.
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
				case 0x0007: // LD Vx, DT - Set Vx = delay timer value
					// The value of DT is placed into Vx.
					std::cout << std::uppercase << std::hex << instruction << ": LD V" << unsigned(X) << ", " << unsigned(delay_timer) << std::endl;
					V[X] = delay_timer;
					PC += 2;
					break;

				case 0x000A: // LD Vx, K - Wait for a key press, store the value of the key in Vx
					// All execution stops until a key is pressed, then the value of that key is stored in Vx.
					std::cout << std::uppercase << std::hex << instruction << ": LD V" << unsigned(X) << " K" << std::endl;
					while (key_is_pressed == false)
					{
						for (int i = 0; i < 16; i++)
						{
							if (key[i] == 0x01)
							{
								key_is_pressed = true;
							}
						}
					}
					PC += 2;
					break;

				case 0x0015: // LD DT, Vx - Set delay timer = Vx
					// DT is set equal to the value of Vx.
					std::cout << std::uppercase << std::hex << instruction << ": LD " << unsigned(delay_timer) << ", V" << unsigned(X) << std::endl;
					delay_timer = V[X];
					PC += 2;
					break;

				case 0x0018: // LD ST, Vx - Set sound timer = Vx
					// ST is set equal to the value of Vx.
					std::cout << std::uppercase << std::hex << instruction << ": LD "  << unsigned(sound_timer) << ", V"  << unsigned(X) << std::endl;
					sound_timer = V[X];
					PC += 2;
					break;

				case 0x001E: // ADD I, Vx - Set I = I + Vx
					std::cout << std::uppercase << std::hex << instruction << ": ADD I, V" << unsigned(X) << std::endl;
					I += V[X];
					PC += 2;
					break;

				case 0x0029: // LD F, Vx - Set I = location of sprite for digit Vx
					// The value of I is set to the location for the hexadecimal sprite corresponding
					// to the value of Vx.
					// See section 2.4, Display, for more information on the Chip-8 hexadecimal font.
					std::cout << std::uppercase << std::hex << instruction << ": LD F, V" << unsigned(X) << std::endl;
					I = V[X] * 5;
					PC += 2;
					break;

				case 0x0033: // LD B, Vx - Store BCD representation of Vx in memory locations I, I+1, and I+2
					// The interpreter takes the decimal value of Vx, and places the hundreds digit in memory
					// at location in I, the tens digit at location I+1, and the ones digit at location I+2.
					std::cout << std::uppercase << std::hex << instruction << ": LD B, V" << unsigned(X) << std::endl;
					memory[I] = V[X] / 100;
					memory[I + 1] = (V[X] / 10) % 10;
					memory[I + 2] = (V[X] % 100) / 10;
					PC += 2;
					break;

				case 0x0055: // LD [I], Vx - Store registers V0 through Vx in memory starting at location I
					// The interpreter reads values from registers V0 through Vx into memory starting at location I.
					std::cout << std::uppercase << std::hex << instruction << ": LD [I], V" << unsigned(X) << std::endl;
					for (int i = 0; i < X; ++i)
					{
						memory[I++] = V[i];
						I += 1;
					}
					break;

				case 0x0065: // LD Vx, [I] - Read registers V0 through Vx from memory starting at location I
					// The interpreter reads values from memory starting at location I into registers V0 through Vx.
					std::cout << std::uppercase << std::hex << instruction << ": LD V" << unsigned(X) << ", [I]" << std::endl;
					for (int i = 0; i < X; ++i)
					{
						V[i] = memory[I++];
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

void chip8::setValues()
{
	O = (instruction & 0xF000) >> 12;
	X = (instruction & 0x0F00) >> 8;
	Y = (instruction & 0x00F0) >> 4;
	N = (instruction & 0x000F);
	KK = (instruction & 0x00FF);
	NNN = (instruction & 0x0FFF);
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