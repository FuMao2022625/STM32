#include "AMG8833.h"
#include "usart.h"
 
void AMG8833_Init(void)
{
	AMG8833_IIC_Init();           //IIC初始化
	
	//enter normal mode
	AMG_IIC_Write_1Byte(AMG88xx_ADR,AMG88xx_PCTL,AMG88xx_NORMAL_MODE);
	
	//software reset
	AMG_IIC_Write_1Byte(AMG88xx_ADR,AMG88xx_RST,AMG88xx_INITIAL_RESET);
	
	//set to 10 FPS
	AMG_IIC_Write_1Byte(AMG88xx_ADR,AMG88xx_FPSC,AMG88xx_FPS_10);
}
 
//float signedMag12ToFloat(uint16_t val)
//{
//	//take first 11 bits as absolute val
//	uint16_t absVal = (val & 0x7FF);
//	
//	return (val & 0x800) ? 0 - (float)absVal : (float)absVal ;
//}
int16_t signedMag12ToFloat(uint16_t val)
{
    uint16_t raw12 = val & 0x0FFF;
    if (raw12 & 0x0800) {
        return (int16_t)(raw12 | 0xF000);
    } else {
        return (int16_t)raw12;
    }
}
 
float AMG88xx_ReadThermistor(void)
{
	uint8_t raw[2];
	uint16_t recast;
	
	AMG_I2C_Read_nByte(AMG88xx_ADR,AMG88xx_TTHL, raw, 2);
	recast = ((uint16_t)raw[1] << 8) | ((uint16_t)raw[0]);
	return signedMag12ToFloat(recast) * AMG88xx_THERMISTOR_CONVERSION;
}
 
void amg88xx_readPixels(float *buf, uint8_t size)
{
	uint16_t recast;
	float converted;
	uint8_t rawArray[128],i;
	
	AMG_I2C_Read_nByte(AMG88xx_ADR,AMG88xx_PIXEL_OFFSET,rawArray,128);
	for(i=0; i<size; i++)
	{
		uint8_t pos = i << 1;
		recast = ((uint16_t)rawArray[pos + 1] << 8) | ((uint16_t)rawArray[pos]);        	
		converted = signedMag12ToFloat(recast) * AMG88xx_PIXEL_TEMP_CONVERSION;
        
		buf[i] = converted;
	}
}

/**
 * @brief 将8x8浮点矩阵格式化为JSON二维数组字符串
 * @param pix 8x8浮点数矩阵
 * @param buffer 输出缓冲区
 * @param buf_size 缓冲区大小
 * @return 返回buffer指针（方便链式调用）
 */
char* format_pix_temp_to_json(float pix[8][8], char *buffer, unsigned int buf_size)
{
    int offset = 0;  // 当前写入位置
		int i,j;
    // 最外层左括号
    offset += snprintf(buffer + offset, buf_size - offset, "[");

    for (i = 0; i < 8; i++) {
        // 行左括号
        offset += snprintf(buffer + offset, buf_size - offset, "[");
        for (j = 0; j < 8; j++) {
            // 写入浮点数，保留2位小数（可根据需要调整格式）
            offset += snprintf(buffer + offset, buf_size - offset, "%.2f", pix[i][j]);
            if (j < 7) {
                offset += snprintf(buffer + offset, buf_size - offset, ","); // 列分隔
            }
        }
        // 行右括号
        offset += snprintf(buffer + offset, buf_size - offset, "]");
        if (i < 7) {
            offset += snprintf(buffer + offset, buf_size - offset, ","); // 行分隔
        }
    }

    // 最外层右括号
    offset += snprintf(buffer + offset, buf_size - offset, "]");
 
    return buffer;
}
