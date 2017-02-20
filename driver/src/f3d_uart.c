/* f3d_uart.c ---
 *
 * Filename: f3d_uart.c
 * Part of: Lab5
 * Description: defines pin initialization functions and char I/O functions
 * Author: Zhongren Shao (shaoz), Erin Leonhard (eeleonha)
 * Created: 9/26/2014
 * Last-Updated: 9/26/2014
 *           By: Authors
 *
 */

#include <stm32f30x.h>

void f3d_uart_init(void) {
  // Initialization routines related to UART1

  GPIO_InitTypeDef GPIO_InitStructure;
  // enable clock
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

  // TX Pin initialization
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC,&GPIO_InitStructure);

  // RX Pin initialization
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC , &GPIO_InitStructure);

  // sets PC4 and PC5 to their alternate function
  GPIO_PinAFConfig(GPIOC,4,GPIO_AF_7);
  GPIO_PinAFConfig(GPIOC,5,GPIO_AF_7);

  // UART initialization
  USART_InitTypeDef USART_InitStructure;

  // UART1 clock enable
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

  // UART1 configuration
  USART_StructInit(&USART_InitStructure);
  USART_InitStructure.USART_BaudRate = 38400;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART1 ,&USART_InitStructure);
  USART_Cmd(USART1 , ENABLE);

}

// putschar from UART to serialT
int sendByte(char c) {
  // doesn't send data until USART is available
  while (USART_GetFlagStatus(USART1,USART_FLAG_TXE) == (uint16_t)RESET);

  // send data to UART
  USART_SendData(USART1, c);
  return 0;
}

// getschar from serialT to UART
char recvByte(void) {
  // ditto as sendByte
  while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == (uint16_t)RESET);

  // recv data from UART
  char c = USART_ReceiveData(USART1);
  return c;
}