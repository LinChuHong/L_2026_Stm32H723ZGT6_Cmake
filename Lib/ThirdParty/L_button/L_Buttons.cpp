#include "button/multi_button.h"
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include "main.h"

// Button instances
static Button btn1, btn2;


// Hardware abstraction layer function
// This simulates reading GPIO states
uint8_t read_button_gpio(uint8_t button_id)
{
    switch (button_id) {
        case 1:
            return HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin);
        case 2:
            return HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin);
        default:
            return 0;
    }
}

// Callback functions for button 1
void btn1_single_click_handler(Button* btn, void* user_data)
{
    (void)btn; 
    (void)user_data;
    if (btn->button_id == 1)
    {
        printf("[BTN%d] Single Click\n",btn->button_id);
    }
    else if (btn->button_id == 2)
    {
        printf("[BTN%d] Single Click\n",btn->button_id);
    }
}

void btn1_double_click_handler(Button* btn, void* user_data)
{
    (void)btn; (void)user_data;
    printf("[BTN1] Double Click\n");
}

void btn1_long_press_start_handler(Button* btn, void* user_data)
{
    (void)btn; (void)user_data;
    printf("[BTN1] Long Press Start\n");
}

void btn1_long_press_hold_handler(Button* btn, void* user_data)
{
    (void)btn; (void)user_data;
    printf("[BTN1] Long Press Hold...\n");
}

void btn1_press_repeat_handler(Button* btn, void* user_data)
{
    (void)user_data;
    printf("[BTN1] Press Repeat (count: %d)\n", button_get_repeat_count(btn));
}

// Callback functions for button 2
void btn2_single_click_handler(Button* btn, void* user_data)
{
    (void)btn; (void)user_data;
    printf("[BTN2] Single Click\n");
}

void btn2_double_click_handler(Button* btn, void* user_data)
{
    (void)btn; (void)user_data;
    printf("[BTN2] Double Click\n");
}

void btn2_press_down_handler(Button* btn, void* user_data)
{
    (void)btn; (void)user_data;
    printf("[BTN2] Press Down\n");
}

void btn2_press_up_handler(Button* btn, void* user_data)
{
    (void)btn; (void)user_data;
    printf("[BTN2] Press Up\n");
}

// Initialize buttons
void buttons_init(void)
{
    // Initialize button 1 (active high for simulation)
    button_init(&btn1, read_button_gpio, 1, 1);

    // Attach event handlers for button 1
    button_attach(&btn1, BTN_SINGLE_CLICK, btn1_single_click_handler, (char*)"btn1");
    button_attach(&btn1, BTN_DOUBLE_CLICK, btn1_double_click_handler, NULL);
    button_attach(&btn1, BTN_LONG_PRESS_START, btn1_long_press_start_handler, NULL);
    button_attach(&btn1, BTN_LONG_PRESS_HOLD, btn1_long_press_hold_handler, NULL);
    button_attach(&btn1, BTN_PRESS_REPEAT, btn1_press_repeat_handler, NULL);

    // Initialize button 2 (active high for simulation)
    button_init(&btn2, read_button_gpio, 1, 2);

    // // Attach event handlers for button 2
    button_attach(&btn2, BTN_SINGLE_CLICK, btn1_single_click_handler, (char*)"btn2");
    button_attach(&btn2, BTN_DOUBLE_CLICK, btn1_double_click_handler, NULL);
    button_attach(&btn2, BTN_LONG_PRESS_START, btn1_long_press_start_handler, NULL);
    button_attach(&btn2, BTN_LONG_PRESS_HOLD, btn1_long_press_hold_handler, NULL);
    button_attach(&btn2, BTN_PRESS_REPEAT, btn1_press_repeat_handler, NULL);

    // Start button processing
    button_start(&btn1);
    button_start(&btn2);
}




