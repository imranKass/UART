/**
 * @file UART1.c
 *
 * @brief Source code for the UART1 driver.
 *
 * This file contains the function definitions for the UART1 driver.
 *
 * @note For more information regarding the UART module, refer to the
 * Universal Asynchronous Receivers / Transmitters (UARTs) section
 * of the TM4C123GH6PM Microcontroller Datasheet.
 *   - Link: https://www.ti.com/lit/gpn/TM4C123GH6PM
 *
 * @note Assumes that the frequency of the system clock is 50 MHz.
 *
 * @author
 */

#include "UART1.h"

// Function to initialize the UART1 module
void UART1_Init(void) {
  // Enable the clock to the UART1 module by setting the R0 bit (Bit 0) in the RCGCUART register
  SYSCTL->RCGCUART |= 0x02;

  // Enable the clock to Port B by setting the R1 bit (Bit 1) in the RCGCGPIO register
  SYSCTL->RCGCGPIO |= 0x02;

  // Disable UART1 while configuring
  UART1->CTL &= ~0x0001;

  // Configure the UART1 module to use the system clock (50 MHz) divided by 16
  UART1->CTL &= ~0x0020;

  // Set the baud rate for 9600
  // BRD = (50,000,000) / (16 * 9600) = 325.5208333
  UART1->IBRD = 325;  // Integer part
  UART1->FBRD = 33;   // Fractional part

  // Configure the data length of the UART packet to be 8 bits
  UART1->LCRH |= 0x60;

  // Enable the Transmit FIFO and the Receive FIFO
  UART1->LCRH |= 0x10;

  // Select one stop bit to be transmitted at the end of a UART frame
  UART1->LCRH &= ~0x08;

  // Disable the parity bit by clearing the PEN bit
  UART1->LCRH &= ~0x02;

  // Enable the UART1 module after configuration
  UART1->CTL |= 0x01;

  // Select the alternate function for the PB1 and PB0 pins
  GPIOB->AFSEL |= 0x03;

  GPIOB->PCTL &= ~0x000000FF;  // Clear PCTL fields for PB1 and PB0
  GPIOB->PCTL |= 0x00000011;    // Set PCTL for UART function
  GPIOB->DEN |= 0x03;           // Enable digital I/O on PB1 and PB0
}

// Function to input a character from UART1
char UART1_Input_Character(void) {
  while ((UART1->FR & UART1_RECEIVE_FIFO_EMPTY_BIT_MASK) != 0);
  return (char)(UART1->DR & 0xFF);
}

// Function to output a character to UART1
void UART1_Output_Character(char data) {
  while ((UART1->FR & UART1_TRANSMIT_FIFO_FULL_BIT_MASK) != 0);
  UART1->DR = data;
}

// Function to input a string from UART1
void UART1_Input_String(char *buffer_pointer, uint16_t buffer_size) {
  int length = 0;
  char character = UART1_Input_Character();

  while (character != UART1_CR) {
    if (character == UART1_BS) {
      if (length) {
        buffer_pointer--;
        length--;
        UART1_Output_Character(UART1_BS);
      }
    } else if (length < buffer_size) {
      *buffer_pointer = character;
      buffer_pointer++;
      length++;
      UART1_Output_Character(character);
    }
    character = UART1_Input_Character();
  }
  *buffer_pointer = 0; // Null-terminate the string
}

// Function to output a string to UART1
void UART1_Output_String(char *pt) {
  while (*pt) {
    UART1_Output_Character(*pt);
    pt++;
  }
}

// Function to input an unsigned decimal number from UART1
uint32_t UART1_Input_Unsigned_Decimal(void) {
  uint32_t number = 0;
  uint32_t length = 0;
  char character = UART1_Input_Character();

  // Accepts until <enter> is typed
  while (character != UART1_CR) {
    if ((character >= '0') && (character <= '9')) {
      number = (10 * number) + (character - '0');
      length++;
      UART1_Output_Character(character);
    }
    else if ((character == UART1_BS) && length) {
      number /= 10;
      length--;
      UART1_Output_Character(character);
    }
    character = UART1_Input_Character();
  }

  return number;
}