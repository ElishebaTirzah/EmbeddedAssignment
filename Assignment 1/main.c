/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct {
    uint8_t dataID;    // 1 byte
    int32_t DataValue; // 4 bytes
} Data_t;			   //Total bytes with padding is 8

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
TaskHandle_t TaskHandle_1;
TaskHandle_t TaskHandle_2;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void ExampleTask1(void *pV);
void ExampleTask2(void *pV);
QueueHandle_t Queue1;

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */
	Queue1 = xQueueCreate(5, sizeof(Data_t));

	BaseType_t status;
	if (Queue1 == NULL) {
		printf("[ERROR]: Queue creation failed");
	    Error_Handler();     	    // Queue creation failed

	}

//-------------------------------[TASK CREATION]------------------------------------------------
  //Task A creation
  status = xTaskCreate(ExampleTask1,"Task-1",128,NULL,2,&TaskHandle_1);
  configASSERT(status ==pdPASS);

  //Task B creation
  status = xTaskCreate(ExampleTask2,"Task-2",128,NULL,2,&TaskHandle_2);
  configASSERT(status ==pdPASS);

  //Start of Scheduler
  vTaskStartScheduler();

  //if code execution reaches here, launch of scheduler has failed due to insufficient memory in the heap
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void ExampleTask1(void *pV){
	Data_t myData;
	int counter = 0;
	myData.dataID = 1;
	while(1){
		myData.DataValue = counter++;

        // Sending to queue
        if (xQueueSend(Queue1, &myData, portMAX_DELAY) != pdPASS) {
            printf("[ERROR]: Sending failed");
        }
        vTaskDelay(pdMS_TO_TICKS(500)); // 500ms delay of sending to queue
//		taskYIELD();
		if (counter == 5){
			counter = 0;
		}
	}
}

void ExampleTask2(void *pV){
	Data_t rxData;
	UBaseType_t originalPriority = uxTaskPriorityGet(NULL);
    uint8_t priorityIncreased = 0;

	while(1){
		if (xQueueReceive(Queue1, &rxData, portMAX_DELAY) ==pdPASS){
			printf("ID: %d, Data: %ld\n", rxData.dataID, rxData.DataValue);
		}
		//Delete Example Task 2
		if (rxData.dataID == 0){
			vTaskDelete(NULL);
		}

		//Allow Processing of Data members
		if (rxData.dataID == 1){
			//Increase the Priority of Example Task by 2 from the value given at creation
			if(rxData.DataValue == 0){
			    vTaskPrioritySet( NULL, originalPriority + 2);
			    priorityIncreased = 1;
			}
			//Decrease the Priority of Example Task 2 if previously increased
			else if(rxData.DataValue == 1){
				if (priorityIncreased){
					vTaskPrioritySet(NULL, originalPriority -1);
				    priorityIncreased = 0;
				}
			}
			//Delete Example Task 2
			else if(rxData.DataValue == 2){
				vTaskDelete(NULL);
			}

		}
//		taskYIELD();
	}
}
/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
