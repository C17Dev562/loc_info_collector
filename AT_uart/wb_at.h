/*
 * @Author: C17_Dev lijialiangww@gmail.com
 * @Date: 2023-05-23 06:19:45
 * @LastEditors: C17_Dev cone7work98@outlook.com
 * @LastEditTime: 2023-06-28 14:21:42
 * @FilePath: /OpenHarmony-v3.1-Release/applications/sample/wifi-iot/app/wb_app/app/AT_uart/wb_at.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
//
//  UART Part Lib (GPSInfoProcess Included)

//
#ifndef __WB_AT_H__
#define __WB_AT_H__

#include "wb_ram_ex.h"
#include "wb_uart.h"
#include <stdlib.h>
#include <cJSON.h>

// init about - START
#define USING_AT_UART_PORT HI_UART_IDX_1 // 使用的UART口

#define AT_FUN_TASK_STAK_SIZE 4096 // UART的栈大小
#define AT_FUN_TASK_PRIORITY 25
#define AT_UART_BUFF_SIZE 512

#define BUFF_MAX 1024        // 综合缓存长度
#define STR_PRO_BUFF_MAX 512 // 字符串(MQTT)处理缓存长度
// init about - END

// AT CONFIG - START
#define AT_SYN_MAX_TIMES 2      // AT指令同步尝试次数
#define AT_SYN_WAIT_TIME_MS 500 // AT指令同步处理等待时间
#define AT_REP_WAIT_TIME_MS 100 // AT指令处理等待时间
#define AT_RESET_TIMES 5        // AT指令重置恢复次数

#define URC_HEAD_MAX_LEN 15     // URC 信息头最大长度
#define URC_CONT_MAX_LEN 15     // URC 内容最大长度
// AT CONFIG - END

// AT Status - Start



// AT Status - end

typedef struct base_station_info
{
    int id;
    int mcc;
    int mnc;
    unsigned int lac;
    unsigned int ci;
    int rx_lev;

    double lat;
    double lon;
    int dev;

    struct base_station_info *next;
} bsta_info;

int at_init();
int at_deinit(); // the fun is not available
int at_module_init();
void at_entry(void);

#endif
