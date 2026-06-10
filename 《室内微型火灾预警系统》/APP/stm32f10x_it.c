/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.3.0
  * @date    04/16/2010
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "interface.h"
#include "IRCtrol.h"
#include "bsp_esp8266.h"
#include "bsp_esp8266_test.h"
#include "usart.h"
#include "motor.h"
#include "string.h"
#include "bsp_beep.h"
/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

volatile uint32_t system_ms = 0;   // 毫秒计数器，每1ms加1
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

void EXTI15_10_IRQHandler()	 //中断服务函数，函数定义在启动文件里
{
	if(EXTI_GetITStatus(IRIN_EXITLINE) != RESET)	//红外中断
	{
		EXTI_ClearITPendingBit(IRIN_EXITLINE);//清除中断标志
		IRIntIsr();
	}
	
//	if(EXTI_GetITStatus(IRIN_EXITLINE) != RESET)	//超声波中断
//	{
//		EXTI_ClearITPendingBit(IRIN_EXITLINE);//清除中断标志
//	}
}


/**
 * @brief TIM3中断服务函数（需放在 stm32f10x_it.c 中）
 *        每 0.5ms 翻转一次，产生 1kHz 连续方波
 */
void TIM3_IRQHandler(void)
{
    BEEP_IRQHandler();
}

/**-------------------------------------------------------
  * @函数名 TIM5_IRQHandler
  * @功能   TIM5中断处理函数，每0.1ms中断一次 
  * @参数   无
  * @返回值 无
***------------------------------------------------------*/
void TIM2_IRQHandler(void)
{
    /* www.armjishu.com ARM技术论坛 */

    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        tick_1ms++;
				if(tick_1ms >= 10)
				{
					tick_1ms = 0;
					speed_count++;
					tick_5ms++;
					system_ms++;
					if(speed_count >= 50)
					{
						speed_count = 0;
					}
					CarMove();
				}
    }
}


/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 

// 串口中断服务函数
void DEBUG_USART_IRQHandler(void)
{
  uint8_t ucCh;
	if ( USART_GetITStatus ( USART1, USART_IT_RXNE ) != RESET )
	{
		ucCh  = USART_ReceiveData( USART1 );
		
		if ( strUSART_Fram_Record .InfBit .FramLength < ( RX_BUF_MAX_LEN - 1 ) )                       //预留1个字节写结束符
			   strUSART_Fram_Record .Data_RX_BUF [ strUSART_Fram_Record .InfBit .FramLength ++ ]  = ucCh;

	}
	 	 
	if ( USART_GetITStatus( USART1, USART_IT_IDLE ) == SET )                                         //数据帧接收完毕
	{
    strUSART_Fram_Record .InfBit .FramFinishFlag = 1;		
		
		ucCh = USART_ReceiveData( USART1 );                                                              //由软件序列清除中断标志位(先读USART_SR，然后读USART_DR)	
  }	
}
/**
  * @brief  This function handles macESP8266_USARTx Handler.
  * @param  None
  * @retval None
  */
void macESP8266_USART_INT_FUN ( void )
{	
	uint8_t ucCh;
	
	if ( USART_GetITStatus ( macESP8266_USARTx, USART_IT_RXNE ) != RESET )
	{
		ucCh  = USART_ReceiveData( macESP8266_USARTx );
		
		if ( strEsp8266_Fram_Record .InfBit .FramLength < ( RX_BUF_MAX_LEN - 1 ) )                       //预留1个字节写结束符
			strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ++ ]  = ucCh;

	}
	 	 
	if ( USART_GetITStatus( macESP8266_USARTx, USART_IT_IDLE ) == SET )                                         //数据帧接收完毕
	{
    strEsp8266_Fram_Record .InfBit .FramFinishFlag = 1;
		
		ucCh = USART_ReceiveData( macESP8266_USARTx );                                                              //由软件序列清除中断标志位(先读USART_SR，然后读USART_DR)
	
		ucTcpClosedFlag = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "CLOSED\r\n" ) ? 1 : 0;                   //获取连接状态
		
  }	

}
/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
