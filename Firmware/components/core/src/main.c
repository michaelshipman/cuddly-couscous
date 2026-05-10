#include "main.h"
#include "radio.h"
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

int main(void) {
  /* System interrupt init*/
  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* SysTick_IRQn interrupt configuration */
  NVIC_SetPriority(SysTick_IRQn,
                   NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 15, 0));

  init_sys_clock();

  lpuart1_init();

  init_subghz();

  set_radio_tcxo();

  calibrate_radio();

  set_radio_fs();

  while (1) {
    while (send_lpuart1_data(test_str, sizeof(test_str)) < 0)
      ;

    while (send_lpuart1_data(another_str, sizeof(another_str)) < 0)
      ;

    get_radio_status();

    snprintf(buffer, sizeof(buffer), "%x\r\n", radio.status);

    while (send_lpuart1_data(buffer, sizeof(buffer)) < 0)
      ;
  }
}
