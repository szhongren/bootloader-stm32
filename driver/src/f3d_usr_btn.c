#include <stm32f30x.h>
#include <stm32f30x_gpio.h>
#include <stm32f30x_rcc.h>


/*Initialization of the UserButton*/
void f3d_usr_btn_init(void){
  
  // ditto as leds, except with GPIOA instead of GPIOE
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  
  // same as leds, inits values in the obj
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;


}

int button_read(void){
  // looks at GPIOA pin 0 and gives the state
  return GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0);
}
