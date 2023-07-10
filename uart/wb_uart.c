/*
 * @Author: C17_Dev cone7work98@outlook.com
 * @Date: 2023-04-14 14:29:05
 * @LastEditors: C17_Dev cone7work98@outlook.com
 * @LastEditTime: 2023-06-29 04:08:53
 * @FilePath: /OpenHarmony-v3.1-Release/applications/sample/wifi-iot/app/wb_app/app/uart/wb_uart.c
 * @Description:
 *
 *
 * Copyright (c) 2023 by C17_Dev cone7work98@outlook.com, All Rights Reserved.
 */
/*
 * Copyright (c) 2020 HiSilicon (Shanghai) Technologies CO., LIMITED.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "wb_uart.h"
// #include "nmea/nmea.h"

/// @brief UART发送函数
/// @param id 使用的UART口编号
/// @param data 需要发送的数据
/// @param data_len 需要发送的数据长度
/// @return 返回原生函数的返回值 hi_uart_write
hi_s32 uart_send_nio(hi_uart_idx id,const hi_u8 *data, hi_u32 data_len)
{
    return hi_uart_write(id, data, data_len);
}

/// @brief 非阻塞的uart读取函数，以hi_uart_read为基础
/// @param id   选择的uart端口
/// @param data 读出的缓冲区
/// @param data_len 读取的最大长度
/// @param read_len 实际读取的长度
/// @return  UART_READ_BUFFFULL 缓存不足 ；HI_ERR_FAILURE-读取失败 ；UART_READ_S_OK - 读取成功
hi_s32 uart_read_nio(hi_uart_idx id, hi_u8 *data, hi_u32 data_len , hi_u32 *read_len)
{
    hi_s32 read_status = 0;
    hi_u32 p = data_len;
    hi_bool flag = 0;
    *read_len = 0;  //重置读取长度
    while (1)
    {
        if ( hi_uart_is_buf_empty(id, &flag) == HI_ERR_FAILURE)
            return UART_READ_FALI;  //检测出现问题

        if (data_len <= 0 )   //数据满了
        {
            if( flag == HI_TRUE )
                return p - data_len;  //数据刚好够存下,返回读取的长度
            else
                return UART_READ_BUFFFULL;  //缓存不够大,返回状态符
        }

        if ( flag == HI_FALSE )
        {
            read_status = hi_uart_read(id, data, data_len);
            if (read_status == HI_ERR_FAILURE)
                return UART_READ_FALI;  //读取出现问题

            data += read_status;    //更改记录标签
            data_len -= read_status;

            continue;
        }

        *read_len = p - data_len;
        return UART_READ_S_OK;
    }
}

/// @brief 阻塞的uart读取函数，以hi_uart_read为基础
/// @param id   选择的uart端口
/// @param data 读出的缓冲区
/// @param data_len 读取的最大长度
/// @return  >=0 实际读取的字节数 ; UART_READ_BUFFFULL 缓存不足 ; HI_ERR_FAILURE-读取失败 ;
hi_s32 uart_read_wait(hi_uart_idx id, hi_u8 *data, hi_u32 data_len,hi_u32 waittime)
{
    hi_s32 read_status = 0;
    hi_u32 p = data_len;
    hi_bool flag = 0;
    hi_u32 read_len = 0;
    int loop = 0;
    while ( loop < 5 )
    {

        if ( hi_uart_is_buf_empty(id, &flag) == HI_ERR_FAILURE)
            return UART_READ_FALI;  //检测出现问题

        if (data_len <= 0 )   //数据满了
        {
            if( flag == HI_TRUE )
                return p - data_len;  //数据刚好够存下,返回读取的长度
            else
                return UART_READ_BUFFFULL;  //缓存不够大,返回状态符
        }
        DBprintf("uart flag:%d",flag);
        if ( flag == HI_FALSE ) //数据非空
        {
            read_status = hi_uart_read(id, data, data_len);
            if (read_status == HI_ERR_FAILURE)
                return UART_READ_FALI;  //读取出现问题

            data += read_status;    //更改记录标签
            data_len -= read_status;

            continue;
        }

        //数据为空

        read_len = p - data_len;

        if( read_len == 0 ){    //啥都没得，等！
            wb_osDelay(waittime);
            loop++;
            continue;
        }

        return read_len;
    }
    return read_len;
}
