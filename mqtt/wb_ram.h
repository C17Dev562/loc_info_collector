/*
 * @Author: C17_Dev lijialiangww@gmail.com
 * @Date: 2023-04-21 07:27:36
 * @LastEditors: C17_Dev cone7work98@outlook.com
 * @LastEditTime: 2023-06-30 13:23:03
 * @FilePath: /OpenHarmony-v3.1-Release/applications/sample/wifi-iot/app/wb_app/app/mqtt/wb_ram.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/*
 * @Author: C17_Dev lijialiangww@gmail.com
 * @Date: 2023-04-21 07:27:36
 * @LastEditors: C17_Dev lijialiangww@gmail.com
 * @LastEditTime: 2023-05-23 13:29:02
 * @FilePath: /OpenHarmony-v3.1-Release/applications/sample/wifi-iot/app/wb_app/app/mqtt/wb_ram.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _RAM_H_
#define _RAW_H_

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <hi_types_base.h>
#include <hi_mem.h>

#include "cmsis_os2.h"

#include "wb_config.h"


typedef struct gps_info{
    double lat;
    double lon;
    int status;
} gps_info_pak;

typedef enum
{
    GPS_STATUS_ERROR = -2,
    GPS_STATUS_FAIL,
    GPS_STATUS_OK
}GPS_STATUS;

// extern gps_info_pak gip_globe;

void wb_ram_init();

#define LMI_STR_LEN 25 //revision's len more than 20

typedef struct lte_module_info{
    char* brand;
    char* modle;
    char* revision;
}lte_m_info;

// lte_module_status enum - START
typedef enum
{
    AT_SYN_ERR = -2,
    AT_SYN_FAIL,
    AT_SYN_OK
}AT_SYN;

typedef enum
{
    AT_ATV_0,
    AT_ATV_1,
    AT_ATV_UNKNOW
}AT_ATV;

typedef enum
{
    AT_ATE_0,
    AT_ATE_1,
    AT_ATE_UNKNOW
}AT_ATE;

typedef enum
{
    AT_CMD_ERR = -5,
    AT_CMD_FALI,
    AT_CMD_OK,
    AT_CMD_WAIT_DATA,
} AT_CMD;

typedef enum
{
    AT_CREG_ERR = -2,
    AT_CREG_FALI,
    AT_CREG_OK
} AT_CREG;

// lte_module_status enum - END

typedef struct lte_module_status{
    hi_u8 at_SYN;
    hi_u8 at_ATV;
    hi_u8 at_ATE;
}lte_m_status;



/* oled 第一页的信息编码 */

typedef enum
{
    OLEDISP_P1_SI_S_UNKNOW,
    OLEDISP_P1_SI_S_WAIT,
    OLEDISP_P1_SI_S_COMP
}oled_p1_si_status;

typedef struct oledisp_p1_status_info{
    hi_u8 status;
}oledisp_p1_si;

/* oled 页面信息 */

typedef struct {
    hi_u8 page;//oled页面位置
    oledisp_p1_si p1si;//第一页的信息
}oledisp_status;

#endif
