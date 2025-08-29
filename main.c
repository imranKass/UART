/*
 * @file main.c
 *
 * @brief Main source code for the UART RGB LED control program.
 *
 * This program implements a UART command interpreter to control the RGB LED
 * on the TM4C123G LaunchPad.
 */

#include "TM4C123GH6PM.h"
#include "SysTick_Delay.h"
#include "UART0.h"
#include "GPIO.h"
#include <string.h>
#include <ctype.h>
#include "UART1.h"
// Function prototypes
void Display_Help_Menu(void);
void Process_Command(char *command);


int main(void)
{
    char input_buffer[64];
    
    // Initialize system components
    SysTick_Delay_Init();
    UART0_Init();
    RGB_LED_Init();
	UART1_Init();
   
    
    
    while(1)
    {
        // Prompt for command
			UART0_Output_String("Enter command: ");
        
        // Get user input
        UART0_Input_String(input_buffer, sizeof(input_buffer));
        UART0_Output_Newline();
        
        // Process the command
        Process_Command(input_buffer);
			
			UART1_Output_Character(0x55);
        
        // Short delay
        SysTick_Delay1ms(500);
    }
}

void Display_Help_Menu(void)
{
    UART0_Output_String("\r\nAvailable Commands:\r\n");
    UART0_Output_String("RED    - Turn on Red LED\r\n");
    UART0_Output_String("GREEN  - Turn on Green LED\r\n");
    UART0_Output_String("BLUE   - Turn on Blue LED\r\n");
    UART0_Output_String("Cycle    - RGB LED cycle\r\n");
   
}

void Process_Command(char *command)
{
    // Convert command to uppercase for case-insensitive comparison
    for(int i = 0; command[i]; i++){
        command[i] = toupper(command[i]);
    }

    // Process recognized commands
    if (strcmp(command, "RED") == 0)
    {
        RGB_SetColor(1, 0, 0);
        UART0_Output_String("RGB LED RED\r\n");
    }
    else if (strcmp(command, "GREEN") == 0)
    {
        RGB_SetColor(0, 1, 0);
        UART0_Output_String("RGB LED GREEN\r\n");
    }
    else if (strcmp(command, "BLUE") == 0)
    {
        RGB_SetColor(0, 0, 1);
        UART0_Output_String("RGB LED BLUE\r\n");
    }
    else if (strcmp(command, "CYCLE") == 0)
    {
        
        UART0_Output_String("RGB LED cycle\r\n");
    }
		
		else if(strcmp(command, "OFF") ==0)
		{
			RGB_Off();
		}
   
    else
    {
        UART0_Output_String("Invalid command. \r\n");
    }
}


void RGB_SetColor(uint8_t red, uint8_t green, uint8_t blue)
{
    // Clear all color bits (bits 1-3) first
    GPIOF->DATA &= ~0x0E;  // ~0x0E clears bits 1, 2, and 3
    
    // Set red if requested (bit 1)
    if(red) {
        GPIOF->DATA |= 0x02;
    }
    
    // Set blue if requested (bit 2)
    if(blue) {
        GPIOF->DATA |= 0x04;
    }
    
    // Set green if requested (bit 3)
    if(green) {
        GPIOF->DATA |= 0x08;
    }
}
void RGB_Off(void)
{
    // Turn off all RGB LEDs
    GPIOF->DATA &= ~0x0E;
}


