/*
 * @Author: C17_Dev cone7work98@outlook.com
 * @Date: 2023-04-14 14:29:05
 * @LastEditors: C17_Dev cone7work98@outlook.com
 * @LastEditTime: 2023-06-30 13:24:11
 * @FilePath: /OpenHarmony-v3.1-Release/applications/sample/wifi-iot/app/wb_app/app/gpsM_uart/wb_gpsm.c
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

#include "wb_gpsm.h"
#include "nmea/nmea.h"
#define MAX_GPS_FAIL_TIMES 100
//buff setup

hi_u8* g_buff = HI_NULL;
hi_s32 buff_len = 0;

hi_u8* g_uart_buff = HI_NULL;
hi_s32 uBuff_len = 0;

//buff end


hi_u32 point = 0;   //buff's point (num point)
//nmea - start
nmeaINFO* g_info = HI_NULL;
nmeaPARSER* g_parser = HI_NULL;
//nmea - end


hi_u32 g_uart_task_id = 0;

    int loc_value_sign = 0; //locate info value sign 0:invalid 1:valid

/// @brief init the uart port and memony , but uart processing not started
/// @return 0 - OK ; -1 - UART Init Fail ; -2 - MEMONY Init Fail
int gpsM_init(){

    hi_io_set_func(HI_IO_NAME_GPIO_11, HI_IO_FUNC_GPIO_11_UART2_TXD); /* uart2 tx */
    hi_io_set_func(HI_IO_NAME_GPIO_12, HI_IO_FUNC_GPIO_12_UART2_RXD); /* uart2 rx */

    hi_u32 ret;
    hi_uart_attribute uart_attr = {
        .baud_rate = 9600, /* baud_rate: 9600 */
        .data_bits = 8,    /* data_bits: 8bits */
        .stop_bits = 1,
        .parity = 0,
    };

    /* Initialize uart driver */
    ret = hi_uart_init(USING_UART_PORT, &uart_attr, HI_NULL);
    if (ret != HI_ERR_SUCCESS)
    {
        printf("[sys_info] Failed to init uart! Err code = %d\n", ret);
        return -1;
    }

    //malloc the memony
    g_buff = hi_malloc(HI_MOD_ID_APP_COMMON,BUFF_MAX);
    g_uart_buff = hi_malloc(HI_MOD_ID_APP_COMMON,GPS_UART_BUFF_SIZE);
    g_info = hi_malloc(HI_MOD_ID_APP_COMMON, sizeof(nmeaINFO)+1 );
    g_parser = hi_malloc(HI_MOD_ID_APP_COMMON, sizeof(nmeaPARSER)+1  );
    printf("uart memony status: %d %d %d %d\n",g_buff,g_uart_buff,g_info,g_parser);
    if( g_buff == HI_NULL || g_uart_buff == HI_NULL || g_info == HI_NULL || g_parser == HI_NULL){
        gpsM_deinit();
        return -2;
    }

    return 0;
}


/// @brief 去初始化gps模块
void gpsM_deinit(){
    if( g_buff == HI_NULL || g_uart_buff == HI_NULL || g_info == HI_NULL || g_parser == HI_NULL){
        if( g_buff != HI_NULL )
            hi_free(HI_MOD_ID_APP_COMMON,g_buff);
        if( g_uart_buff != HI_NULL )
            hi_free(HI_MOD_ID_APP_COMMON,g_uart_buff);
        if( g_info != HI_NULL )
            hi_free(HI_MOD_ID_APP_COMMON,g_info);
        if( g_parser != HI_NULL )
            hi_free(HI_MOD_ID_APP_COMMON,g_parser);
    }
}

/// @brief 双模混合GPS坐标纠偏
/// @param input 输入的gps经度/纬度
/// @return 纠偏过的gps经度/纬度
double gps_Correction(double input){
    int l = input/100;
    double r = (input-l*100)/60+l;
    return r;
}

