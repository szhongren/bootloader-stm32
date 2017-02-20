#include "utils.h"

void btPinInit(void) {
  GPIO_InitTypeDef GPIO_InitStructure;
  // PB0 pin used as bootloader check pin
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(GPIOC,&GPIO_InitStructure);
}

uint8_t bNOT8(uint8_t val) {
  return ((~val) & 0xff);
}

uint16_t bNOT16(uint16_t val) {
  return ((~val) & 0xffff);
}

uint32_t bNOT32(uint32_t val) {
  return ((~val) & 0xffffffff);
}

uint8_t readFlash8(uint32_t addr) {
  return *(uint8_t *)addr;
}

uint16_t readFlash16(uint32_t addr){
  return *(uint16_t *)addr;
}

uint32_t readFlash32(uint32_t addr){
  return *(uint32_t *)addr;
}

uint32_t revWord(uint32_t Word) {
  uint32_t rev = 0;
  rev |= (Word & 0x000000FF) << 24;
  rev |= (Word & 0x0000FF00) << 8;
  rev |= (Word & 0x00FF0000) >> 8;
  rev |= (Word & 0xFF000000) >> 24;
  return rev;
}

uint16_t revHalfWord(uint16_t hWord) {
  uint16_t rev = 0;
  rev |= (hWord & 0x00FF) << 8;
  rev |= (hWord & 0xFF00) >> 8;
  return rev;
}

void sleep(int cycles) {
  while (cycles--) {
    asm("nop");
  }
}

void ack() {
  sendByte(ACK);

}

void nak() {
  sendByte(NAK);
}

uint8_t eraseRAM(void) {
  uint32_t addr = SRAM_BEGIN + 0x00000400;
  while (addr <= SRAM_END) {
    *(uint32_t *)addr = 0x00000000;
    addr += 4;
  }
  return 1;
}

uint32_t recvAddr(uint8_t *chksm) {
  uint8_t i;
  uint32_t addr;
  for (i = 0; i < 4; i++) { // get addr start, msb first
    uint8_t buf = recvByte();
    *chksm ^= buf;
    addr |= buf << ((3 - i) * 8);
  }
  return addr;
}

uint8_t chkCmdComp(uint8_t cmd) {
  uint8_t cmdComp = recvByte();
  return cmd == bNOT8(cmdComp);
}

uint8_t inRange(uint32_t addr, uint8_t type) {
	if (type == CMD_READMEM) { // readMem
		return (addr >= 0x08000000 && addr < 0x08040000) || (addr >= 0x1fffd800 && addr < 0x1ffff800) || (addr >= 0x1ffff800 && addr < 0x1ffff810) || (addr >= 0x20001400 && addr < 0x2000a000);
	} else if (type == CMD_GO) {
		return (addr >= 0x08000000 && addr < 0x08040000) || (addr >= 0x20001400 && addr < 0x2000a000);
  } else if (type == CMD_WRITEMEM) {
		return (addr >= 0x08000000 && addr < 0x08040000) || (addr >= 0x20001400 && addr < 0x2000a000);
	} else if (type == CMD_ERASE) {
		return 1;
	} else if (type == CMD_WRITE_PROTECT) {
		return 1;
	} else {
		return 0;
  }
}

uint32_t addrPageStart(uint32_t addr) {
  uint32_t offset = addr % PAGE_SIZE;
  return addr - offset;
}

