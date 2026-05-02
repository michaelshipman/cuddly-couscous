#include "main.h"
#include "stm32wlxx.h"
#include "stm32wlxx_ll_dma.h"
#include "stm32wlxx_ll_dmamux.h"
#include "system.h"
#include "usart.h"
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char test_str[] = "hello world!\r\n";

void main(void) {

  /* System interrupt init*/
  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* SysTick_IRQn interrupt configuration */
  NVIC_SetPriority(SysTick_IRQn,
                   NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 15, 0));

  init_sys_clock();

  MX_LPUART1_UART_Init();

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMAMUX1);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

  LL_DMA_ConfigTransfer(DMA1, LL_DMA_CHANNEL_1,
                        LL_DMA_DIRECTION_MEMORY_TO_PERIPH | LL_DMA_MODE_NORMAL |
                            LL_DMA_PERIPH_NOINCREMENT |
                            LL_DMA_MEMORY_INCREMENT | LL_DMA_PDATAALIGN_BYTE |
                            LL_DMA_MDATAALIGN_BYTE | LL_DMA_PRIORITY_LOW);

  LL_DMAMUX_SetRequestID(DMAMUX1, LL_DMAMUX_CHANNEL_0,
                         LL_DMAMUX_REQ_LPUART1_TX);

  LL_DMA_ConfigAddresses(
      DMA1, LL_DMA_CHANNEL_1, (uint32_t)test_str,
      LL_LPUART_DMA_GetRegAddr(LPUART1, LL_LPUART_DMA_REG_DATA_TRANSMIT),
      LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

  LL_LPUART_EnableIT_TC(LPUART1);
  LL_LPUART_EnableDMAReq_TX(LPUART1);

  while (1) {

    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, (uint32_t)14);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);

    while (!LL_DMA_IsActiveFlag_TC1(DMA1))
      ;

    while (!LL_LPUART_IsActiveFlag_TC(LPUART1))
      ;

    LL_LPUART_ClearFlag_TC(LPUART1);
    LL_DMA_ClearFlag_TC1(DMA1);
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_1);
  }
}
