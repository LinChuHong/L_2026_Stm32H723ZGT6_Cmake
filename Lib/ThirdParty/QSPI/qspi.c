/**
 ****************************************************************************************************
 * @file        qspi.c
 * @version     V1.0
 * @brief       QSPI 驱动代码
 ****************************************************************************************************
 * @attention   Waiken-Smart 慧勤智远
 *
 * 实验平台:    STM32H723ZGT6小系统板
 *
 ****************************************************************************************************
 */
 
#include "qspi.h"


OSPI_HandleTypeDef g_ospi_handle;     /* OSPI句柄 */

/**
 * @brief       等待状态标志
 * @param       flag : 需要等待的标志位
 * @param       sta  : 需要等待的状态
 * @param       wtime: 等待时间
 * @retval      0, 等待成功; 1, 等待失败.
 */
uint8_t ospi_wait_flag(uint32_t flag, uint8_t sta, uint32_t wtime)
{
    uint8_t flagsta = 0;

    while (wtime)
    {
        flagsta = (OCTOSPI1->SR & flag) ? 1 : 0; /* 获取状态标志 */

        if (flagsta == sta)
        {
            break;
        }

        wtime--;
    }

    if (wtime)
    {
        return 0;
    }
    else 
    {
        return 1;
    }
}

/**
 * @brief       初始化OSPI接口
 * @param       无
 * @retval      0, 成功; 1, 失败.
 */
uint8_t ospi_init(void)
{
    uint32_t i;
    char *p;

    p = (char *)&g_ospi_handle;
    
    /* 清零OSPI句柄参数 */
    for (i = 0; i < sizeof(g_ospi_handle); i++)    
    {
        *p++ = 0;
    }	
    
    g_ospi_handle.Instance  = OCTOSPI1;                                     /* 使用OSPI1 */
    HAL_OSPI_DeInit(&g_ospi_handle);                                        /* 复位OSPI1 */	
      
    g_ospi_handle.Init.ClockPrescaler = 2;                                  /* 设置时钟分频值，将OSPI内核时钟进行2分频得到OSPI通信驱动时钟 
                                                                             * 默认选择rcc_hclk3时钟作为OSPI时钟源，则OSPI CLK时钟 = 260M / 2 = 130Mhz,7.7ns
                                                                             */                                                                               
    g_ospi_handle.Init.FifoThreshold = 4;                                   /* FIFO阈值为4个字节 */
    g_ospi_handle.Init.DualQuad = HAL_OSPI_DUALQUAD_DISABLE;                /* 禁止双QSPI模式 */
    g_ospi_handle.Init.MemoryType = HAL_OSPI_MEMTYPE_MICRON;                /* 存储器类型:Micron模式,默认值 */
    g_ospi_handle.Init.DeviceSize = 24;					                            /* 外部存储器大小，W25Q128大小为16M字节，这里设置为24，即2^24字节 */					         
    g_ospi_handle.Init.ChipSelectHighTime = 7;                              /* 片选保持高电平时间为7个时钟周期(7.7*7=54ns),即手册里面的tSHSL参数 */
    g_ospi_handle.Init.FreeRunningClock = HAL_OSPI_FREERUNCLK_DISABLE;      /* 禁止自由运行时钟模式 */
    g_ospi_handle.Init.ClockMode = HAL_OSPI_CLOCK_MODE_3;                   /* 时钟模式3,nCS为高电平时CLK保持高电平 */
    g_ospi_handle.Init.WrapSize	= HAL_OSPI_WRAP_NOT_SUPPORTED;              /* 外部存储器设备不支持回卷读取 */
    g_ospi_handle.Init.SampleShifting = HAL_OSPI_SAMPLE_SHIFTING_HALFCYCLE; /* 采样移位半个周期(当数据阶段使用DTR模式时(DDTR=1),必须设置为0) */
    g_ospi_handle.Init.DelayHoldQuarterCycle = HAL_OSPI_DHQC_DISABLE;       /* 不使用延迟保持功能 */
    g_ospi_handle.Init.ChipSelectBoundary = 0;											        /* 禁止片选边界功能 */
    g_ospi_handle.Init.DelayBlockBypass = HAL_OSPI_DELAY_BLOCK_BYPASSED;    /* 延时模块旁路 */
    g_ospi_handle.Init.MaxTran = 0;                                         /* 禁止通信管理功能 */
    g_ospi_handle.Init.Refresh = 0;                                         /* 禁止刷新功能 */

    if (HAL_OSPI_Init(&g_ospi_handle) != HAL_OK)                            /* 初始化OSPI配置 */
    {
        return 1;      /* OSPI初始化不成功 */
    }
    
    return 0;    
}

