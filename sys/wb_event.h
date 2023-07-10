#ifndef _WB_TASK_H_
#define _WB_TASK_H_

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include <hi_types_base.h>

#include "wb_ram_ex.h"

//message/wati_bit define
#define GPS_INFO_SUCESS 0x00000001U
#define GPS_INFO_Fail   0x00000002U
#define GPS_INFO_TIMEOUT osFlagsErrorTimeout
#define GPS_INFO_UNKNOW osFlagsErrorParameter

#define GPS_WAITALL_BIT 0x00000003U

extern osEventFlagsId_t gps_mqtt_pubID;

int gps_mqtt_event_init();
int gps_mqtt_event_send( hi_u32 message );
uint32_t gps_mqtt_event_wait( uint32_t wait_ms );


#endif
