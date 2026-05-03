#include "usart.h"

#include "stm32wlxx_ll_adc.h"
#include "stm32wlxx_ll_bus.h"
#include "stm32wlxx_ll_cortex.h"
#include "stm32wlxx_ll_dma.h"
#include "stm32wlxx_ll_exti.h"
#include "stm32wlxx_ll_gpio.h"
#include "stm32wlxx_ll_i2c.h"
#include "stm32wlxx_ll_lptim.h"
#include "stm32wlxx_ll_lpuart.h"
#include "stm32wlxx_ll_pwr.h"
#include "stm32wlxx_ll_rcc.h"
#include "stm32wlxx_ll_system.h"
#include "stm32wlxx_ll_tim.h"
#include "stm32wlxx_ll_utils.h"

#include <string.h>

static char lpuart1_buffer[1024] = {0};
static uint8_t tx_queued = 0;

void lpuart1_init(void) {

  LL_LPUART_InitTypeDef LPUART_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_RCC_SetLPUARTClockSource(LL_RCC_LPUART1_CLKSOURCE_PCLK1);

  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_LPUART1);

  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
  GPIO_InitStruct.Pin = LL_GPIO_PIN_1 | LL_GPIO_PIN_0;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_8;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  LPUART_InitStruct.PrescalerValue = LL_LPUART_PRESCALER_DIV1;
  LPUART_InitStruct.BaudRate = 115200;
  LPUART_InitStruct.DataWidth = LL_LPUART_DATAWIDTH_8B;
  LPUART_InitStruct.StopBits = LL_LPUART_STOPBITS_1;
  LPUART_InitStruct.Parity = LL_LPUART_PARITY_NONE;
  LPUART_InitStruct.TransferDirection = LL_LPUART_DIRECTION_TX_RX;
  LPUART_InitStruct.HardwareFlowControl = LL_LPUART_HWCONTROL_NONE;
  LL_LPUART_Init(LPUART1, &LPUART_InitStruct);
  LL_LPUART_SetTXFIFOThreshold(LPUART1, LL_LPUART_FIFOTHRESHOLD_1_8);
  LL_LPUART_SetRXFIFOThreshold(LPUART1, LL_LPUART_FIFOTHRESHOLD_1_8);

  LL_LPUART_Enable(LPUART1);

  while ((!(LL_LPUART_IsActiveFlag_TEACK(LPUART1))) ||
         (!(LL_LPUART_IsActiveFlag_REACK(LPUART1)))) {
  }

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
      DMA1, LL_DMA_CHANNEL_1, (uint32_t)lpuart1_buffer,
      LL_LPUART_DMA_GetRegAddr(LPUART1, LL_LPUART_DMA_REG_DATA_TRANSMIT),
      LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

  LL_LPUART_EnableIT_TC(LPUART1);
  LL_LPUART_EnableDMAReq_TX(LPUART1);

  return;
}

int32_t send_lpuart1_data(char *buf, uint32_t size) {

  if (size > sizeof(lpuart1_buffer))
    return -3;

  if (tx_queued) {
    if (!LL_DMA_IsActiveFlag_TC1(DMA1))
      return -1;

    if (!LL_LPUART_IsActiveFlag_TC(LPUART1))
      return -2;

    tx_queued = 0;

    LL_LPUART_ClearFlag_TC(LPUART1);
    LL_DMA_ClearFlag_TC1(DMA1);
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_1);
  }

  memcpy(lpuart1_buffer, buf, size);

  LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, size);
  LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);

  tx_queued = 1;

  return 0;
}
