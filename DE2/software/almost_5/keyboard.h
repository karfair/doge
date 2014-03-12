#include <assert.h>
#include <priv/alt_legacy_irq.h>
#include "altera_up_ps2_keyboard.h"
#include "altera_up_avalon_character_lcd.h"
#include "altera_up_avalon_ps2.h"
#include "nios.h"
#include "alt_types.h"

#define KEYBOARD_NAME "/dev/ps2_0"

//keyboard interrupt variables
alt_up_ps2_dev *ps2;
KB_CODE_TYPE decode_mode;


extern alt_up_char_buffer_dev *char_buffer;

char ascii;
alt_u8 buf[4];

extern volatile int game_state;
extern volatile int highlighted_item;
extern volatile int highlighted_level;
volatile int priviledge = 1;
volatile int ind;

char playername[12];
char decode(short int buffer);

static void keyboard_ISR( void *mstr, alt_u32 id ){
	master* m = (master*)mstr;
	charac* c = m->c;

	//keyboard interrupt handler
	decode_scancode(ps2,&decode_mode, buf, &ascii);

	if (game_state == 0){
			//game_state == 0 is TITLE SCREEN
			if (decode_mode == KB_BINARY_MAKE_CODE){
				if (buf[0]==0x29){ //SPACE BAR
						game_state =  9;
						highlighted_item = 3;
				}
			}
	} else if (game_state == 1){
			//game_state == 1 is MAIN MENU
			if (decode_mode == KB_BINARY_MAKE_CODE){
					if (buf[0]==0x29) game_state =  highlighted_item;

			} else if (decode_mode == KB_LONG_BINARY_MAKE_CODE) {
					if (buf[0]==0x74){ //RIGHT ARROW
							if (highlighted_item < 4){
								highlighted_item++;
								update_highlight();
							}

					} else if (buf[0]==0x6B){ //LEFT ARROW
							if (highlighted_item > 2){
								highlighted_item--;
								update_highlight();
							}
					}
			}
	} else if (game_state == 2){
			//game_state == 2 is INSTRUCTIONS
			if (decode_mode == KB_BINARY_MAKE_CODE){
					if (buf[0]==0x29) game_state =  1;
			}

	} else if (game_state == 3){
			//game_state == 3 is LEVEL SELECT
			if (decode_mode == KB_BINARY_MAKE_CODE){
					if (buf[0]==0x29){ //SPACE BAR
							game_state = 5;
							printf("Next Screen: %i\n Entering level: %i\n", game_state, highlighted_level);
					}

			} else if (decode_mode == KB_LONG_BINARY_MAKE_CODE) {
					if (buf[0]==0x74){ //RIGHT ARROW
							if (highlighted_level < priviledge){
									highlighted_level++;
									update_highlight();
							}

					} else if (buf[0]==0x6B){ //LEFT ARROW
							if (highlighted_level > 1){
									highlighted_level--;
									update_highlight();
							}
					}
			}

	} else if (game_state == 4){
		//game_state == 4 is DOGE COLOUR
		if (decode_mode == KB_BINARY_MAKE_CODE){
				if (buf[0]==0x29) game_state =  1;
		} else if(decode_mode == KB_ASCII_MAKE_CODE){
	 		if(buf[0]==0x1C){

	 			bmp * dog10 = read_bmp("dog10.bmp");
	 			alt_up_char_buffer_string(char_buffer, "dog10.bmp", 0, 28);

	 			bmp * dog11 = read_bmp("dog11.bmp");
	 			alt_up_char_buffer_string(char_buffer, "dog11.bmp", 0, 29);

	 			bmp * dog12 = read_bmp("dog12.bmp");
	 			alt_up_char_buffer_string(char_buffer, "dog12.bmp", 0, 30);

	 			bmp * dog13 = read_bmp("dog13.bmp");
	 			alt_up_char_buffer_string(char_buffer, "dog13.bmp", 0, 31);

	 		}
	 			//A skey
	 		} else if (buf[0]==0x23){ //D key


	 			bmp * dog20 = read_bmp("dog20.bmp");
	 			alt_up_char_buffer_string(char_buffer, "dog20.bmp", 0, 28);

	 			bmp * dog21 = read_bmp("dog21.bmp");
	 			alt_up_char_buffer_string(char_buffer, "dog21.bmp", 0, 29);

	 			bmp * dog22 = read_bmp("dog22.bmp");
	 			alt_up_char_buffer_string(char_buffer, "dog22.bmp", 0, 30);

	 			bmp * dog23 = read_bmp("dog23.bmp");
	 			alt_up_char_buffer_string(char_buffer, "dog23.bmp", 0, 31);

	 		}

	} else if (game_state == 5){
	 	//printf("ENTER GAME!\n\n");

	 	if (decode_mode == KB_BINARY_MAKE_CODE){
			if (buf[0]==0x29) { //spacebar

				c->jump_pressed = true;
				//TODO: jumpinh
			}
	 	}
		else if(decode_mode == KB_ASCII_MAKE_CODE){
	 		if(buf[0]==0x1C){ //A skey
				//printf("RSHIFT\n");
				c->color++;
				if(c->color == 2)
					c->color = 0;
				}else if (buf[0]==0x23){ //D key
					c->height = 21;
					c->ducking = true;
				}else if (buf[0]==0x4B){	//L Key
					printf("ESC\n");
					m->c->collide = 1;
				}
	 	}
		else if(decode_mode == KB_BREAK_CODE){
						if (buf[0]==0x29)
						//c->jump_released = true;
						c->jumping = false;
						if(buf[0] == 0x23 ){
							c->ducking = false;
						}
					}
 } else if (game_state == 6){
				//game_state == 6 is WIN
				if (decode_mode == KB_BINARY_MAKE_CODE){
						if (buf[0]==0x29) game_state =  1;
				}
	} else if (game_state == 7){
				//game_state == 7 is LOSE
				if (decode_mode == KB_BINARY_MAKE_CODE){
						if (buf[0]==0x29) game_state =  1;
				}

	} else if (game_state == 9){
			//game_state == 9 is NAME
			if (decode_mode == KB_BINARY_MAKE_CODE){
					if (buf[0]==0x29) {
						playername[ind] = '\0';
						alt_up_char_buffer_clear(char_buffer);
						game_state = 1;
					}

			} else if (decode_mode == KB_ASCII_MAKE_CODE){
					playername[ind] = decode(buf[0]);
					alt_up_char_buffer_draw(char_buffer, playername[ind], 35+ind, 32);
					ind++;
					if (ind == 12){
							alt_up_char_buffer_clear(char_buffer);
							game_state = 1;
					}
			}
	}
}

