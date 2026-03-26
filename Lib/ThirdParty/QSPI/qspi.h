/**
 ****************************************************************************************************
 * @file        qspi.h
 * @version     V1.0
 * @brief       QSPI 驱动代码
 ****************************************************************************************************
 * @attention   Waiken-Smart 慧勤智远
 *
 * 实验平台:    STM32H723ZGT6小系统板
 *
 ****************************************************************************************************
 */

#ifndef __QSPI_H
#define __QSPI_H
#include "sys/sys.h"
#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************************/
/* OSPI 相关 引脚 定义 */

#define OSPIM_P1_CLK_GPIO_PORT          GPIOF
#define OSPIM_P1_CLK_GPIO_PIN           GPIO_PIN_10
#define OSPIM_P1_CLK_GPIO_AF            GPIO_AF9_OCTOSPIM_P1
#define OSPIM_P1_CLK_GPIO_CLK_ENABLE()  do{ __HAL_RCC_GPIOF_CLK_ENABLE(); }while(0)   /* PF口时钟使能 */

#define OSPIM_P1_NCS_GPIO_PORT          GPIOB
#define OSPIM_P1_NCS_GPIO_PIN           GPIO_PIN_6
#define OSPIM_P1_NCS_GPIO_AF            GPIO_AF10_OCTOSPIM_P1
#define OSPIM_P1_NCS_GPIO_CLK_ENABLE()  do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PB口时钟使能 */

#define OSPIM_P1_IO0_GPIO_PORT          GPIOF
#define OSPIM_P1_IO0_GPIO_PIN           GPIO_PIN_8
#define OSPIM_P1_IO0_GPIO_AF            GPIO_AF10_OCTOSPIM_P1
#define OSPIM_P1_IO0_GPIO_CLK_ENABLE()  do{ __HAL_RCC_GPIOF_CLK_ENABLE(); }while(0)   /* PF口时钟使能 */

#define OSPIM_P1_IO1_GPIO_PORT          GPIOF
#define OSPIM_P1_IO1_GPIO_PIN           GPIO_PIN_9
#define OSPIM_P1_IO1_GPIO_AF            GPIO_AF10_OCTOSPIM_P1
#define OSPIM_P1_IO1_GPIO_CLK_ENABLE()  do{ __HAL_RCC_GPIOF_CLK_ENABLE(); }while(0)   /* PF口时钟使能 */

#define OSPIM_P1_IO2_GPIO_PORT          GPIOF
#define OSPIM_P1_IO2_GPIO_PIN           GPIO_PIN_7
#define OSPIM_P1_IO2_GPIO_AF            GPIO_AF10_OCTOSPIM_P1
#define OSPIM_P1_IO2_GPIO_CLK_ENABLE()  do{ __HAL_RCC_GPIOF_CLK_ENABLE(); }while(0)   /* PF口时钟使能 */

#define OSPIM_P1_IO3_GPIO_PORT          GPIOF
#define OSPIM_P1_IO3_GPIO_PIN           GPIO_PIN_6
#define OSPIM_P1_IO3_GPIO_AF            GPIO_AF10_OCTOSPIM_P1
#define OSPIM_P1_IO3_GPIO_CLK_ENABLE()  do{ __HAL_RCC_GPIOF_CLK_ENABLE(); }while(0)   /* PF口时钟使能 */

/******************************************************************************************/

uint8_t ospi_wait_flag(uint32_t flag, uint8_t sta, uint32_t wtime);              /* OSPI等待某个状态 */
uint8_t ospi_init(void);                                                         /* 初始化OSPI */
void ospi_send_cmd(uint8_t cmd, uint32_t addr, uint16_t mode, uint8_t dmcycle);  /* OSPI发送命令 */
uint8_t ospi_receive(uint8_t *buf, uint32_t datalen);                            /* OSPI接收数据 */
uint8_t ospi_transmit(uint8_t *buf, uint32_t datalen);                           /* OSPI发送数据 */


#ifdef __cplusplus
}
#endif

#endif













