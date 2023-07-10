/*
 * @Author: C17_Dev cone7work98@outlook.com
 * @Date: 2023-04-14 14:29:05
 * @LastEditors: C17_Dev cone7work98@outlook.com
 * @LastEditTime: 2023-07-10 06:59:22
 * @FilePath: /OpenHarmony-v3.1-Release/applications/sample/wifi-iot/app/wb_app/app/app.c
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

#include "app.h"

void app_entry(void)
{
    wb_tools_init();
    wb_osDelay(10000);
    printf("gps_mqtt_event code:%d\n",gps_mqtt_event_init());
    printf("gpsM_init code:%d\n",gpsM_init());
    wb_ram_init();
    printf("oled_init code:%d\n",oled_entry());
    gpsM_entry();

    int flag = 0;
    for( int i = 0; i< AT_SYN_MAX_TIMES;i++)
    {
        wb_osDelay(1000);
        if ( at_init() != 0 )
            continue;
        if ( at_module_init() != AT_SYN_OK )
            continue;
        flag = 1;
        break;
    }

    DBprintf("4G Init Status:%d\n",flag);
    if ( flag == 1 )
        at_entry();

}

APP_FEATURE_INIT(app_entry);
