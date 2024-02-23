#include "stm8s.h"

#ifndef _MAX7219_H_
#define _MAX7219_H_

#define DISPLAY_SIZE 8
#define DISPLAY_COUNT 4

void max7219_init(void);
void max7219_clear(void);
void max7219_on(void);
void max7219_off(void);
void max7219_set_intensity(uint8_t intensity);
void max7219_posli(uint8_t adresa, uint8_t *data);
int leftRotate(int n, unsigned int d, int *overflow);
char convertCharToIndex(char znak);
char special_characters(char znak);

#endif