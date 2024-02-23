/* Includes ------------------------------------------------------------------*/
#include "stm8s_it.h" /* SDCC patch: required by SDCC for interrupts */
#include "stm8s.h"
#include "max_font.h"
#include "string.h"
#include "keyboard.h"
#include "max7219.h"
#include "texts.h"
#include "milis.h"
#include "lights.h"

/* Private define ------------------------------------------------------------*/
#define INT_LED_PIN GPIO_PIN_5
#define INT_LED_PORT GPIOC

#define BLUE_LED_PIN GPIO_PIN_1
#define BLUE_LED_PORT GPIOA

#define RED_LED_PIN GPIO_PIN_2
#define RED_LED_PORT GPIOA

/* Private variables ---------------------------------------------------------*/
char text[TEXT_SIZE];
bool text_changed = FALSE;
uint8_t brightness = 0x08;
bool display_on = TRUE;
uint16_t delay_speed = 200;
int8_t light_mode = -1;

/* Private function prototypes -----------------------------------------------*/
void fix_text(char *text);
void keyboard_logic(void);
void lights_logic(void);

//--------------------------------------------------------------------------------------------
void main(void)
{
    GPIO_Init(INT_LED_PORT, INT_LED_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(RED_LED_PORT, RED_LED_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(BLUE_LED_PORT, BLUE_LED_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);

    keyboard_init();
    max7219_init();
    init_milis();

    uint8_t data[DISPLAY_COUNT];
    uint16_t time = millis();

    while (strlen(text) == 0)
    {
        keyboard_logic();
        // pokud rozdil mezi predchozim bliknutim a stavajicim casem > n blikame
        if (millis() - time > 5)
        {
            lights_logic();
            // nastavime cas bliknuti na aktualni cas
            time = millis();
        }
    }

    max7219_on();
    max7219_set_intensity(brightness);

    while (TRUE)
    {
        if (text_changed)
        {
            fix_text(text);
            text_changed = FALSE;
        }

        uint8_t delkaTextu = strlen(text);
        int8_t zbytekTextu = delkaTextu - 4;

        if (zbytekTextu < 0)
        {
            zbytekTextu = 0;
        }

        // cyklus, který prochází všemi charektery + nastavuje offset textu
        for (uint8_t znakOffset = 0; znakOffset <= zbytekTextu; znakOffset++)
        {
            if (text_changed)
            {
                break;
            }

            char znak0 = text[znakOffset];
            char znak1 = text[znakOffset + 1];
            char znak2 = text[znakOffset + 2];
            char znak3 = text[znakOffset + 3];
            char znak4 = text[znakOffset + 4];
            
            // pokud je znak4 mimo dosah, nastaví správný počáteční charakter textu
            uint8_t lastCharIndex = delkaTextu - 1;
            if (znakOffset + DISPLAY_COUNT > lastCharIndex)
            {
                // protože přidáváme mezeru, nevolíme počáteční charakter na pozici 0, ale -1
                znak4 = text[DISPLAY_COUNT - 1];
            }

            for (uint8_t posun = 0; posun < DISPLAY_SIZE; posun++)
            {
                // pokud se v průběhu programu načte nový input z klávesnice, cyklus se přeruší a začne s novým textem
                if (text_changed)
                {
                    break;
                }
                for (uint8_t radek = 1; radek < 9; radek++)
                {
                    int overflow0_unused;
                    int overflow1;
                    int overflow2;
                    int overflow3;
                    int overflow4;
                    leftRotate(IMAGES[convertCharToIndex(znak4)][radek - 1], posun, &overflow4);

                    data[3] = overflow4 | leftRotate(IMAGES[convertCharToIndex(znak3)][radek - 1], posun, &overflow3);
                    data[2] = overflow3 | leftRotate(IMAGES[convertCharToIndex(znak2)][radek - 1], posun, &overflow2);
                    data[1] = overflow2 | leftRotate(IMAGES[convertCharToIndex(znak1)][radek - 1], posun, &overflow1);
                    data[0] = overflow1 | leftRotate(IMAGES[convertCharToIndex(znak0)][radek - 1], posun, &overflow0_unused);

                    max7219_posli(radek, data);
                }
                for (unsigned int j = 0; j < delay_speed; j++)
                {
                    // voláme cyklus "keyboard_logic()" každý druhý cyklus pro zrychlení cyklu
                    if (j % 4)
                    {
                        keyboard_logic();
                    }
                    if (j == delay_speed - 1)
                    {
                        lights_logic();
                    }
                }
            }
        }
    }
}
//--------------------------------------------------------------------------------------------
void keyboard_logic()
{
    // založíme nové proměnné "previous_key" & "current_key", abychom case přepínali pouze jednou pro jeden input
    static int8_t previous_key;
    int current_key = get_keyboard_key();

    // kontrola, zda input je aktivní, nebo byl aktivní
    // viz. http://www.elektromys.eu/clanky/stm8_4_gpio2/clanek.html
    if (previous_key == 100 && current_key != previous_key)
    {
        switch (current_key) // přepínáme módy na základě inputu z klávesnice
        {
        case 1:
            strncpy(text, texts[0], TEXT_SIZE);
            text_changed = TRUE;
            break;

        case 2:
            strncpy(text, texts[1], TEXT_SIZE);
            text_changed = TRUE;
            break;

        case 3:
            strncpy(text, texts[2], TEXT_SIZE);
            text_changed = TRUE;
            break;

        case 4:
            light_mode = 1;
            break;

        case 5:
            light_mode = 2;
            break;

        case 6:
            light_mode = 3;
            break;

        case 7:
            strncpy(text, texts[3], TEXT_SIZE);
            text_changed = TRUE;
            light_mode = 4;
            break;

        case 8: // Reset pro LED pásky
            light_mode = 6;
            break;

        case 9:
            strncpy(text, texts[4], TEXT_SIZE);
            text_changed = TRUE;
            light_mode = 5;
            break;

        case 10: // Nastavení jasu displeje
            brightness += 0x05;
            if (brightness > 0x0F)
            {
                brightness = 0x00;
            }
            max7219_set_intensity(brightness);
            break;

        case 11: // Zapínání/Vypínání displeje
            switch (display_on)
            {
            case TRUE:
                max7219_off();
                display_on = FALSE;
                break;
            case FALSE:
                max7219_on();
                display_on = TRUE;
                // we want to refresh the text
                text_changed = TRUE;
                break;
            }
            break;

        case 12: // Nastavení rychlosti textu na displeji
            delay_speed += 200;

            if (delay_speed > 400) // Max. rychlost
            {
                delay_speed = 10; // Min. rychlost
            }
            break;

        default:
            break;
        }
    }
    previous_key = current_key;
}
//--------------------------------------------------------------------------------------------
void lights_logic(void)
{
    static uint8_t light_index = 0;

    if (light_mode == -1)
    {
        return;
    }

    bool redLight = lights[light_mode][0][light_index]; // První 16 stavů pro červené diody
    bool blueLight = lights[light_mode][1][light_index]; // Druhých 16 stavů pro modré diody

    if (redLight)
    {
        GPIO_WriteHigh(RED_LED_PORT, RED_LED_PIN);
    }
    else
    {
        GPIO_WriteLow(RED_LED_PORT, RED_LED_PIN);
    }

    if (blueLight)
    {
        GPIO_WriteHigh(BLUE_LED_PORT, BLUE_LED_PIN);
    }
    else
    {
        GPIO_WriteLow(BLUE_LED_PORT, BLUE_LED_PIN);
    }

    light_index++;
    if (light_index > LIGHTS_COUNT)
    {
        light_index = 0; // Jakmile dojdeme k poslednímu stavu, vrátíme se k prvnímu stavu
    }
}
//--------------------------------------------------------------------------------------------
void fix_text(char *text)
{
    // změna znaku " " za "[" vzhledem k pořadí v ASCI Tabulce vůci "max_font.h"
    for (int i = 0; text[i] != '\0'; i++)
    {
        text[i] = special_characters(text[i]);
    }
    // přidání mezery a <DISPLAY_COUNT-1> od začátku do konce textu
    char prvni3znaky[DISPLAY_COUNT] = {'['};
    strncpy(prvni3znaky + 1, text, DISPLAY_COUNT - 1);
    strcat(text, prvni3znaky);
}
//--------------------------------------------------------------------------------------------
#ifdef USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *   where the assert_param error has occurred.
 * @param file: pointer to the source file name
 * @param line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {
    }
}
#endif

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
