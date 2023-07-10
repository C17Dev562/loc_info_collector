/*
 * @Author: C17_Dev lijialiangww@gmail.com
 * @Date: 2023-04-18 12:27:16
 * @LastEditors: C17_Dev cone7work98@outlook.com
 * @LastEditTime: 2023-06-28 14:47:28
 * @FilePath: /OpenHarmony-v3.1-Release/applications/sample/wifi-iot/app/wb_app/app/uart/wb_uart.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
//
//  UART Part Lib (GPSInfoProcess Included)

//
#ifndef __WB_UART_H__
#define __WB_UART_H__

#include <stdio.h>
#include <unistd.h>

#include <hi_types_base.h>
#include <hi_mem.h>

#include "cmsis_os2.h"

#include <string.h>

#include <hi_io.h>
#include <hi_uart.h>

#include "wb_event.h"
#include "wb_ram_ex.h"
// #include "tools.h"
#include "wb_config.h"
#include "wb_tools.h"

hi_s32 uart_send_nio(hi_uart_idx id,const hi_u8 *data, hi_u32 data_len);

hi_s32 uart_read_nio(hi_uart_idx id, hi_u8 *data, hi_u32 data_len , hi_u32 *read_len);

hi_s32 uart_read_wait(hi_uart_idx id, hi_u8 *data, hi_u32 data_len,hi_u32 waittime);

typedef enum
{   UART_READ_BUFFFULL = -2,    //缓存已满,但仍有数据等待读取
    UART_READ_FALI,
    UART_READ_S_OK ,    //当读取数据需要直接返回时不要使用
} UART_READ;

#endif