char decode(short int buffer){
	if (buffer == 0x1C){
		return 'a';
	} else if (buffer == 0x32){
		return 'b';
	} else if (buffer == 0x21){
		return 'c';
	} else if (buffer == 0x23){
		return 'd';
	} else if (buffer == 0x24){
		return 'e';
	} else if (buffer == 0x2B){
		return 'f';
	} else if (buffer == 0x34){
		return 'g';
	} else if (buffer == 0x33){
		return 'h';
	} else if (buffer == 0x43){
		return 'i';
	} else if (buffer == 0x3B){
		return 'j';
	} else if (buffer == 0x42){
		return 'k';
	} else if (buffer == 0x4B){
		return 'l';
	} else if (buffer == 0x3A){
		return 'm';
	} else if (buffer == 0x31){
		return 'n';
	} else if (buffer == 0x44){
		return 'o';
	} else if (buffer == 0x4D){
		return 'p';
	} else if (buffer == 0x15){
		return 'q';
	} else if (buffer == 0x2D){
		return 'r';
	} else if (buffer == 0x1B){
		return 's';
	} else if (buffer == 0x2C){
		return 't';
	} else if (buffer == 0x3C){
		return 'u';
	} else if (buffer == 0x2A){
		return 'v';
	} else if (buffer == 0x1D){
		return 'w';
	} else if (buffer == 0x22){
		return 'x';
	} else if (buffer == 0x35){
		return 'y';
	} else if (buffer == 0x1A){
		return 'z';
	}
	return ' ';
}
