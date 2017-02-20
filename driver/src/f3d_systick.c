/* f3d_systick.c --- 
 * 
 * Filename: f3d_systick.c
 * Description: 
 * Author: Bryce Himebaugh
 * Maintainer: 
 * Created: Thu Nov 14 07:57:37 2013
 * Last-Updated: 
 *           By: 
 *     Update #: 0
 * Keywords: 
 * Compatibility: 
 * 
 */

/* Change log:
 * 11/14/14 Erin Leonhard (eeleonha) and Zhongren Shao (shaoz)
 * 
 */

/* Copyright (c) 2004-2007 The Trustees of Indiana University and 
 * Indiana University Research and Technology Corporation.  
 * 
 * All rights reserved. 
 * 
 * Additional copyrights may follow 
 */

/* Code: */

#include <f3d_systick.h>
#include <f3d_led.h> 
#include <f3d_usr_btn.h>
#include <f3d_nunchuk.h>
#include <queue.h>

volatile int systick_flag = 0;
volatile int flag = 0;
volatile int nunchuk_flag = 0;
volatile int move_flag = 0;
volatile int action_flag = 0;

extern queue_t txbuf, rxbuf;

static __IO uint32_t TimingDelay;

void f3d_systick_init(int inter_per_sec) {
  SysTick_Config(SystemCoreClock/inter_per_sec);
}

void systick_delay(int time) {
    TimingDelay = time;
    while (TimingDelay != 0);
}

int next_led(int curr_led) {
  if (curr_led == 15) 
    return 8;
  else
    return curr_led + 1;
}

void SysTick_Handler(void) {
  static int led = 8;
  if (TimingDelay != 0x00)
    TimingDelay--;
  if (!queue_empty(&txbuf)) {
    flush_uart();
  }
  //enqueue(&rxbuf, dequeue(&txbuf));
  f3d_led_off(led);
  led = next_led(led);  
  f3d_led_on(led);

  nunchuk_t nun_data;
  f3d_nunchuk_read(&nun_data);
  move_flag = readMove(&nun_data);
  action_flag = readBlockAct(&nun_data);
}
/* f3d_systick.c ends here */
