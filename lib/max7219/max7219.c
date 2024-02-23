#include "stm8s.h"
#include "stm8s_gpio.h"
#include "max7219.h"

//* Define -------------------------------------------------------------------*/
#define CLK_GPIO GPIOD
#define CLK_PIN GPIO_PIN_4
#define DATA_GPIO GPIOD
#define DATA_PIN GPIO_PIN_6
#define CS_GPIO GPIOD
#define CS_PIN GPIO_PIN_5

#define CLK_HIGH GPIO_WriteHigh(CLK_GPIO, CLK_PIN)
#define CLK_LOW GPIO_WriteLow(CLK_GPIO, CLK_PIN)
#define DATA_HIGH GPIO_WriteHigh(DATA_GPIO, DATA_PIN)
#define DATA_LOW GPIO_WriteLow(DATA_GPIO, DATA_PIN)
#define CS_HIGH GPIO_WriteHigh(CS_GPIO, CS_PIN)
#define CS_LOW GPIO_WriteLow(CS_GPIO, CS_PIN)
/* Private macro -------------------------------------------------------------*/
#define DECODE_MODE 9
#define INTENSITY 10
#define SCAN_LIMIT 11
#define SHUTDOWN 12
#define DISPLAY_TEST 15

// argumenty pro SHUTDOWN
#define DISPLAY_ON 1
#define DISPLAY_OFF 0

// argumenty pro DISPLAY_TEST
#define DISPLAY_TEST_ON 1
#define DISPLAY_TEST_OFF 0

// argumenty pro DECODE_MOD
#define DECODE_ALL 0xff
#define DECODE_NONE 0

void max7219_posli(uint8_t adresa, uint8_t *data)
{
    uint8_t i = 0;
    uint8_t pomocna_promenna;
    CS_LOW;

    for (i = 0; i < 4; i++) //viz. elektromys.eu
    {
        pomocna_promenna = 0b10000000;
        CLK_LOW;
        while (pomocna_promenna)
        {
            if (pomocna_promenna & adresa)
            {
                DATA_HIGH;
            }
            else
            {
                DATA_LOW;
            }

            CLK_HIGH;
            pomocna_promenna = pomocna_promenna >> 1;
            CLK_LOW;
        }

        pomocna_promenna = 0b10000000;
        while (pomocna_promenna)
        {
            if (pomocna_promenna & data[i])
            {
                DATA_HIGH;
            }
            else
            {
                DATA_LOW;
            }
            CLK_HIGH;
            pomocna_promenna = pomocna_promenna >> 1;
            CLK_LOW;
        }
    }

    CS_HIGH;
}

void setForAllDisplays(uint8_t adress, uint8_t command)
{
    uint8_t data[DISPLAY_COUNT];
    for (uint8_t i = 0; i < DISPLAY_COUNT; i++)
    {
        data[i] = command;
    }
    max7219_posli(adress, data);
}

void max7219_init(void)
{
    uint8_t data[4];
    uint8_t i = 0;
    GPIO_Init(CS_GPIO, CS_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(CLK_GPIO, CLK_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(DATA_GPIO, DATA_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    CS_HIGH;

    // -----------------------------------------------------
    setForAllDisplays(DISPLAY_TEST, DISPLAY_TEST_OFF);
    setForAllDisplays(DECODE_MODE, 0);
    setForAllDisplays(SCAN_LIMIT, 7);
    max7219_off();
    // -----------------------------------------------------
}

void max7219_on(void)
{
    setForAllDisplays(SHUTDOWN, DISPLAY_ON);
}

void max7219_off(void)
{
    setForAllDisplays(SHUTDOWN, DISPLAY_OFF);
}

void max7219_set_intensity(uint8_t intensity)
{
    setForAllDisplays(INTENSITY, intensity);
}

int leftRotate(int n, unsigned int d, int *overflow)
{
    *overflow = (n >> (DISPLAY_SIZE - d));
    return (n << d);
}

char convertCharToIndex(char znak)
{
    return znak - 'A';
}

// Změna znaku za jiný znak vzhledek k ASCI tabulce
char special_characters(char znak)
{
    switch (znak)
    {
    case ' ':
        return '[';
    case '1':
        return ']';
    case '2':
        return '^';
    case '3':
        return '_';
    case '4':
        return '`';
    case '5':
        return 'a';
    case '6':
        return 'b';
    case '7':
        return 'c';
    case '8':
        return 'd';
    case '9':
        return 'e';

    case '+': // šipka NAHORU
        return 'f';
    case 'e': // šipka DOLŮ
        return 'g';
    case 's': // šipka DOPRAVA
        return 'h';
    case 'c': // šipka DOLEVA
        return 'i';
    case '>':
        return 'j';
    case '<':
        return 'k';
    case '-':
        return 'l';
    default:
        return znak;
    }
}