void disableAllIRQ() {

  // NVIC_DisableIRQ(NonMaskableInt_IRQn);
  // NVIC_DisableIRQ(MemoryManagement_IRQn);
  // NVIC_DisableIRQ(BusFault_IRQn);
  // NVIC_DisableIRQ(UsageFault_IRQn);
  // NVIC_DisableIRQ(SVCall_IRQn);
  //NVIC_DisableIRQ(DebugMonitor_IRQn);
  // NVIC_DisableIRQ(PendSV_IRQn);
  // NVIC_DisableIRQ(SysTick_IRQn);
  // above might potentially not be needed
  NVIC_DisableIRQ(WWDG_IRQn);
  NVIC_DisableIRQ(PVD_IRQn);
  NVIC_DisableIRQ(TAMPER_STAMP_IRQn);
  NVIC_DisableIRQ(RTC_WKUP_IRQn);
  NVIC_DisableIRQ(FLASH_IRQn);
  NVIC_DisableIRQ(RCC_IRQn);
  NVIC_DisableIRQ(EXTI0_IRQn);
  NVIC_DisableIRQ(EXTI1_IRQn);
  NVIC_DisableIRQ(EXTI2_TS_IRQn);
  NVIC_DisableIRQ(EXTI3_IRQn);
  NVIC_DisableIRQ(EXTI4_IRQn);
  NVIC_DisableIRQ(DMA1_Channel1_IRQn);
  NVIC_DisableIRQ(DMA1_Channel2_IRQn);
  NVIC_DisableIRQ(DMA1_Channel3_IRQn);
  NVIC_DisableIRQ(DMA1_Channel4_IRQn);
  NVIC_DisableIRQ(DMA1_Channel5_IRQn);
  NVIC_DisableIRQ(DMA1_Channel6_IRQn);
  NVIC_DisableIRQ(DMA1_Channel7_IRQn);
  NVIC_DisableIRQ(ADC1_2_IRQn);
  NVIC_DisableIRQ(USB_HP_CAN1_TX_IRQn);
  NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
  NVIC_DisableIRQ(CAN1_RX1_IRQn);
  NVIC_DisableIRQ(CAN1_SCE_IRQn);
  NVIC_DisableIRQ(EXTI9_5_IRQn);
  NVIC_DisableIRQ(TIM1_BRK_TIM15_IRQn);
  NVIC_DisableIRQ(TIM1_UP_TIM16_IRQn);
  NVIC_DisableIRQ(TIM1_TRG_COM_TIM17_IRQn);
  NVIC_DisableIRQ(TIM1_CC_IRQn);
  NVIC_DisableIRQ(TIM2_IRQn);
  NVIC_DisableIRQ(TIM3_IRQn);
  NVIC_DisableIRQ(TIM4_IRQn);
  NVIC_DisableIRQ(I2C1_EV_IRQn);
  NVIC_DisableIRQ(I2C1_ER_IRQn);
  NVIC_DisableIRQ(I2C2_EV_IRQn);
  NVIC_DisableIRQ(I2C2_ER_IRQn);
  NVIC_DisableIRQ(SPI1_IRQn);
  NVIC_DisableIRQ(SPI2_IRQn);
  NVIC_DisableIRQ(USART1_IRQn);
  NVIC_DisableIRQ(USART2_IRQn);
  NVIC_DisableIRQ(USART3_IRQn);
  NVIC_DisableIRQ(EXTI15_10_IRQn);
  NVIC_DisableIRQ(RTC_Alarm_IRQn);
  NVIC_DisableIRQ(USBWakeUp_IRQn);
  NVIC_DisableIRQ(TIM8_BRK_IRQn);
  NVIC_DisableIRQ(TIM8_UP_IRQn);
  NVIC_DisableIRQ(TIM8_TRG_COM_IRQn);
  NVIC_DisableIRQ(TIM8_CC_IRQn);
  NVIC_DisableIRQ(ADC3_IRQn);
  NVIC_DisableIRQ(SPI3_IRQn);
  NVIC_DisableIRQ(UART4_IRQn);
  NVIC_DisableIRQ(UART5_IRQn);
  NVIC_DisableIRQ(TIM6_DAC_IRQn);
  NVIC_DisableIRQ(TIM7_IRQn);
  NVIC_DisableIRQ(DMA2_Channel1_IRQn);
  NVIC_DisableIRQ(DMA2_Channel2_IRQn);
  NVIC_DisableIRQ(DMA2_Channel3_IRQn);
  NVIC_DisableIRQ(DMA2_Channel4_IRQn);
  NVIC_DisableIRQ(DMA2_Channel5_IRQn);
  NVIC_DisableIRQ(ADC4_IRQn);
  NVIC_DisableIRQ(COMP1_2_3_IRQn);
  NVIC_DisableIRQ(COMP4_5_6_IRQn);
  NVIC_DisableIRQ(COMP7_IRQn);
  NVIC_DisableIRQ(USB_HP_IRQn);
  NVIC_DisableIRQ(USB_LP_IRQn);
  NVIC_DisableIRQ(USBWakeUp_RMP_IRQn);
  NVIC_DisableIRQ(FPU_IRQn);
}


void bootToUser(uint32_t addr) {
	uint32_t appStack;
  uint32_t appEntry;

  SysTick->CTRL = 0; // turn off clk

  disableAllIRQ(); // disable all interrupts, not that any should be active

  /* Get the application stack pointer (First entry in the application vector table) */
	appStack = (uint32_t) *((__IO uint32_t*)addr);

  // Get the application entry point (Second entry in the application vector table)
	appEntry = *(__IO uint32_t*) (addr + 4);

	/* Reconfigure vector table offset register to match the application location */
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, addr - FLASH_BEGIN);

	/* Set the application stack pointer */
	__set_MSP(appStack);
  /* Start the application */
  jumpto(appEntry);
}