/**
 * @brief       OSPI底层驱动，引脚配置，时钟使能
 * @param       hospi:OSPI句柄
 * @note        此函数会被HAL_OSPI_Init()调用
 * @retval      无
 */
void HAL_OSPI_MspInit(OSPI_HandleTypeDef *hospi)
{
    GPIO_InitTypeDef gpio_init_struct;
    OSPIM_CfgTypeDef ospim_cfg_struct = {0};

		__HAL_RCC_OSPI1_CLK_ENABLE();       /* 使能OSPI1时钟 */
		__HAL_RCC_OCTOSPIM_CLK_ENABLE();		/* 使能OSPIM时钟 */
    OSPIM_P1_CLK_GPIO_CLK_ENABLE();     /* OSPIM_P1_CLK IO口时钟使能 */
    OSPIM_P1_NCS_GPIO_CLK_ENABLE();     /* OSPIM_P1_NCS IO口时钟使能 */
    OSPIM_P1_IO0_GPIO_CLK_ENABLE();     /* OSPIM_P1_IO0 IO口时钟使能 */
    OSPIM_P1_IO1_GPIO_CLK_ENABLE();     /* OSPIM_P1_IO1 IO口时钟使能 */
    OSPIM_P1_IO2_GPIO_CLK_ENABLE();     /* OSPIM_P1_IO2 IO口时钟使能 */
    OSPIM_P1_IO3_GPIO_CLK_ENABLE();     /* OSPIM_P1_IO3 IO口时钟使能 */
  
    gpio_init_struct.Pin = OSPIM_P1_CLK_GPIO_PIN;               /* OSPIM_P1_CLK引脚 */
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;                    /* 复用推挽输出 */
    gpio_init_struct.Pull = GPIO_PULLUP;                        /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;         /* 高速 */
    gpio_init_struct.Alternate = OSPIM_P1_CLK_GPIO_AF;          /* 复用为OSPIM_P1_CLK */
    HAL_GPIO_Init(OSPIM_P1_CLK_GPIO_PORT, &gpio_init_struct);   /* 初始化OSPIM_P1_CLK引脚 */

    gpio_init_struct.Pin = OSPIM_P1_NCS_GPIO_PIN;               /* OSPIM_P1_NCS引脚 */
    gpio_init_struct.Alternate = OSPIM_P1_NCS_GPIO_AF;          /* 复用为OSPIM_P1_NCS */
    HAL_GPIO_Init(OSPIM_P1_NCS_GPIO_PORT, &gpio_init_struct);   /* 初始化OSPIM_P1_NCS引脚 */

    gpio_init_struct.Pin = OSPIM_P1_IO0_GPIO_PIN;               /* OSPIM_P1_IO0引脚 */
    gpio_init_struct.Alternate = OSPIM_P1_IO0_GPIO_AF;          /* 复用为OSPIM_P1_IO0 */
    HAL_GPIO_Init(OSPIM_P1_IO0_GPIO_PORT, &gpio_init_struct);   /* 初始化OSPIM_P1_IO0引脚 */
   
    gpio_init_struct.Pin = OSPIM_P1_IO1_GPIO_PIN;               /* OSPIM_P1_IO1引脚 */
    gpio_init_struct.Alternate = OSPIM_P1_IO1_GPIO_AF;          /* 复用为OSPIM_P1_IO1 */
    HAL_GPIO_Init(OSPIM_P1_IO1_GPIO_PORT, &gpio_init_struct);   /* 初始化OSPIM_P1_IO1引脚 */   

    gpio_init_struct.Pin = OSPIM_P1_IO2_GPIO_PIN;               /* OSPIM_P1_IO2引脚 */
    gpio_init_struct.Alternate = OSPIM_P1_IO2_GPIO_AF;          /* 复用为OSPIM_P1_IO2 */
    HAL_GPIO_Init(OSPIM_P1_IO2_GPIO_PORT, &gpio_init_struct);   /* 初始化OSPIM_P1_IO2引脚 */
    
    gpio_init_struct.Pin = OSPIM_P1_IO3_GPIO_PIN;               /* OSPIM_P1_IO3引脚 */
    gpio_init_struct.Alternate = OSPIM_P1_IO3_GPIO_AF;          /* 复用为OSPIM_P1_IO3 */
    HAL_GPIO_Init(OSPIM_P1_IO3_GPIO_PORT, &gpio_init_struct);   /* 初始化OSPIM_P1_IO3引脚 */
    
    ospim_cfg_struct.ClkPort = 1;                               /* OSPI1_CLK使用OSPIM端口1的CLK */
    ospim_cfg_struct.NCSPort = 1;                               /* OSPI1_NCS使用OSPIM端口1的NCS */
    ospim_cfg_struct.IOLowPort = HAL_OSPIM_IOPORT_1_LOW;        /* OSPI1_IO[3:0]使用OSPIM端口1的IO[3:0] */

    HAL_OSPIM_Config(&g_ospi_handle, &ospim_cfg_struct, HAL_OSPI_TIMEOUT_DEFAULT_VALUE);  /* 配置OSPI引脚分配管理器 */
}

