#include "main.h"
#include "stm32wlxx.h"
#include "stm32wlxx_ll_dma.h"
#include "stm32wlxx_ll_dmamux.h"
#include "stm32wlxx_ll_spi.h"
#include "system.h"
#include "usart.h"
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char test_str[] = "hello world!\r\n";
char another_str[] = "a much longer string for testing purposes\r\n";
char buffer[1024] = "";

__attribute__((optimize("-O0"))) int foo(int j) {

  for (int i = 0; i < j; i++)
    buffer[0] = i;

  return j;
}

int main(void) {
  /* System interrupt init*/
  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* SysTick_IRQn interrupt configuration */
  NVIC_SetPriority(SysTick_IRQn,
                   NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 15, 0));

  init_sys_clock();

  lpuart1_init();

  LL_APB3_GRP1_EnableClock(LL_APB3_GRP1_PERIPH_SUBGHZSPI);

  /* Disable SUBGHZSPI Peripheral */
  CLEAR_BIT(SUBGHZSPI->CR1, SPI_CR1_SPE);

  /*----------------------- SPI CR1 Configuration ----------------------------*
   *             SPI Mode: Master                                             *
   *   Communication Mode: 2 lines (Full-Duplex)                              *
   *       Clock polarity: Low                                                *
   *                phase: 1st Edge                                           *
   *       NSS management: Internal (Done with External bit inside PWR        *
   *  Communication speed: BaudratePrescaler                             *
   *            First bit: MSB                                                *
   *      CRC calculation: Disable                                            *
   *--------------------------------------------------------------------------*/
  WRITE_REG(SUBGHZSPI->CR1,
            (SPI_CR1_MSTR | SPI_CR1_SSI | SPI_CR1_BR_1 | SPI_CR1_SSM));

  /*----------------------- SPI CR2 Configuration ----------------------------*
   *            Data Size: 8bits                                              *
   *              TI Mode: Disable                                            *
   *            NSS Pulse: Disable                                            *
   *    Rx FIFO Threshold: 8bits                                              *
   *--------------------------------------------------------------------------*/
  WRITE_REG(SUBGHZSPI->CR2,
            (SPI_CR2_FRXTH | SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2));

  /* Enable SUBGHZSPI Peripheral */
  SET_BIT(SUBGHZSPI->CR1, SPI_CR1_SPE);

  LL_RCC_RF_DisableReset();
  LL_PWR_UnselectSUBGHZSPI_NSS();

  while (LL_RCC_IsRFUnderReset())
    ;

  LL_PWR_SelectSUBGHZSPI_NSS();

  while (LL_PWR_IsActiveFlag_RFBUSYMS())
    ;

  LL_PWR_UnselectSUBGHZSPI_NSS();

  while (LL_PWR_IsActiveFlag_RFBUSYMS())
    ;

  while (1) {
    while (send_lpuart1_data(test_str, sizeof(test_str)) < 0)
      ;

    while (send_lpuart1_data(another_str, sizeof(another_str)) < 0)
      ;

    while (LL_PWR_IsActiveFlag_RFBUSY())
      ;

    LL_PWR_SelectSUBGHZSPI_NSS();

    while (!LL_PWR_IsSUBGHZSPI_NSS_Selected())
      ;

    LL_SPI_TransmitData8(SUBGHZSPI, 0xC0);

    uint8_t rx = LL_SPI_ReceiveData8(SUBGHZSPI);

    foo(10);

    LL_PWR_UnselectSUBGHZSPI_NSS();

    snprintf(buffer, sizeof(buffer), "%x\r\n", rx);

    while (send_lpuart1_data(buffer, sizeof(buffer)) < 0)
      ;
  }
}