static hi_void *gpsM_task(hi_void *param)
{
    hi_u8 *uart_buff = g_uart_buff;
    hi_u8* buff = g_buff;
    nmeaINFO* info = g_info;
    nmeaPARSER* parser = g_parser;

    hi_unref_param(param);

    printf("gpsM_task starting...\n");

    printf("uart memony status: %d %d %d %d\n",uart_buff,buff,info,parser);

    nmea_zero_INFO(info);
    nmea_parser_init(parser);

    int num_of_list = 0;
    int buff_p = 0;
    int gps_info_send_status = 0;
    int flag = 0;

    loc_value_sign = 0;

    for (int fail_times = MAX_GPS_FAIL_TIMES; fail_times > 0 ; fail_times --)
    {
        // DBprintf("uart loop +1\n");

        flag = uart_read_nio(USING_UART_PORT, uart_buff, GPS_UART_BUFF_SIZE , &uBuff_len);

        // if( flag == UART_READ_FALI )
        // {
        //     point = 0;
        //     continue;
        // }

        // DBprintf("uart readlen:%d\n",uBuff_len);

        if (uBuff_len > 0)
        {
            // DBprintf("uart read +1\n");
            fail_times = MAX_GPS_FAIL_TIMES;
            if (uart_buff[0] != '$') // Is it new data
            {
                // DBprintf("uart effect +1\n");
                // not a new data
                if (point != 0) // Buffer is not empty
                {
                    if (point >= BUFF_MAX - 1)
                    { // Buffer full, discard data
                        point = 0;
                        continue;
                    }
                    // Data is good to go
                }
                else
                { // Buffer is empty but the data is not new,discard it
                    continue;
                }
            }
            else
            { // a new data income
                point = 0;
                if (uBuff_len >= BUFF_MAX - 1) // Buffer full, discard data
                    continue;
            }
            // test start

            strncpy(&buff[point], uart_buff, uBuff_len);
            point = point + uBuff_len; // the point always point to the next unused data point

            if (buff[point - 1] == '\n')
            {
                // DBprintf("uart read a available gps info +1\n");
                buff_len = point;
                /* data process */
                // printf("gps_info:%.*s|%d\n", buff_len, buff,buff_len);

                nmea_parse(parser, buff, buff_len, info , &loc_value_sign);

                // DBprintf("gps info parse in +1\n");

                if( loc_value_sign != 0 ){
                    // DBprintf("gps info push to mqtt +1\n");
                    gip_globe.lat = gps_Correction(info->lat);
                    gip_globe.lon = gps_Correction(info->lon);
                    gip_globe.status = GPS_STATUS_OK;
                    gps_mqtt_event_send(GPS_INFO_SUCESS);
                    // DBprintf("gps info push to mqtt successed +1\n");
                    /* gps info share code */
                    osDelay(100);
                    point = 0;
                    continue;
                }
                // DBprintf("gps info push to mqtt fail +1\n");
                gip_globe.status = GPS_STATUS_FAIL;
                gps_info_send_status = gps_mqtt_event_send(GPS_INFO_Fail);
                // DBprintf("gps info push status : %d\n",gps_info_send_status);
                point = 0;
            }
        }
        else
        {
            wb_osDelay(500);
        }
    }

    gpsM_deinit();
    hi_task_delete(g_uart_task_id);
    g_uart_task_id = 0;

    return HI_NULL;
}


/// @brief uart task entry ; create a thread for UART
/// @param
void gpsM_entry(void)
{

    osThreadAttr_t attr;

    attr.name = "uart_entry";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = GPS_FUN_TASK_STAK_SIZE;
    attr.priority = GPS_FUN_TASK_PRIORITY;

    if (osThreadNew((osThreadFunc_t)gpsM_task, HI_NULL, &attr) == NULL)
    {
        printf("[sys_info] Falied to create uart_task!\n");
    }
}

// SYS_RUN(uart_entry);

//UART FOR GPS MODLE END


