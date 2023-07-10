#include "wb_event.h"


osEventFlagsId_t gps_mqtt_pubID; // gps info mqtt send event


/// @brief GPS MQTT INFO SEND
/// @return 0 success ; -1 fail
int gps_mqtt_event_init()
{
    gps_mqtt_pubID = osEventFlagsNew(NULL);
    if (gps_mqtt_pubID == NULL)
    {
        printf("Falied to create gps_mqtt_event!\n");
        return -1;
    }
    return 0;
}

/// @brief send a event to the mqtt thread for starting process
/// @return -1:the event handle NULL ; -2:EventSet Fail ; 0:Success ;
int gps_mqtt_event_send( hi_u32 message ){
    if (gps_mqtt_pubID == NULL)
    {
        return -1;
    }
    if( osEventFlagsSet(gps_mqtt_pubID,message) == osFlagsErrorParameter ){
        return -2;
    }
    return 0;
}

/// @brief
/// @return
uint32_t gps_mqtt_event_wait( uint32_t wait_ms ){
    double wait_s = (double)wait_ms / 1000;
    uint32_t wait_tick = (uint32_t)(osKernelGetTickFreq()*wait_s);
    if (gps_mqtt_pubID == NULL)
    {
        return -1;
    }
    uint32_t status = osEventFlagsWait(gps_mqtt_pubID, GPS_WAITALL_BIT, osFlagsWaitAny, wait_tick );
    if( status == GPS_INFO_TIMEOUT){
        gip_globe.status = GPS_INFO_TIMEOUT;
    }
    return status;
}
