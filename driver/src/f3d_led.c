/* Names: Erin Leonhard (eeleonha) Zhongren Shao (shaoz)
 * 
 * Filename: f3d_led.c
 * Part of: Lab4
 * Date Created: 9/19/14
 * Last Modified: 9/19/14
 *
 *
 */


#include <stm32f30x.h>
#include <stm32f30x_gpio.h>
#include <stm32f30x_rcc.h>
#include <f3d_led.h>

// array for all GPIO_Pins so that int led can be used as index
int pins[16] = {GPIO_Pin_0,
		GPIO_Pin_1,
		GPIO_Pin_2,
		GPIO_Pin_3,
		GPIO_Pin_4,
		GPIO_Pin_5,
		GPIO_Pin_6,
		GPIO_Pin_7,
		GPIO_Pin_8,
		GPIO_Pin_9,
		GPIO_Pin_10,
		GPIO_Pin_11,
		GPIO_Pin_12,
		GPIO_Pin_13,
		GPIO_Pin_14,
		GPIO_Pin_15};

//intializes the port and pins for the 8 leds on the board
void f3d_led_init(void) {
  // enables clock signal for indiv gpio port
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);
  
  // declare a GPIO_InitTypeDef object called GPIO_InitStructure
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_StructInit(&GPIO_InitStructure);
  // inits values in GPIO_InitStructure obj
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  // applies a config to a specific port pin
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
}

/*Turns on the appropriate led as specified by the parameter.*/
void f3d_led_on(int led) {
  GPIOE->BSRR = pins[led];
  return;
}

/*Turns off the approiate led as specified by the parameter*/ 
void f3d_led_off(int led) {
  GPIOE->BRR = pins[led];
  return;
} 

/*Turns on all LEDs*/
void f3d_led_all_on(void) {
  int i = 8;
  for (i = 8; i < 16; i++) {
    GPIOE->BSRR = pins[i];
  }
  return;
} 

/*Turns off all LEDs*/
void f3d_led_all_off(void) {
  int i = 8;
  for (i = 8; i < 16; i++) {
    GPIOE->BRR = pins[i];
  }
  return;
} 

/* led.c ends here */

