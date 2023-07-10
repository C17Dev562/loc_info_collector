/*
 * @Author: C17_Dev lijialiangww@gmail.com
 * @Date: 2023-04-21 11:52:31
 * @LastEditors: C17_Dev cone7work98@outlook.com
 * @LastEditTime: 2023-05-22 09:24:53
 * @FilePath: /OpenHarmony-v3.1-Release/applications/sample/wifi-iot/app/wb_app/app/OLED_FUN/oled.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _OLED_H_
#define _OLED_H_

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <hi_mem.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "hi_io.h"

#include "iot_gpio.h"
#include "iot_pwm.h"
#include "iot_i2c.h"
#include "iot_errno.h"
#include "ssd1306.h"

#include "wb_event.h"
#include "wb_ram_ex.h"

#include "wb_config.h"

#define OLED_I2C_BAUDRATE 400*1000
#define STRING_CACHE_MAXLEN 20
#define OLED_TASK_STAK_SIZE 4096   //UART的栈大小
#define OLED_TASK_PRIORITY  24

#define OLED_GPS_STATUS_FAIL 0
#define OLED_GPS_STATUS_SUCCESS 1
#define OLED_GPS_STATUS_TIMEOUT 2
#define OLED_GPS_STATUS_UNKNOW 3



int oled_entry();

#endif
