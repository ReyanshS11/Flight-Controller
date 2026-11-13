/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include <iostream>
#include "lwgps/lwgps.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct {
  float accel_g[3];
  float gyro_dps[3];
  float temp_c;
} IMU_Data_t;

typedef struct {
  float mag_ut[3];
  float temp_c;
} MAG_Data_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define Baro_ADDR       (0x47 << 1)
#define Charger_ADDR    (0x6B << 1)

#define Baro_REG_PRESSURE_0   0x20
#define Baro_REG_TEMP_0       0x1D

#define Charger_REG_VCELL1      0x1A       
#define Charger_REG_VCELL2      0x1C

#define IMU_REG_ACCEL_X1        0x00
#define IMU_PWR_MGMT0           0x10
#define IMU_ACCEL_CONFIG0       0x1B
#define IMU_GYRO_CONFIG0        0x1C
#define IMU_WHO_AM_I            0x72

#define ACCEL_CONVERSION        2048.0f
#define GYRO_CONVERSION         16.384f

#define MAG_WHO_AM_I            0x4F // Returns 0x40
#define OUTX_L_REG              0x68
#define MAG_CFG_REG_A           0x60
#define MAG_CFG_REG_B           0x61
#define MAG_CFG_REG_C           0x62

#define MAG_CONVERSION          0.15f
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc3;

I2C_HandleTypeDef hi2c1;

SD_HandleTypeDef hsd1;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart8;

HCD_HandleTypeDef hhcd_USB_OTG_FS;

/* USER CODE BEGIN PV */
volatile uint8_t read_sensors_flag = 0;

uint8_t barometer_online = 0;
float pressure = 0.0f;
float temperature = 0.0f;

float cell1_voltage = 0.0f;
float cell2_voltage = 0.0f;

uint8_t bmp_buffer[3];
uint8_t bq_buffer[2];

IMU_Data_t imu_data;
uint8_t imu_online = 0;

MAG_Data_t mag_data;
uint8_t mag_online = 0;

const uint8_t ubx_cfg_rate_10hz[14] = {
    0xB5, 0x62,             // UBX Sync Header
    0x06, 0x08,             // Class: CFG (0x06), ID: RATE (0x08)
    0x06, 0x00,             // Payload Length: 6 bytes (Little-Endian)
    0x64, 0x00,             // measRate: 100 ms = 0x0064 -> 10 Hz (Little-Endian)
    0x01, 0x00,             // navRate: 1 cycle (must always be 1)
    0x01, 0x00,             // timeRef: 1 = GPS time (0 = UTC time)
    0x7A, 0x12              // Checksum: CK_A = 0x7A, CK_B = 0x12
};

lwgps_t gps_data;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MPU_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_ADC3_Init(void);
static void MX_SDMMC1_SD_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM1_Init(void);
static void MX_UART8_Init(void);
static void MX_USB_OTG_FS_HCD_Init(void);
static void MX_TIM6_Init(void);
/* USER CODE BEGIN PFP */
uint8_t Barometer_Init(void);
void Read_Barometer(void);
void Read_Charger(void);

uint8_t IMU_Init(void);
void Read_IMU(IMU_Data_t *data);

uint8_t MAG_Init(void);
void Read_MAG(MAG_Data_t *data);

