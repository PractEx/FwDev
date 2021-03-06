/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l0xx_hal.h"
#include "dma.h"
#include "usart.h"
#include "tim.h"
#include "gpio.h"
#include "string.h"
#include "stm32l0xx_it.h"

HAL_TIM_StateTypeDef std_t21;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void TIM21_IRQHandler(void);
//UART_HandleTypeDef UartHandle;

#define  PERIOD_VALUE       (uint32_t)(1024 - 1)  /* Period Value  */
#define  PULSE1_VALUE       (uint32_t)(PERIOD_VALUE*95/100)        /* Capture Compare 1 Value  */
#define  PULSE2_VALUE       (uint32_t)(PERIOD_VALUE*60/100) /* Capture Compare 2 Value  */
#define  PULSE3_VALUE       (uint32_t)(PERIOD_VALUE*37.5/100)        /* Capture Compare 3 Value  */
#define  PULSE4_VALUE       (uint32_t)(PERIOD_VALUE*12.5/100) /* Capture Compare 4 Value  */

uint32_t timer_value2 = 0;
uint32_t timer_value21 = 0;


int main(void)
{
  
  
  HAL_Init();                   //Reset of all peripherals, Initializes the Flash interface and the Systick.
  SystemClock_Config();         // Config sys clk
  MX_GPIO_Init();               // init gpio
  MX_DMA_Init();                // init DMP
  
  MX_LPUART1_UART_Init();       // init debugger uart
  MX_USART2_UART_Init();        // init rs485 comm uart
  MX_TIM2_Init();               // init timer2 for pwms
  MX_TIM21_Init();              // init timer21 for pwms
  
  
  
  //HAL_TIM_Base_Start_IT(&htim21); 
  if(HAL_TIM_Base_Start_IT(&htim21) != HAL_OK); 
  {
     /* Starting Error */
    //Error_Handler();
  }
  
  
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    // Initialization Error
    Error_Handler();
  }
  
  char TxM1[] = "\n\r I'm alive\n\r";
  uint16_t TxM1_L = strlen(TxM1);
  
  if (HAL_UART_Transmit_DMA(&huart2, (uint8_t *)TxM1, TxM1_L) != HAL_OK)
  {
    /* Transfer error in transmission process */
    Error_Handler();
  }  
  
  /*if (HAL_UART_Init(&hlpuart1) != HAL_OK)
  {
    // Initialization Error 
    Error_Handler();
  }*/
  
  char TxM2[] = "\n\r Debug Port Active\n\r";
  uint16_t TxM2_L = strlen(TxM1);
  
  
    if (HAL_UART_Transmit_DMA(&hlpuart1, (uint8_t *)TxM2, TxM2_L) != HAL_OK)
  {
    /* Transfer error in transmission process */
    Error_Handler();
  }  

  sConfigOC.Pulse = PULSE1_VALUE;
  HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1);
  sConfigOC.Pulse = PULSE2_VALUE;
  HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2);
  sConfigOC.Pulse = PULSE3_VALUE;
  HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3);
  sConfigOC.Pulse = PULSE4_VALUE;
  HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4);
  
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
      HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
        HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
  
     
        
        HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
        HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
        HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
        HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
  
  //USART2_CR1.WAKE = 1;
  //USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
  
  while (1)
  {
  //HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin); //heartbeat
    timer_value21 = __HAL_TIM_GET_COUNTER(&htim21);
    timer_value2 = __HAL_TIM_GET_COUNTER(&htim2);
    std_t21 = HAL_TIM_Base_GetState(&htim21);
  }
  

}


void TIM21_IRQHandler(void) // do i need this???
{
    HAL_TIM_IRQHandler(&htim21);
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  //BSP_LED_Toggle(LED3);
  if (htim->Instance == TIM21) 
  { // maybe not needed
    HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
  }
}

// System Clock Configuration

void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_LPUART1;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
