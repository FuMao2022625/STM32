#ifndef __AMG8833_H
#define __AMG8833_H
#include "AMG_IIC.h"
 
#define AMG88xx_PIXEL_TEMP_CONVERSION 0.25f
#define AMG88xx_THERMISTOR_CONVERSION 0.0625
 
enum
{
	AMG88xx_PCTL = 0x00,
	AMG88xx_RST = 0x01,
	AMG88xx_FPSC = 0x02,
	AMG88xx_INTC = 0x03,
	AMG88xx_STAT = 0x04,
	AMG88xx_SCLR = 0x05,
	//0x06 reserved
	AMG88xx_AVE = 0x07,
	AMG88xx_INTHL = 0x08,
	AMG88xx_INTHH = 0x09,
	AMG88xx_INTLL = 0x0A,
	AMG88xx_INTLH = 0x0B,
	AMG88xx_IHYSL = 0x0C,
	AMG88xx_IHYSH = 0x0D,
	AMG88xx_TTHL = 0x0E,
	AMG88xx_TTHH = 0x0F,
	AMG88xx_INT_OFFSET = 0x010,
	AMG88xx_PIXEL_OFFSET = 0x80
};
 
enum power_modes{
	AMG88xx_NORMAL_MODE = 0x00,
	AMG88xx_SLEEP_MODE = 0x01,
	AMG88xx_STAND_BY_60 = 0x20,
	AMG88xx_STAND_BY_10 = 0x21
};
 
enum sw_resets {
	AMG88xx_FLAG_RESET = 0x30,
	AMG88xx_INITIAL_RESET = 0x3F
};
 
enum frame_rates {
	AMG88xx_FPS_10 = 0x00,
	AMG88xx_FPS_1 = 0x01
};
 
enum int_enables{
	AMG88xx_INT_DISABLED = 0x00,
	AMG88xx_INT_ENABLED = 0x01
};
 
enum int_modes {
	AMG88xx_DIFFERENCE = 0x00,
	AMG88xx_ABSOLUTE_VALUE = 0x01
};
 
void AMG8833_Init(void);
float AMG88xx_ReadThermistor(void);
void amg88xx_readPixels(float *buf, uint8_t size);
char* format_pix_temp_to_json(float pix[8][8], char *buffer, unsigned int buf_size);
 
#endif
