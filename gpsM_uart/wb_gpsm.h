//
//  UART Part Lib (GPSInfoProcess Included)

//
#ifndef __WB_GPSM_H__
#define __WB_GPSM_H__

#include "wb_uart.h"
#include "wb_event.h"
#include "wb_tools.h"
#define WRITE_BY_INT
#define GPS_FUN_TASK_STAK_SIZE 4096   //UART的栈大小
#define GPS_FUN_TASK_PRIORITY  25
#define USING_UART_PORT         HI_UART_IDX_2
#define GPS_UART_BUFF_SIZE          512

#define BUFF_MAX 1024


int gpsM_init();
void gpsM_deinit(); //the fun is not available
void gpsM_entry(void);

#endif
