#ifndef __LD2412_H_
#define __LD2412_H_
#include <stdint.h>

#define FLAG_PEOPLE GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_11)
void LD2412_Init(void);
uint8_t LD2412_Read(void);

#endif
