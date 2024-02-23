#include "stm8s.h"
#include "milis.h"
#include "stm8s_gpio.h"
#include "keyboard.h"

/* Private define ------------------------------------------------------------*/
#define R1_PORT GPIOC
#define R1_PIN GPIO_PIN_5

#define R2_PORT GPIOD
#define R2_PIN GPIO_PIN_3

#define R3_PORT GPIOD
#define R3_PIN GPIO_PIN_2

#define R4_PORT GPIOC
#define R4_PIN GPIO_PIN_7

#define C1_PORT GPIOC
#define C1_PIN GPIO_PIN_6

#define C2_PORT GPIOC
#define C2_PIN GPIO_PIN_4

#define C3_PORT GPIOD
#define C3_PIN GPIO_PIN_1

#define BUTTON_PORT GPIOB
#define BUTTON_PIN_1 GPIO_PIN_3
#define BUTTON_PIN_2 GPIO_PIN_4
#define BUTTON_PIN_3 GPIO_PIN_5
//---------------------------------------------------------------------------------------------------------

void keyboard_init()
{
    GPIO_Init(R1_PORT, R1_PIN, GPIO_MODE_IN_PU_NO_IT); // R1
    GPIO_Init(R2_PORT, R2_PIN, GPIO_MODE_IN_PU_NO_IT); // R2
    GPIO_Init(R3_PORT, R3_PIN, GPIO_MODE_IN_PU_NO_IT); // R3
    GPIO_Init(R4_PORT, R4_PIN, GPIO_MODE_IN_PU_NO_IT); // R4

    GPIO_Init(C1_PORT, C1_PIN, GPIO_MODE_OUT_OD_LOW_SLOW); // C1
    GPIO_Init(C2_PORT, C2_PIN, GPIO_MODE_OUT_OD_LOW_SLOW); // C2
    GPIO_Init(C3_PORT, C3_PIN, GPIO_MODE_OUT_OD_LOW_SLOW); // C3

    GPIO_Init(BUTTON_PORT, BUTTON_PIN_1, GPIO_MODE_OUT_PP_LOW_SLOW); // BUTTON 1
    GPIO_Init(BUTTON_PORT, BUTTON_PIN_2, GPIO_MODE_OUT_PP_LOW_SLOW); // BUTTON 2
    GPIO_Init(BUTTON_PORT, BUTTON_PIN_3, GPIO_MODE_OUT_PP_LOW_SLOW); // BUTTON 3
}

int get_keyboard_key() //Funkce, která při inputu uživatele returne hodnotu pro danou klávesu
{
    GPIO_WriteLow(C1_PORT, C1_PIN);
    GPIO_WriteHigh(C2_PORT, C2_PIN);
    GPIO_WriteHigh(C3_PORT, C3_PIN);

    if (GPIO_ReadInputPin(R1_PORT, R1_PIN) == RESET)
    { // R1
        return 1;
    }

    if (GPIO_ReadInputPin(R2_PORT, R2_PIN) == RESET)
    { // R2
        return 4;
    }

    if (GPIO_ReadInputPin(R3_PORT, R3_PIN) == RESET)
    { // R3
        return 7;
    }

    if (GPIO_ReadInputPin(R4_PORT, R4_PIN) == RESET)
    { // R4
        //"*"
        return 10;
    }

    GPIO_WriteHigh(C1_PORT, C1_PIN);
    GPIO_WriteLow(C2_PORT, C2_PIN);
    GPIO_WriteHigh(C3_PORT, C3_PIN);

    if (GPIO_ReadInputPin(R1_PORT, R1_PIN) == RESET)
    {
        // x=2;
        return 2;
    }

    if (GPIO_ReadInputPin(R2_PORT, R2_PIN) == RESET)
    {
        // x=5;
        return 5;
    }

    if (GPIO_ReadInputPin(R3_PORT, R3_PIN) == RESET)
    {
        // x=8;
        return 8;
    }

    if (GPIO_ReadInputPin(R4_PORT, R4_PIN) == RESET)
    {
        //"0"
        return 11;
    }

    GPIO_WriteHigh(C1_PORT, C1_PIN);
    GPIO_WriteHigh(C2_PORT, C2_PIN);
    GPIO_WriteLow(C3_PORT, C3_PIN);

    if (GPIO_ReadInputPin(R1_PORT, R1_PIN) == RESET)
    {
        // x=3;
        return 3;
    }

    if (GPIO_ReadInputPin(R2_PORT, R2_PIN) == RESET)
    {
        // x=6;
        return 6;
    }

    if (GPIO_ReadInputPin(R3_PORT, R3_PIN) == RESET)
    {
        // x=9;
        return 9;
    }

    if (GPIO_ReadInputPin(R4_PORT, R4_PIN) == RESET)
    {
        //"#"
        return 12;
    }
    return 100;
}