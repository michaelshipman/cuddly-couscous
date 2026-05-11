#ifndef SUBGHZ_H
#define SUBGHZ_H

#include "stm32wlxx_ll_pwr.h"
#include "stm32wlxx_ll_rcc.h"
#include "stm32wlxx_ll_spi.h"
#include "stm32wlxx_ll_system.h"
#include "stm32wlxx_ll_utils.h"

typedef struct {
  uint8_t status_mode;
  uint8_t cmd_status;
} Radio_t;

Radio_t radio;

__attribute__((optimize("-O0"))) int foo(int j) {

  for (int i = 0; i < j; i++)
    ;

  return j;
}

static inline void init_subghz(void) {

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

  while (LL_PWR_IsActiveFlag_RFBUSY())
    ;

  while (LL_RCC_IsRFUnderReset())
    ;

  LL_PWR_SelectSUBGHZSPI_NSS();

  while (LL_PWR_IsActiveFlag_RFBUSYMS())
    ;

  LL_PWR_UnselectSUBGHZSPI_NSS();

  while (LL_PWR_IsActiveFlag_RFBUSYMS())
    ;
}

static inline void set_radio_tcxo(void) {

  while (LL_PWR_IsActiveFlag_RFBUSY())
    ;

  LL_PWR_SelectSUBGHZSPI_NSS();

  while (!LL_PWR_IsSUBGHZSPI_NSS_Selected())
    ;

  LL_SPI_TransmitData8(SUBGHZSPI, 0x97);
  LL_SPI_TransmitData8(SUBGHZSPI, 0x01);
  LL_SPI_TransmitData8(SUBGHZSPI, 0x00);
  LL_SPI_TransmitData8(SUBGHZSPI, 0x00);
  LL_SPI_TransmitData8(SUBGHZSPI, 0x00);

  foo(10);

  LL_PWR_UnselectSUBGHZSPI_NSS();

  LL_RCC_HSE_EnableTcxo();

  LL_RCC_HSE_Enable();

  while (!LL_RCC_HSE_IsReady())
    ;
}

static inline void get_radio_status(void) {

  uint8_t data = 0;

  while (LL_PWR_IsActiveFlag_RFBUSY())
    ;

  LL_PWR_SelectSUBGHZSPI_NSS();

  while (!LL_PWR_IsSUBGHZSPI_NSS_Selected())
    ;

  LL_SPI_TransmitData8(SUBGHZSPI, 0xC0);

  data = LL_SPI_ReceiveData8(SUBGHZSPI);

  foo(10);

  LL_PWR_UnselectSUBGHZSPI_NSS();

  radio.status_mode = (data & 0x70) >> 4;
  radio.cmd_status = (data & 0x0E) >> 1;
}

static inline void calibrate_radio(void) {

  while (LL_PWR_IsActiveFlag_RFBUSY())
    ;

  LL_PWR_SelectSUBGHZSPI_NSS();

  while (!LL_PWR_IsSUBGHZSPI_NSS_Selected())
    ;

  LL_SPI_TransmitData8(SUBGHZSPI, 0x89);
  LL_SPI_TransmitData8(SUBGHZSPI, 0x7F);

  foo(10);

  LL_PWR_UnselectSUBGHZSPI_NSS();
}

static inline void set_radio_fs(void) {

  while (LL_PWR_IsActiveFlag_RFBUSY())
    ;

  LL_PWR_SelectSUBGHZSPI_NSS();

  while (!LL_PWR_IsSUBGHZSPI_NSS_Selected())
    ;

  LL_SPI_TransmitData8(SUBGHZSPI, 0xC1);

  foo(10);

  LL_PWR_UnselectSUBGHZSPI_NSS();
}

#endif