void GPS_Init(void);
void Read_GPS(lwgps_t *data);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_ADC3_Init();
  MX_SDMMC1_SD_Init();
  MX_SPI1_Init();
  MX_TIM1_Init();
  MX_UART8_Init();
  MX_USB_OTG_FS_HCD_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
  barometer_online = Barometer_Init();

  imu_online = IMU_Init();
  mag_online = MAG_Init();

  lwgps_init(&gps_data);
  GPS_Init();

  HAL_TIM_Base_Start_IT(&htim6);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    if (read_sensors_flag == 1)
    {
      read_sensors_flag = 0;

      // Read IMU
      if (imu_online)
      {
        Read_IMU(&imu_data);
      }

      static uint8_t tick = 0;
      tick++;

      switch (tick)
      {
        case 1:
          if (mag_online)
          {
            Read_MAG(&mag_data);
          }
          break;

        case 2:
          if (barometer_online)
          {
            Read_Barometer();
          }
          break;

        case 3:
          Read_Charger();
          break;

        case 4:
          Read_GPS(&gps_data);
          break;

        case 10:
          tick = 0;
          break;

        default:
          break;
      }
    }

    HAL_Delay(100);
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

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSI
                              |RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 160;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 10;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_CKPER;
  PeriphClkInitStruct.CkperClockSelection = RCC_CLKPSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC3_Init(void)
{

  /* USER CODE BEGIN ADC3_Init 0 */

  /* USER CODE END ADC3_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC3_Init 1 */

  /* USER CODE END ADC3_Init 1 */

  /** Common config
  */
  hadc3.Instance = ADC3;
  hadc3.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc3.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc3.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc3.Init.LowPowerAutoWait = DISABLE;
  hadc3.Init.ContinuousConvMode = DISABLE;
  hadc3.Init.NbrOfConversion = 1;
  hadc3.Init.DiscontinuousConvMode = DISABLE;
  hadc3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc3.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
  hadc3.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc3.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  hadc3.Init.OversamplingMode = DISABLE;
  hadc3.Init.Oversampling.Ratio = 1;
  if (HAL_ADC_Init(&hadc3) != HAL_OK)
  {
    Error_Handler();
  }
  hadc3.Init.Resolution = ADC_RESOLUTION_12B;
  if (HAL_ADC_Init(&hadc3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_11;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  sConfig.OffsetSignedSaturation = DISABLE;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC3_Init 2 */

  /* USER CODE END ADC3_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x10C0ECFF;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SDMMC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDMMC1_SD_Init(void)
{

  /* USER CODE BEGIN SDMMC1_Init 0 */

  /* USER CODE END SDMMC1_Init 0 */

  /* USER CODE BEGIN SDMMC1_Init 1 */

  /* USER CODE END SDMMC1_Init 1 */
  hsd1.Instance = SDMMC1;
  hsd1.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
  hsd1.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hsd1.Init.BusWide = SDMMC_BUS_WIDE_4B;
  hsd1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd1.Init.ClockDiv = 0;
  if (HAL_SD_Init(&hsd1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SDMMC1_Init 2 */

  /* USER CODE END SDMMC1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_4BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 0x0;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  hspi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi1.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi1.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 199;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 999;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief UART8 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART8_Init(void)
{

  /* USER CODE BEGIN UART8_Init 0 */

  /* USER CODE END UART8_Init 0 */

  /* USER CODE BEGIN UART8_Init 1 */

  /* USER CODE END UART8_Init 1 */
  huart8.Instance = UART8;
  huart8.Init.BaudRate = 115200;
  huart8.Init.WordLength = UART_WORDLENGTH_8B;
  huart8.Init.StopBits = UART_STOPBITS_1;
  huart8.Init.Parity = UART_PARITY_NONE;
  huart8.Init.Mode = UART_MODE_TX_RX;
  huart8.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart8.Init.OverSampling = UART_OVERSAMPLING_16;
  huart8.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart8.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart8.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart8, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart8, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart8) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART8_Init 2 */

  /* USER CODE END UART8_Init 2 */

}

/**
  * @brief USB_OTG_FS Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_OTG_FS_HCD_Init(void)
{

  /* USER CODE BEGIN USB_OTG_FS_HCD_Init 0 */

  /* USER CODE END USB_OTG_FS_HCD_Init 0 */

  /* USER CODE BEGIN USB_OTG_FS_HCD_Init 1 */

  /* USER CODE END USB_OTG_FS_HCD_Init 1 */
  hhcd_USB_OTG_FS.Instance = USB_OTG_FS;
  hhcd_USB_OTG_FS.Init.Host_channels = 16;
  hhcd_USB_OTG_FS.Init.speed = HCD_SPEED_FULL;
  hhcd_USB_OTG_FS.Init.dma_enable = DISABLE;
  hhcd_USB_OTG_FS.Init.phy_itface = HCD_PHY_EMBEDDED;
  hhcd_USB_OTG_FS.Init.Sof_enable = DISABLE;
  if (HAL_HCD_Init(&hhcd_USB_OTG_FS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_OTG_FS_HCD_Init 2 */

  /* USER CODE END USB_OTG_FS_HCD_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, RF_RESET_Pin|GPS_RESET_Pin|ICM_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, MAG_CS_Pin|GPS_CS_Pin|RF_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(BQ_CE_GPIO_Port, BQ_CE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : RF_DIO1_Pin */
  GPIO_InitStruct.Pin = RF_DIO1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(RF_DIO1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : RF_DIO0_Pin GPS_EXTINT_Pin */
  GPIO_InitStruct.Pin = RF_DIO0_Pin|GPS_EXTINT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : RF_RESET_Pin GPS_RESET_Pin */
  GPIO_InitStruct.Pin = RF_RESET_Pin|GPS_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : ICM_CS_Pin */
  GPIO_InitStruct.Pin = ICM_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ICM_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : ICM_INT1_Pin ICM_INT2_Pin */
  GPIO_InitStruct.Pin = ICM_INT1_Pin|ICM_INT2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : MAG_CS_Pin GPS_CS_Pin RF_CS_Pin */
  GPIO_InitStruct.Pin = MAG_CS_Pin|GPS_CS_Pin|RF_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : BQ_CE_Pin */
  GPIO_InitStruct.Pin = BQ_CE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BQ_CE_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : BMP_INT_Pin BQ_INT_Pin */
  GPIO_InitStruct.Pin = BMP_INT_Pin|BQ_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : SD_CS_Pin */
  GPIO_InitStruct.Pin = SD_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SD_CS_GPIO_Port, &GPIO_InitStruct);

  /*AnalogSwitch Config */
  HAL_SYSCFG_AnalogSwitchConfig(SYSCFG_SWITCH_PA1, SYSCFG_SWITCH_PA1_CLOSE);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
static HAL_StatusTypeDef BMP581_WriteReg(uint8_t reg, uint8_t val)
{
    return HAL_I2C_Mem_Write(&hi2c1, Baro_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &val, 1, 10);
}

static HAL_StatusTypeDef BMP581_ReadRegs(uint8_t start_reg, uint8_t *buf, uint16_t len)
{
    return HAL_I2C_Mem_Read(&hi2c1, Baro_ADDR, start_reg, I2C_MEMADD_SIZE_8BIT, buf, len, 10);
}

uint8_t Barometer_Init(void)
{
    uint8_t chip_id = 0;

    // Issue a soft reset (0xB6) and wait 5 ms for NVM memory to reload
    BMP581_WriteReg(0x7E, 0xB6);
    HAL_Delay(5);

    // Verify I2C communication by reading CHIP_ID (0x01 -> returns 0x50)
    if (BMP581_ReadRegs(0x01, &chip_id, 1) != HAL_OK || chip_id != 0x50) {
        return 0; // Check I2C wiring, pull-ups, or 0x47 vs 0x46 SDO address
    }

    // Configure IIR Low-Pass Filter: Coeff 1 for Temp (0x01) & Coeff 1 for Press (0x08)
    BMP581_WriteReg(0x31, 0x09);

    // Enable Pressure (bit 6 = 0x40), Press OSR = 8x (bits 5:3 = 011 -> 0x18), Temp OSR = 1x (0x00)
    // Combined: 0x40 | 0x18 | 0x00 = 0x58 (conversion time ~4.5 ms, easily fits inside 100 Hz)
    BMP581_WriteReg(0x36, 0x58);

    // Set ODR = 100.2 Hz (bits 6:2 = 0x0A << 2 = 0x28) and Power Mode = Normal (bits 1:0 = 01)
    // Combined: 0x28 | 0x01 = 0x29 (bit 7 = 0 disables deep standby)
    BMP581_WriteReg(0x37, 0x29);
    HAL_Delay(10);

    return 1; // Initialization successful
}

void Read_Barometer(void)
{
  if (HAL_I2C_Mem_Read(&hi2c1, Baro_ADDR, Baro_REG_PRESSURE_0, I2C_MEMADD_SIZE_8BIT, bmp_buffer, 3, 100) == HAL_OK)
  {
    uint32_t raw_pressure = ((uint32_t)bmp_buffer[2] << 16) |
                            ((uint32_t)bmp_buffer[1] << 8) |
                            (uint32_t)bmp_buffer[0];

    pressure = (float)raw_pressure / 64.0f;
  }

  if (HAL_I2C_Mem_Read(&hi2c1, Baro_ADDR, Baro_REG_TEMP_0, I2C_MEMADD_SIZE_8BIT, bmp_buffer, 3, 100) == HAL_OK)
  {
    uint32_t raw_temp = ((int32_t)bmp_buffer[2] << 16) |
                        ((int32_t)bmp_buffer[1] << 8) |
                        (int32_t)bmp_buffer[0];

    if (raw_temp & 0x00800000) {
      raw_temp |= 0xFF000000;
    }

    temperature = (float)raw_temp / 65536.0f;
  }
}


void Read_Charger(void)
{
  if (HAL_I2C_Mem_Read(&hi2c1, Charger_ADDR, Charger_REG_VCELL1, I2C_MEMADD_SIZE_8BIT, bq_buffer, 2, 100) == HAL_OK)
  {
      uint16_t raw_vcell1 = ((uint16_t)bq_buffer[0] << 8) | bq_buffer[1];
      cell1_voltage = (float)raw_vcell1 / 1000.0f;
  }
  
  if (HAL_I2C_Mem_Read(&hi2c1, Charger_ADDR, Charger_REG_VCELL2, I2C_MEMADD_SIZE_8BIT, bq_buffer, 2, 100) == HAL_OK)
  {
      uint16_t raw_vcell2 = ((uint16_t)bq_buffer[0] << 8) | bq_buffer[1];
      cell2_voltage = (float)raw_vcell2 / 1000.0f;
  }
}


static void IMU_WriteReg(uint8_t reg, uint8_t val)
{
  uint8_t tx[2] = { reg & 0x7F, val }; // Bit 7 = 0 for SPI write
  HAL_GPIO_WritePin(ICM_CS_GPIO_Port, ICM_CS_Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi1, tx, 2, 10);
  HAL_GPIO_WritePin(ICM_CS_GPIO_Port, ICM_CS_Pin, GPIO_PIN_SET);
}

static void IMU_ReadRegs(uint8_t start_reg, uint8_t *buf, uint16_t len)
{
    uint8_t tx = start_reg | 0x80; // Bit 7 = 1 for SPI read
    HAL_GPIO_WritePin(ICM_CS_GPIO_Port, ICM_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, &tx, 1, 10);
    HAL_SPI_Receive(&hspi1, buf, len, 10);
    HAL_GPIO_WritePin(ICM_CS_GPIO_Port, ICM_CS_Pin, GPIO_PIN_SET);
}

uint8_t IMU_Init(void)
{
  uint8_t who_am_i = 0;

  HAL_GPIO_WritePin(ICM_CS_GPIO_Port, ICM_CS_Pin, GPIO_PIN_SET);
  HAL_Delay(10); // Give chip time to boot

  // Verify SPI comms (0x72 should return 0xE9)
  IMU_ReadRegs(IMU_WHO_AM_I, &who_am_i, 1);
  if (who_am_i != 0xE9)
  {
    return 0; // We have a problem
  }

  IMU_WriteReg(IMU_ACCEL_CONFIG0, 0x15);
  IMU_WriteReg(IMU_GYRO_CONFIG0, 0x15);
  IMU_WriteReg(IMU_PWR_MGMT0, 0x0F); // Enables low-noise mode for both accel and gyro

  HAL_Delay(50); // Give time for sensors to stabilize

  return 1; // Success
}

void Read_IMU(IMU_Data_t *data)
{
  uint8_t raw[14];

  IMU_ReadRegs(IMU_REG_ACCEL_X1, raw, 14);

  // Combine bytes in pairs to create 16-bit integers
  int16_t raw_ax = (int16_t)((raw[0] << 8) | raw[1]);
  int16_t raw_ay = (int16_t)((raw[2] << 8) | raw[3]);
  int16_t raw_az = (int16_t)((raw[4] << 8) | raw[5]);

  int16_t raw_gx = (int16_t)((raw[6] << 8) | raw[7]);
  int16_t raw_gy = (int16_t)((raw[8] << 8) | raw[9]);
  int16_t raw_gz = (int16_t)((raw[10] << 8) | raw[11]);

  int16_t raw_temp = (int16_t)((raw[12] << 8) | raw[13]);

  // Convert to physical units
  data->accel_g[0]  = (float)raw_ax / ACCEL_CONVERSION;
  data->accel_g[1]  = (float)raw_ay / ACCEL_CONVERSION;
  data->accel_g[2]  = (float)raw_az / ACCEL_CONVERSION;

  data->gyro_dps[0] = (float)raw_gx / GYRO_CONVERSION;
  data->gyro_dps[1] = (float)raw_gy / GYRO_CONVERSION;
  data->gyro_dps[2] = (float)raw_gz / GYRO_CONVERSION;

  // Temperature conversion formula from datasheet
  data->temp_c      = ((float)raw_temp / 128.0f) + 25.0f;
}


static void MAG_WriteReg(uint8_t reg, uint8_t val)
{
  uint8_t tx[2] = { reg & 0x7F, val }; // Bit 7 = 0 for SPI write
  HAL_GPIO_WritePin(MAG_CS_GPIO_Port, MAG_CS_Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi1, tx, 2, 10);
  HAL_GPIO_WritePin(MAG_CS_GPIO_Port, MAG_CS_Pin, GPIO_PIN_SET);
}

static void MAG_ReadRegs(uint8_t start_reg, uint8_t *buf, uint16_t len)
{
  uint8_t tx = start_reg | 0x80; // Bit 7 = 1 for SPI read
  HAL_GPIO_WritePin(MAG_CS_GPIO_Port, MAG_CS_Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi1, &tx, 1, 10);
  HAL_SPI_Receive(&hspi1, buf, len, 10);
  HAL_GPIO_WritePin(MAG_CS_GPIO_Port, MAG_CS_Pin, GPIO_PIN_SET);
}

uint8_t MAG_Init(void)
{
  uint8_t who_am_i = 0;

  HAL_GPIO_WritePin(MAG_CS_GPIO_Port, MAG_CS_Pin, GPIO_PIN_SET);
  HAL_Delay(10); // Give chip time to boot  

  // Verify SPI comms (0x4F should return 0x40)
  IMU_ReadRegs(MAG_WHO_AM_I, &who_am_i, 1);
  if (who_am_i != 0x40)
  {
    return 0; // We have a problem
  }

  MAG_WriteReg(MAG_CFG_REG_C, 0x14);
  MAG_WriteReg(MAG_CFG_REG_A, 0x8c);
  MAG_WriteReg(MAG_CFG_REG_B, 0x03);

  HAL_Delay(50); // Give time for sensors to stabilize

  return 1; // Success
}

void Read_MAG(MAG_Data_t *data)
{
  uint8_t raw[8];

  MAG_ReadRegs(OUTX_L_REG, raw, 8);

  // Combine bytes in pairs to create 16-bit integers
  int16_t raw_x = (int16_t)((raw[0] << 8) | raw[1]);
  int16_t raw_y = (int16_t)((raw[2] << 8) | raw[3]);
  int16_t raw_z = (int16_t)((raw[4] << 8) | raw[5]);

  int16_t raw_temp = (int16_t)((raw[6] << 8) | raw[7]);

  // Convert to physical units
  data->mag_ut[0]   = (float)raw_x / MAG_CONVERSION;
  data->mag_ut[1]   = (float)raw_y / MAG_CONVERSION;
  data->mag_ut[2]   = (float)raw_z / MAG_CONVERSION;

  data->temp_c      = ((float)raw_temp / 8.0f) + 25.0f;
}


void GPS_Init(void)
{
  // Ensure GPS CS starts HIGH and give the NEO-M8U 100 ms after power-on to boot
  HAL_GPIO_WritePin(GPS_CS_GPIO_Port, GPS_CS_Pin, GPIO_PIN_SET);
  HAL_Delay(100);

  // Transmit the 14-byte UBX-CFG-RATE (10 Hz) packet over SPI
  HAL_GPIO_WritePin(GPS_CS_GPIO_Port, GPS_CS_Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi1, (uint8_t *)ubx_cfg_rate_10hz, sizeof(ubx_cfg_rate_10hz), 10);
  HAL_GPIO_WritePin(GPS_CS_GPIO_Port, GPS_CS_Pin, GPIO_PIN_SET);

  // Give the GNSS engine a brief moment to apply the new rate
  HAL_Delay(20);
}

void Read_GPS(lwgps_t *data)
{
  uint8_t dummy_tx[64];
  uint8_t rx_buf[64];
  memset(dummy_tx, 0xFF, sizeof(dummy_tx));

  // Write 64 blank bytes while reading 64 bytes
  HAL_GPIO_WritePin(GPS_CS_GPIO_Port, GPS_CS_Pin, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive(&hspi1, (uint8_t *)dummy_tx, rx_buf, 64, 5);
  HAL_GPIO_WritePin(GPS_CS_GPIO_Port, GPS_CS_Pin, GPIO_PIN_SET);

  for (uint8_t i = 0; i < 64; i++)
  {
    if (rx_buf[i] != 0xFF)
    {
      lwgps_process(data, &rx_buf[i], 1);
    }
  }
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM_CHANNEL_6)
  {
    read_sensors_flag = 1;
  }
}
/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

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
#ifdef USE_FULL_ASSERT
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
