#ifndef USART_H
#define USART_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

void lpuart1_init(void);

int32_t send_lpuart1_data(char *, uint32_t);

#ifdef __cplusplus
}
#endif

#endif /* USART_H */
