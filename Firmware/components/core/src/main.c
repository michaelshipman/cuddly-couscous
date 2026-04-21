#include "main.h"
#include "stm32wlxx.h"
#include "system.h"
#include "usart.h"
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void main(void) {

  /* System interrupt init*/
  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* SysTick_IRQn interrupt configuration */
  NVIC_SetPriority(SysTick_IRQn,
                   NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 15, 0));

  init_sys_clock();

  MX_LPUART1_UART_Init();

  int i = 0;

  while (1) {
    LL_LPUART_TransmitData8(LPUART1, 'm');

    i++;
  }
}