/**
 * @brief       OSPI发送命令
 * @param       cmd : 要发送的指令
 * @param       addr: 发送到的目的地址
 * @param       mode: 模式,详细位定义如下:
 *   @arg       mode[2:0]:  指令模式; 000,无指令;  001,单线传输指令; 010,双线传输指令; 011,四线传输指令; 100,八线传输指令.
 *   @arg       mode[5:3]:  地址模式; 000,无地址;  001,单线传输地址; 010,双线传输地址; 011,四线传输地址; 100,八线传输地址.
 *   @arg       mode[7:6]:  地址长度; 00,8位地址;   01,16位地址;      10,24位地址;      11,32位地址.
 *   @arg       mode[10:8]: 数据模式; 000,无数据;  001,单线传输数据; 010,双线传输数据; 011,四线传输数据; 100,八线传输数据.
 * @param       dmcycle: 空指令周期数
 * @retval      无
 */
void ospi_send_cmd(uint8_t cmd, uint32_t addr, uint16_t mode, uint8_t dmcycle)
{
  	OSPI_RegularCmdTypeDef sCommand;	                               /* OSPI常规命令结构体 */

    sCommand.OperationType = HAL_OSPI_OPTYPE_COMMON_CFG;             /* 通用配置(间接模式或自动轮询模式下使用) */
    sCommand.FlashId = HAL_OSPI_FLASH_ID_1;                          /* 选择FLASH1 */
    sCommand.Instruction = cmd;                                      /* 设置要发送的指令 */

    if(((mode >> 0) & 0x07) == 0)
    sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_NONE;            /* 指令模式 */
    else if(((mode >> 0) & 0x07) == 1)
    sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;          /* 指令模式 */
    else if(((mode >> 0) & 0x07) == 2)
    sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_2_LINES;         /* 指令模式 */
    else if(((mode >> 0) & 0x07) == 3)
    sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_4_LINES;         /* 指令模式 */    
    else if(((mode >> 0) & 0x07) == 4)
    sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_8_LINES;         /* 指令模式 */    
    
    sCommand.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;          /* 指令长度为8位 */
    sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;  /* 禁止指令阶段DTR模式 */
    sCommand.Address = addr;                                         /* 设置要发送的地址 */
    
    if(((mode >> 3) & 0x07) == 0)
    sCommand.AddressMode = HAL_OSPI_ADDRESS_NONE;                    /* 地址模式 */
    else if(((mode >> 3) & 0x07) == 1)
    sCommand.AddressMode = HAL_OSPI_ADDRESS_1_LINE;                  /* 地址模式 */
    else if(((mode >> 3) & 0x07) == 2)
    sCommand.AddressMode = HAL_OSPI_ADDRESS_2_LINES;                 /* 地址模式 */
    else if(((mode >> 3) & 0x07) == 3)
    sCommand.AddressMode = HAL_OSPI_ADDRESS_4_LINES;                 /* 地址模式 */    
    else if(((mode >> 3) & 0x07) == 4)
    sCommand.AddressMode = HAL_OSPI_ADDRESS_8_LINES;                 /* 地址模式 */  

    if(((mode >> 6) & 0x03) == 0)
    sCommand.AddressSize = HAL_OSPI_ADDRESS_8_BITS;                  /* 地址长度 */
    else if(((mode >> 6) & 0x03) == 1)
    sCommand.AddressSize = HAL_OSPI_ADDRESS_16_BITS;                 /* 地址长度 */
    else if(((mode >> 6) & 0x03) == 2)
    sCommand.AddressSize = HAL_OSPI_ADDRESS_24_BITS;                 /* 地址长度 */
    else if(((mode >> 6) & 0x03) == 3) 
    sCommand.AddressSize = HAL_OSPI_ADDRESS_32_BITS;                 /* 地址长度 */   
  
    sCommand.AddressDtrMode = HAL_OSPI_ADDRESS_DTR_DISABLE;          /* 禁止地址阶段DTR模式 */    
    sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;     /* 无交替字节 */         

    if(((mode >> 8) & 0x07) == 0)
    sCommand.DataMode = HAL_OSPI_DATA_NONE;                          /* 数据模式 */
    else if(((mode >> 8) & 0x07) == 1)
    sCommand.DataMode = HAL_OSPI_DATA_1_LINE;                        /* 数据模式 */
    else if(((mode >> 8) & 0x07) == 2)
    sCommand.DataMode = HAL_OSPI_DATA_2_LINES;                       /* 数据模式 */
    else if(((mode >> 8) & 0x07) == 3)
    sCommand.DataMode = HAL_OSPI_DATA_4_LINES;                       /* 数据模式 */    
    else if(((mode >> 8) & 0x07) == 4)
    sCommand.DataMode = HAL_OSPI_DATA_8_LINES;                       /* 数据模式 */     
    
    sCommand.DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE;                /* 禁止数据阶段DTR模式 */
    sCommand.DummyCycles = dmcycle;                                  /* 设置空指令周期数 */
    sCommand.DQSMode = HAL_OSPI_DQS_DISABLE;                         /* 不使用DQS */
    sCommand.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;                /* 每次都发送指令 */   

    HAL_OSPI_Command(&g_ospi_handle, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE);  /* 设置OSPI命令配置参数 */
}

/**
 * @brief       OSPI接收指定长度的数据
 * @param       buf     : 接收数据缓冲区首地址
 * @param       datalen : 要传输的数据长度
 * @retval      0, 成功; 1, 失败.
 */
uint8_t ospi_receive(uint8_t *buf, uint32_t datalen)
{
    g_ospi_handle.Instance->DLR = datalen - 1;   /* 配置数据传输长度 */
  
    if (HAL_OSPI_Receive(&g_ospi_handle, buf, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) == HAL_OK) 
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

/**
 * @brief       OSPI发送指定长度的数据
 * @param       buf     : 发送数据缓冲区首地址
 * @param       datalen : 要传输的数据长度
 * @retval      0, 成功; 1, 失败.
 */
uint8_t ospi_transmit(uint8_t *buf, uint32_t datalen)
{
    g_ospi_handle.Instance->DLR = datalen - 1;   /* 配置数据传输长度 */
  
    if (HAL_OSPI_Transmit(&g_ospi_handle, buf, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) == HAL_OK)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}













