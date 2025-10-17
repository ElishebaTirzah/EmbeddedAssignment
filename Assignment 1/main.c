#include "main.h"
#include <stdio.h>
#include <inttypes.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct {
    uint8_t dataID;    // 1 byte
    int32_t DataValue; // 4 bytes
} Data_t;			   //Total bytes with padding is 8

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define QUEUE_LENGTH            (5U)
#define PRODUCER_DELAY_MS       (500U)
#define TASK_STACK_WORDS        (128U)
#define TASK_PRIORITY           (2U)
#define PRIORITY_BOOST_DELTA    (2U)
#define PRIORITY_REDUCE_DELTA   (1U)

#define DATA_ID_DELETE          (0U)
#define DATA_ID_PROCESS         (1U)

#define DATA_VALUE_BOOST        (0)
#define DATA_VALUE_REDUCE       (1)
#define DATA_VALUE_DELETE       (2)

#define COUNTER_MAX_EXCLUSIVE   (5)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static TaskHandle_t TaskHandle_1;
static TaskHandle_t TaskHandle_2;

/* Global variables for data structure population */
uint8_t G_DataID = 1U;
int32_t G_DataValue = 0;

/* Counter for updating G_DataValue */
static uint32_t g_update_counter = 0U;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static void ExampleTask1(void *pV);
static void ExampleTask2(void *pV);
static QueueHandle_t Queue1;

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
	Queue1 = xQueueCreate(QUEUE_LENGTH, sizeof(Data_t));

	BaseType_t status;
	if (Queue1 == NULL) {
		printf("[ERROR]: Queue creation failed");
	    Error_Handler();     	    // Queue creation failed

	}

//-------------------------------[TASK CREATION]------------------------------------------------
  //Task A creation
  status = xTaskCreate(ExampleTask1,"Task-1",TASK_STACK_WORDS,NULL,TASK_PRIORITY,&TaskHandle_1);
  configASSERT(status ==pdPASS);

  //Task B creation
  status = xTaskCreate(ExampleTask2,"Task-2",TASK_STACK_WORDS,NULL,TASK_PRIORITY,&TaskHandle_2);
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
static void ExampleTask1(void *pV){
    Data_t myData;
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(PRODUCER_DELAY_MS);

    /* Initialize the last wake time for precise timing */
    xLastWakeTime = xTaskGetTickCount();

    while(1){
        /* Populate structure from global variables */
        myData.dataID = G_DataID;
        myData.DataValue = G_DataValue;

        /* Send to queue */
        if (xQueueSend(Queue1, &myData, portMAX_DELAY) != pdPASS) {
            printf("[ERROR]: Sending failed");
        }
        
        /* Wait for exactly 500ms using vTaskDelayUntil for precise timing */
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

static void ExampleTask2(void *pV){
    Data_t rxData;
    const UBaseType_t originalPriority = uxTaskPriorityGet(NULL);
    uint8_t priorityIncreased = 0U;

	while(1){
        if (xQueueReceive(Queue1, &rxData, portMAX_DELAY) == pdPASS){
            printf("ID: %" PRIu8 ", Data: %" PRId32 "\n", rxData.dataID, rxData.DataValue);
		}
        // Delete Example Task 2
        if (rxData.dataID == DATA_ID_DELETE){
			vTaskDelete(NULL);
		}

        // Allow Processing of Data members
        if (rxData.dataID == DATA_ID_PROCESS){
            // Increase priority
            if(rxData.DataValue == DATA_VALUE_BOOST){
                vTaskPrioritySet(NULL, originalPriority + PRIORITY_BOOST_DELTA);
                priorityIncreased = 1U;
			}
            // Reduce priority if previously increased
            else if(rxData.DataValue == DATA_VALUE_REDUCE){
                if (priorityIncreased != 0U){
                    vTaskPrioritySet(NULL, originalPriority - PRIORITY_REDUCE_DELTA);
                    priorityIncreased = 0U;
				}
			}
            // Delete Example Task 2
            else if(rxData.DataValue == DATA_VALUE_DELETE){
			        	vTaskDelete(NULL);
			}

        }
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
    
    /* Update G_DataValue every 100ms (assuming 1ms tick) */
    g_update_counter++;
    if (g_update_counter >= 100U) {
      g_update_counter = 0U;
      G_DataValue++;
      if (G_DataValue >= COUNTER_MAX_EXCLUSIVE) {
        G_DataValue = 0;
      }
    }
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
